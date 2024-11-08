#include "miko.std.syntaxor.h"
#include <exbuffer.h>

// type

typedef enum miko_std_syntax_reader_flag_t miko_std_syntax_reader_flag_t;
typedef struct miko_std_syntax_reader_stack_t miko_std_syntax_reader_stack_t;
typedef struct miko_std_syntax_reader_inode_t miko_std_syntax_reader_inode_t;
typedef struct miko_std_syntax_reader_data_t miko_std_syntax_reader_data_t;
typedef struct miko_std_syntax_reader_t miko_std_syntax_reader_t;

// | [ 1] -> [ 2] |> [ 3] -> [ 4]                                 => [12] |>      => |
// |              |> [ 5] -> [ 6] -> [ 7] |>              => [10] =>      |> [13] => |
// |                                      |> [ 8] -> [ 9] =>                         |
// |              |> [11]                                         =>                 |
// | inode group_id okaynext rollback back_gid |
// |  [ 0]      (0)     null     null      (0) |
// |  [ 1]      (1)     [ 2]     none      (0) |
// |  [ 2]      (1)     [ 3]     none      (0) |
// |  [ 3]      (2)     [ 4]     [ 5]      (1) |
// |  [ 4]      (2)     [12]     [ 5]      (1) |
// |  [ 5]      (3)     [ 6]     [11]      (1) |
// |  [ 6]      (3)     [ 7]     [11]      (1) |
// |  [ 7]      (3)     [ 8]     [11]      (1) |
// |  [ 8]      (4)     [ 9]     [10]      (3) |
// |  [ 9]      (4)     [10]     [10]      (3) |
// |  [10]      (5)     [12]     [11]      (1) |
// |  [11]      (5)     [12]     none      (0) |
// |  [12]      (6)     [13]     none      (0) |
// |  [13]      (7)     null     null      (6) |

enum miko_std_syntax_reader_flag_t {
	miko_std_syntax_reader_flag__allow_ignore  = 0x01,  // allow ignore this check
	miko_std_syntax_reader_flag__must_finish   = 0x02,  // this vlist.next must is tail
	miko_std_syntax_reader_flag__replace_scope = 0x10,  // if check fail, allow replace scope (this to tail)
};

struct miko_std_syntax_reader_stack_t {
	miko_index_t group_id;  // group-id
	vattr_vlist_t *vlist;   // list-tail
};

struct miko_std_syntax_reader_inode_t {
	miko_index_t group_id; // => this-list group-id
	miko_index_t okaynext; // => inode
	miko_index_t rollback; // => inode
	miko_index_t back_gid; // => roll-back group-id
};

struct miko_std_syntax_reader_data_t {
	miko_index_t type;
	refer_nstring_t syntax;
	miko_offset_t slot;
	uintptr_t flags;
};

struct miko_std_syntax_reader_t {
	miko_std_syntax_reader_inode_t inode;
	miko_std_syntax_reader_data_t data;
};

struct miko_std_syntax_reader_s {
	exbuffer_t reader_cache;
	const miko_std_syntaxor_s *syntaxor;
	miko_std_syntax_reader_stack_t *stack_array;
	miko_std_syntax_reader_t *reader_array;
	miko_count_t reader_count;
	miko_index_t group_curr_id;
	miko_count_t syntax_slot;
};

// func

static void miko_std_syntax_reader_free_func(miko_std_syntax_reader_s *restrict r)
{
	miko_std_syntax_reader_t *restrict p;
	uintptr_t i, n;
	p = r->reader_array;
	n = r->reader_count;
	for (i = 0; i < n; ++i)
	{
		refer_ck_free(p[i].data.syntax);
	}
	refer_ck_free(r->syntaxor);
	exbuffer_uini(&r->reader_cache);
}

miko_std_syntax_reader_s* miko_std_syntax_reader_alloc(const miko_std_syntaxor_s *restrict syntaxor, miko_count_t syntax_slot)
{
	miko_std_syntax_reader_s *restrict r;
	miko_std_syntax_reader_t reader;
	if (syntaxor && (r = (miko_std_syntax_reader_s *) refer_alloz(sizeof(miko_std_syntax_reader_s))))
	{
		refer_hook_free(r, syntax_reader);
		r->syntaxor = (const miko_std_syntaxor_s *) refer_save(syntaxor);
		r->syntax_slot = syntax_slot;
		if (exbuffer_init(&r->reader_cache, 0))
		{
			reader.inode.group_id = 0;
			reader.inode.okaynext = 0;
			reader.inode.rollback = 0;
			reader.inode.back_gid = 0;
			reader.data.type = 0;
			reader.data.syntax = NULL;
			reader.data.slot = ~(miko_offset_t) 0;
			reader.data.flags = 0;
			if ((r->reader_array = (miko_std_syntax_reader_t *) exbuffer_append(
				&r->reader_cache, &reader, sizeof(reader))))
			{
				r->reader_count += 1;
				return r;
			}
		}
		refer_free(r);
	}
	return NULL;
}

miko_index_t miko_std_syntax_reader_now_group(miko_std_syntax_reader_s *restrict r)
{
	return r->group_curr_id;
}

miko_index_t miko_std_syntax_reader_new_group(miko_std_syntax_reader_s *restrict r)
{
	if (!r->stack_array)
		return (r->group_curr_id += 1);
	return 0;
}

miko_index_t miko_std_syntax_reader_add_inode_by_nstring(miko_std_syntax_reader_s *restrict r, miko_index_t id_index, refer_nstring_t syntax, miko_offset_t slot)
{
	miko_std_syntax_reader_t *restrict p;
	miko_std_syntax_reader_t reader;
	miko_index_t group;
	miko_count_t count;
	if (!r->stack_array && (group = r->group_curr_id) && (!~slot || slot < r->syntax_slot))
	{
		reader.inode.group_id = group;
		reader.inode.okaynext = 0;
		reader.inode.rollback = ~(miko_index_t) 0;
		reader.inode.back_gid = 0;
		reader.data.type = id_index;
		reader.data.syntax = syntax;
		reader.data.slot = slot;
		reader.data.flags = 0;
		if ((p = (miko_std_syntax_reader_t *) exbuffer_append(&r->reader_cache, &reader, sizeof(reader))))
		{
			refer_save(reader.data.syntax);
			r->reader_array = p;
			p += (count = r->reader_count) - 1;
			r->reader_count = count + 1;
			if (count > 1 && p->inode.group_id == group)
				p->inode.okaynext = count;
			return count;
		}
	}
	return 0;
}

miko_index_t miko_std_syntax_reader_add_inode_by_cstring(miko_std_syntax_reader_s *restrict r, miko_index_t id_index, const char *restrict syntax, miko_offset_t slot)
{
	refer_nstring_t nstring;
	miko_index_t result;
	nstring = NULL;
	if (!syntax || (nstring = refer_dump_nstring(syntax)))
	{
		result = miko_std_syntax_reader_add_inode_by_nstring(r, id_index, nstring, slot);
		refer_ck_free(nstring);
		return result;
	}
	return 0;
}

miko_index_t miko_std_syntax_reader_enable_inode_allow_ignore(miko_std_syntax_reader_s *restrict r)
{
	miko_std_syntax_reader_t *restrict p;
	miko_index_t index;
	if (!r->stack_array && (index = (r->reader_count - 1)))
	{
		p = r->reader_array + index;
		p->data.flags |= miko_std_syntax_reader_flag__allow_ignore;
		return index;
	}
	return 0;
}

miko_index_t miko_std_syntax_reader_enable_inode_must_finish(miko_std_syntax_reader_s *restrict r)
{
	miko_std_syntax_reader_t *restrict p;
	miko_index_t index;
	if (!r->stack_array && (index = (r->reader_count - 1)))
	{
		p = r->reader_array + index;
		p->data.flags |= miko_std_syntax_reader_flag__must_finish;
		return index;
	}
	return 0;
}

miko_index_t miko_std_syntax_reader_enable_inode_replace_scope(miko_std_syntax_reader_s *restrict r)
{
	miko_std_syntax_reader_t *restrict p;
	miko_index_t index;
	if (!r->stack_array && (index = (r->reader_count - 1)))
	{
		p = r->reader_array + index;
		p->data.flags |= miko_std_syntax_reader_flag__replace_scope;
		return index;
	}
	return 0;
}

static miko_index_t miko_std_syntax_reader_group_find_first(const miko_std_syntax_reader_t *restrict p, miko_count_t n, miko_index_t group_id, miko_count_t *restrict rcount)
{
	miko_index_t pos, c;
	for (pos = 0; pos < n && p[pos].inode.group_id != group_id; ++pos) ;
	if (pos < n)
	{
		if (rcount)
		{
			for (c = pos; c < n && p[c].inode.group_id == group_id; ++c) ;
			*rcount = c - pos;
		}
		return pos;
	}
	if (rcount) *rcount = 0;
	return 0;
}

miko_index_t miko_std_syntax_reader_group_okaynext(miko_std_syntax_reader_s *restrict r, miko_index_t src_group_id, miko_index_t dst_group_id)
{
	miko_std_syntax_reader_t *restrict p;
	miko_count_t n, count;
	miko_index_t spos, dpos;
	if (!r->stack_array && src_group_id && src_group_id < dst_group_id && dst_group_id <= r->group_curr_id)
	{
		p = r->reader_array;
		n = r->reader_count;
		if ((spos = miko_std_syntax_reader_group_find_first(p, n, src_group_id, &count)) &&
			(dpos = miko_std_syntax_reader_group_find_first(p, n, dst_group_id, NULL)))
		{
			p[spos + count - 1].inode.okaynext = dpos;
			return src_group_id;
		}
	}
	return 0;
}

miko_index_t miko_std_syntax_reader_group_rollback(miko_std_syntax_reader_s *restrict r, miko_index_t src_group_id, miko_index_t target_group_id, miko_index_t depend_group_id)
{
	miko_std_syntax_reader_t *restrict p;
	miko_count_t n, count;
	miko_index_t spos, dpos;
	if (!r->stack_array && src_group_id && (!target_group_id || src_group_id < target_group_id) &&
		target_group_id <= r->group_curr_id && depend_group_id < src_group_id)
	{
		p = r->reader_array;
		n = r->reader_count;
		if ((spos = miko_std_syntax_reader_group_find_first(p, n, src_group_id, &count)) &&
			(dpos = miko_std_syntax_reader_group_find_first(p, n, target_group_id, NULL)))
		{
			p += spos;
			for (spos = 0; spos < count; ++spos)
			{
				p[spos].inode.rollback = dpos;
				p[spos].inode.back_gid = depend_group_id;
			}
			return src_group_id;
		}
	}
	return 0;
}

miko_std_syntax_reader_s* miko_std_syntax_reader_finish(miko_std_syntax_reader_s *restrict r)
{
	miko_std_syntax_reader_t *restrict p;
	miko_count_t group_count;
	if (!r->stack_array)
	{
		group_count = r->group_curr_id + 1;
		if ((p = (miko_std_syntax_reader_t *) exbuffer_need(&r->reader_cache, r->reader_cache.used +
			group_count * sizeof(miko_std_syntax_reader_stack_t))))
		{
			r->reader_array = p;
			r->stack_array = (miko_std_syntax_reader_stack_t *) (p + r->reader_count);
			r->group_curr_id = group_count;
			return r;
		}
	}
	return NULL;
}

#include <memory.h>

static inline miko_std_syntax_s* miko_std_syntax_reader_verify(miko_std_syntax_s *restrict syntax, const miko_std_syntax_reader_data_t *restrict data)
{
	if ((!data->type || syntax->id.index == data->type) &&
		(!data->syntax || (syntax->data.syntax && syntax->data.syntax->length == data->syntax->length &&
			!memcmp(syntax->data.syntax->string, data->syntax->string, data->syntax->length))))
		return syntax;
	return NULL;
}

static vattr_vlist_t* miko_std_syntax_reader_stack_back(miko_std_syntax_reader_stack_t *restrict stack, uintptr_t *restrict nstack, uintptr_t *restrict gid, miko_index_t back_gid)
{
	uintptr_t i;
	i = *nstack;
	while (i)
	{
		i -= 1;
		if (stack[i].group_id == back_gid)
		{
			*nstack = i;
			*gid = back_gid;
			return stack[i].vlist;
		}
	}
	return NULL;
}

static vattr_vlist_t* miko_std_syntax_reader_replace_scope(const miko_std_syntaxor_s *restrict syntaxor, const miko_std_syntax_reader_data_t *restrict data, vattr_vlist_t *restrict vlist_start, vattr_vlist_t *restrict vlist_stop, miko_std_syntax_reader_stack_t *restrict stack, uintptr_t nstack)
{
	vattr_s *restrict vattr;
	const miko_std_syntax_id_t *restrict id;
	vattr_vlist_t *restrict vlist_tail;
	miko_std_syntax_s *restrict syntax;
	miko_std_syntax_source_t source;
	uintptr_t i;
	vattr = vlist_start->vslot->vattr;
	vlist_tail = vlist_stop?vattr_last(vlist_stop):vattr_tail(vattr);
	if (vlist_tail != vattr_last_end(vattr) &&
		(id = miko_std_syntaxor_index2id(syntaxor, data->type)) &&
		miko_std_syntax_fetch_source_by_vlist(&source, vlist_start, vlist_tail) &&
		(syntax = miko_std_syntax_create_scope_by_vlist(id, &source, vlist_start, vlist_stop)))
	{
		syntax->data.syntax = (refer_nstring_t) refer_save(data->syntax);
		syntax->data.value = (refer_nstring_t) refer_save(data->syntax);
		if (miko_std_syntax_scope_replace_vlist2syntax(vlist_start, vlist_stop, syntax))
		{
			vlist_tail = vlist_stop?vattr_last(vlist_stop):vattr_tail(vattr);
			if (vlist_tail != vattr_last_end(vattr))
			{
				for (i = 0; i < nstack; ++i)
				{
					if (stack[i].vlist == vlist_start)
						stack[i].vlist = vlist_tail;
				}
				refer_free(syntax);
				return vlist_tail;
			}
		}
		refer_free(syntax);
	}
	return NULL;
}

miko_std_syntax_reader_s* miko_std_syntax_reader_fetch(miko_std_syntax_reader_s *restrict r, miko_std_syntax_s *syntax_slot[], miko_count_t slot_count, vattr_vlist_t *restrict vlist_start, vattr_vlist_t *restrict vlist_stop, vattr_vlist_t *restrict *restrict vlist_rpos)
{
	miko_std_syntax_reader_stack_t *restrict stack;
	miko_std_syntax_reader_t *restrict p;
	miko_std_syntax_s *restrict syntax;
	vattr_vlist_t *restrict vlist;
	uintptr_t i, n, nstack, gid, slot;
	memset(syntax_slot, 0, slot_count * sizeof(miko_std_syntax_s *));
	if ((stack = r->stack_array) && r->syntax_slot == slot_count)
	{
		p = r->reader_array;
		n = r->reader_count;
		vlist = vlist_start;
		nstack = gid = 0;
		i = 1;
		while (i && vlist && vlist != vlist_stop)
		{
			if (i >= n) goto label_fail;
			if (gid != p[i].inode.group_id)
			{
				stack[nstack].group_id = gid;
				stack[nstack].vlist = vlist;
				nstack += 1;
			}
			// verify
			label_verify:
			syntax = (miko_std_syntax_s *) vlist->value;
			if (miko_std_syntax_reader_verify(syntax, &p[i].data))
			{
				if (p[i].data.flags & miko_std_syntax_reader_flag__replace_scope)
				{
					if (vlist->vattr_next != vlist_stop)
						goto label_replace_scope;
				}
				if (p[i].data.flags & miko_std_syntax_reader_flag__must_finish)
				{
					if (vlist->vattr_next != vlist_stop)
						goto label_rollback;
				}
				// verify okay
				if ((slot = p[i].data.slot) < slot_count)
					syntax_slot[slot] = syntax;
				i = p[i].inode.okaynext;
				vlist = vlist->vattr_next;
			}
			else if (p[i].data.flags & miko_std_syntax_reader_flag__allow_ignore)
				i = p[i].inode.okaynext;
			else if (p[i].data.flags & miko_std_syntax_reader_flag__replace_scope)
			{
				label_replace_scope:
				if (!(vlist = miko_std_syntax_reader_replace_scope(
					r->syntaxor, &p[i].data, vlist, vlist_stop, stack, nstack)))
					goto label_fail;
				goto label_verify;
			}
			else
			{
				label_rollback:
				if (!(vlist = miko_std_syntax_reader_stack_back(
					stack, &nstack, &gid, p[i].inode.back_gid)))
					goto label_fail;
				i = p[i].inode.rollback;
			}
		}
		if (!i)
		{
			if (vlist_rpos) *vlist_rpos = vlist;
			return r;
		}
	}
	label_fail:
	if (vlist_rpos) *vlist_rpos = vlist_start;
	return NULL;
}
