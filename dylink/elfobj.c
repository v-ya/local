#include "elfobj.h"
#include "dylink.h"
#include <hashmap.h>
#include <rbtree.h>
#include <inttypes.h>
#include <elf.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static uint8_t* elfobj_load_file(const char *path, size_t *psize)
{
	uint8_t *r;
	FILE *fp;
	size_t n;
	r = NULL;
	if (path)
	{
		fp = fopen(path, "rb");
		if (fp)
		{
			fseek(fp, 0, SEEK_END);
			n = ftell(fp);
			if (psize) *psize = n;
			if (n)
			{
				r = malloc(n + 1);
				if (r)
				{
					fseek(fp, 0, SEEK_SET);
					if (fread(r, 1, n, fp) == n) r[n] = 0;
					else
					{
						free(r);
						r = NULL;
					}
				}
			}
			fclose(fp);
		}
	}
	return r;
}

inline static char* elfobj_get_name(char *namelist, size_t nlsize, size_t ndx)
{
	if (ndx < nlsize) return namelist + ndx;
	return NULL;
}

typedef enum elfobj_sym_bind_t {
	elfobj_sym_local,
	elfobj_sym_global,
	elfobj_sym_undef
} elfobj_sym_bind_t;

typedef struct elfobj_sym_t {
	uint64_t index;
	uint64_t offset;
	uint64_t size;
	elfobj_sym_bind_t bind;
	uint32_t session_index;
	uint64_t g_pos;
} elfobj_sym_t;

typedef struct elfobj_rela_list_t {
	struct elfobj_rela_list_t *next;
	elfobj_sym_t *sym;
	uint64_t offset;
	int64_t addend;
	uint32_t name_offset;
	uint32_t type;
} elfobj_rela_list_t;

typedef struct elfobj_rela_session_list_t {
	struct elfobj_rela_session_list_t *next;
	Elf64_Rela *list;
	uint32_t number;
	uint32_t shndx;
} elfobj_rela_session_list_t;

static void elfobj_set_sym_free_func(hashmap_vlist_t *vl)
{
	if (vl->value) free(vl->value);
}

static int elfobj_set_sym(hashmap_t *s, char *name, uint64_t index, uint64_t offset, uint64_t size, elfobj_sym_bind_t bind, uint32_t sdx)
{
	elfobj_sym_t *v;
	if (hashmap_get_head(s, index)) ;
	else
	{
		v = (elfobj_sym_t *) malloc(sizeof(elfobj_sym_t));
		if (!v) goto Err;
		v->index = index;
		v->offset = offset;
		v->size = size;
		v->bind = bind;
		v->session_index = sdx;
		v->g_pos = 0;
		if (!hashmap_set_head(s, index, v, elfobj_set_sym_free_func))
		{
			free(v);
			goto Err;
		}
		if (bind == elfobj_sym_global || bind == elfobj_sym_undef)
		{
			if (!name || !*name) goto Err;
			if (!hashmap_set_name(s, name, v, NULL)) goto Err;
		}
	}
	return 0;
	Err:
	return -1;
}

elfobj_rela_session_list_t* elfobj_rela_session_list_insert(elfobj_rela_session_list_t **p, Elf64_Rela *list, uint32_t number, uint32_t shndx)
{
	elfobj_rela_session_list_t *v;
	v = (elfobj_rela_session_list_t *) malloc(sizeof(elfobj_rela_session_list_t));
	if (v)
	{
		v->list = list;
		v->number = number;
		v->shndx = shndx;
		v->next = *p;
		*p = v;
	}
	return v;
}

void elfobj_rela_session_list_clear(elfobj_rela_session_list_t **p)
{
	elfobj_rela_session_list_t *v;
	while ((v = *p))
	{
		*p = v->next;
		free(v);
	}
}

struct elf64obj_t {
	uint8_t *objdata;
	size_t objsize;
	Elf64_Ehdr *header;
	Elf64_Shdr *session_header;
	size_t session_number;
	hashmap_t session;
	hashmap_t need_session;
	hashmap_t symtab;
	char *session_namelist;
	size_t session_nlsize;
	Elf64_Sym *symtab_list;
	size_t symtab_number;
	char *symtab_namelist;
	size_t symtab_nlsize;
	elfobj_rela_session_list_t *rslist;
	// Elf64_Rela *rela_text_list;
	// size_t rela_text_number;
	// Elf64_Rela *rela_data_list;
	// size_t rela_data_number;
	// uint32_t rela_text_shndx;
	// uint32_t rela_data_shndx;
	size_t common_size;
	// build set
	hashmap_t *strpool_offset;
	uint8_t *strpool_data;
	size_t strpool_size;
	hashmap_t *session_offset;
	uint8_t *image_data;
	size_t image_size;
	size_t image_takeup;
	hashmap_t *import;
	size_t isym_number;
	size_t esym_number;
};

static int elf64obj_set_sym(elf64obj_t *e, uint32_t index)
{
	Elf64_Sym *v;
	Elf64_Shdr *s;
	char *name;
	uint32_t t;
	elfobj_sym_bind_t b;
	if (!index || index >= e->symtab_number) goto Err;
	v = e->symtab_list + index;
	switch (ELF64_ST_BIND(v->st_info))
	{
		case STB_LOCAL:
			b = elfobj_sym_local;
			break;
		case STB_GLOBAL:
			b = elfobj_sym_global;
			break;
		default:
			goto Err;
	}
	t = ELF64_ST_TYPE(v->st_info);
	switch (v->st_shndx)
	{
		case SHN_UNDEF:
			b = elfobj_sym_undef;
			name = elfobj_get_name(e->symtab_namelist, e->symtab_nlsize, v->st_name);
			if (elfobj_set_sym(&e->symtab, name, index, 0, 0, b, 0)) goto Err;
			break;
		case SHN_ABS:
			if (t != STT_FILE) goto Err;
			break;
		case SHN_COMMON:
			if (v->st_size && !hashmap_get_head(&e->symtab, index))
			{
				name = elfobj_get_name(e->symtab_namelist, e->symtab_nlsize, v->st_name);
				if (elfobj_set_sym(&e->symtab, name, index, e->common_size, v->st_size, b, 0)) goto Err;
				e->common_size += v->st_size;
			}
			break;
		default:
			if (v->st_shndx >= e->session_number) goto Err;
			s = e->session_header + v->st_shndx;
			name = elfobj_get_name(e->session_namelist, e->session_nlsize, s->sh_name);
			if (!name) goto Err;
			if (!hashmap_put_name(&e->need_session, name, s, NULL)) goto Err;
			name = elfobj_get_name(e->symtab_namelist, e->symtab_nlsize, v->st_name);
			if (elfobj_set_sym(&e->symtab, name, index, v->st_value, v->st_size, b, v->st_shndx)) goto Err;
			break;
	}
	return 0;
	Err:
	return -1;
}

static int elf64obj_set_need_session(elf64obj_t *e)
{
	Elf64_Rela *r;
	Elf64_Sym *v;
	elfobj_rela_session_list_t *rslist;
	size_t n;
	uint32_t i;
	v = e->symtab_list;
	n = e->symtab_number;
	i = 0;
	while (n)
	{
		if (ELF64_ST_BIND(v->st_info) == elfobj_sym_global && v->st_shndx)
		{
			if (v->st_shndx < e->session_number || v->st_shndx == SHN_COMMON)
			{
				if (elf64obj_set_sym(e, i)) goto Err;
			}
			else goto Err;
		}
		++i;
		++v;
		--n;
	}
	rslist = e->rslist;
	while (rslist)
	{
		r = rslist->list;
		n = rslist->number;
		while (n)
		{
			if (elf64obj_set_sym(e, ELF64_R_SYM(r->r_info))) goto Err;
			++r;
			--n;
		}
		rslist = rslist->next;
	}
	return 0;
	Err:
	return -1;
}

elf64obj_t* elf64obj_load(const char *elf64obj_path)
{
	elf64obj_t *e;
	Elf64_Shdr *s;
	char *name;
	size_t p, n;
	e = (elf64obj_t *) calloc(1, sizeof(elf64obj_t));
	if (e)
	{
		if (!hashmap_init(&e->session) || !hashmap_init(&e->need_session) || !hashmap_init(&e->symtab)) goto Err;
		if (!(e->objdata = elfobj_load_file(elf64obj_path, &e->objsize))) goto Err;
		if (e->objsize < sizeof(Elf64_Ehdr)) goto Err;
		e->header = (Elf64_Ehdr *) e->objdata;
		// ELF
		if (*(uint32_t *) e->objdata != 0x464c457f) goto Err;
		// 64 bits
		if (e->header->e_ident[4] != 0x02) goto Err;
		// little endian
		if (e->header->e_ident[5] != 0x01) goto Err;
		// version
		if (e->header->e_ident[6] != 0x01) goto Err;
		if (e->header->e_shentsize != sizeof(Elf64_Shdr)) goto Err;
		p = e->header->e_shoff;
		n = e->header->e_shnum;
		if (p + n * sizeof(Elf64_Shdr) > e->objsize) goto Err;
		e->session_header = (Elf64_Shdr *) (e->objdata + p);
		e->session_number = n;
		n = e->header->e_shstrndx;
		if (n >= e->session_number) goto Err;
		p = e->session_header[n].sh_offset;
		n = e->session_header[n].sh_size;
		if (p + n > e->objsize) goto Err;
		e->session_namelist = (char *) (e->objdata + p);
		e->session_nlsize = n;
		for (p = 0, n = e->session_number; p < n; ++p)
		{
			s = e->session_header + p;
			if (s->sh_type != SHT_NOBITS && (size_t) s->sh_offset + (size_t) s->sh_size > e->objsize) goto Err;
			name = elfobj_get_name(e->session_namelist, e->session_nlsize, s->sh_name);
			if (name)
			{
				if (!hashmap_set_name(&e->session, name, e->session_header + p, NULL))
					goto Err;
			}
		}
		// .symtab
		s = hashmap_get_name(&e->session, ".symtab");
		if (s)
		{
			p = s->sh_offset;
			n = s->sh_size;
			if (p + n > e->objsize) goto Err;
			if (n % sizeof(Elf64_Sym)) goto Err;
			e->symtab_list = (Elf64_Sym *) (e->objdata + p);
			e->symtab_number = n / sizeof(Elf64_Sym);
			p = s->sh_link;
			if (!p || p >= e->session_number) goto Err;
			s = e->session_header + p;
			p = s->sh_offset;
			n = s->sh_size;
			if (p + n > e->objsize) goto Err;
			e->symtab_namelist = (char *) (e->objdata + p);
			e->symtab_nlsize = n;
		}
		// .rela.*
		for (p = 0, n = e->session_number; p < n; ++p)
		{
			s = e->session_header + p;
			if (s->sh_type == SHT_RELA &&
				(name = elfobj_get_name(e->session_namelist, e->session_nlsize, s->sh_name)) &&
				strcmp(name, ".rela.eh_frame"))
			{
				if (s->sh_offset + s->sh_size > e->objsize) goto Err;
				if (s->sh_size % sizeof(Elf64_Rela)) goto Err;
				if (!s->sh_info || s->sh_info >= e->session_number) goto Err;
				if (!elfobj_rela_session_list_insert(
					&e->rslist,
					(Elf64_Rela *) (e->objdata + s->sh_offset),
					s->sh_size / sizeof(Elf64_Rela),
					s->sh_info
				)) goto Err;
			}
		}
		if (elf64obj_set_need_session(e)) goto Err;
		return e;
		Err:
		elf64obj_free(e);
	}
	return NULL;
}

void elf64obj_free(elf64obj_t *e)
{
	if (e)
	{
		// build set
		if (e->strpool_offset) hashmap_free(e->strpool_offset);
		if (e->strpool_data) free(e->strpool_data);
		if (e->session_offset) hashmap_free(e->session_offset);
		if (e->image_data) free(e->image_data);
		if (e->import) hashmap_free(e->import);
		// init set
		hashmap_uini(&e->session);
		hashmap_uini(&e->need_session);
		hashmap_uini(&e->symtab);
		elfobj_rela_session_list_clear(&e->rslist);
		if (e->objdata) free(e->objdata);
		free(e);
	}
}

void elf64obj_delete_sym(elf64obj_t *e, const char *name)
{
	elfobj_sym_t *s;
	uint64_t index;
	if (e && name)
	{
		s = (elfobj_sym_t *) hashmap_find_name(&e->symtab, name);
		if (s)
		{
			index = s->index;
			hashmap_delete_name(&e->symtab, name);
			hashmap_delete_head(&e->symtab, index);
		}
	}
}

// elf64obj_build
static void elf64obj_build_namelist_func(hashmap_vlist_t *vl, rbtree_t ***namelist)
{
	elfobj_sym_t *s;
	uint64_t len;
	if (*namelist && vl->name && (s = (elfobj_sym_t *) vl->value))
	{
		if (s->bind == elfobj_sym_global || s->bind ==elfobj_sym_undef)
		{
			len = strlen(vl->name);
			if (len)
			{
				if (!rbtree_insert(*namelist, NULL, (~(len << 32) & 0xffffffff00000000ul) | s->index, vl->name, NULL))
					*namelist = NULL;
			}
		}
	}
}

static void elf64obj_build_strpool_offset_func(rbtree_t *v, elf64obj_t **e)
{
	char *name;
	uint32_t offset;
	uint32_t n;
	if (*e && (name = (char *) v->value))
	{
		if (hashmap_find_name((*e)->strpool_offset, name)) return ;
		offset = (*e)->strpool_size;
		n = ~(uint32_t)(v->key_index >> 32);
		(*e)->strpool_size += n + 1;
		while (n)
		{
			hashmap_put_name((*e)->strpool_offset, name, (void *)(uintptr_t) offset, NULL);
			--n;
			++name;
			++offset;
		}
	}
}

static void elf64obj_build_strpool_data_func(rbtree_t *v, elf64obj_t *e)
{
	size_t offset;
	uint32_t n;
	if ((n = ~(uint32_t)(v->key_index >> 32)))
	{
		offset = (size_t)(uintptr_t) hashmap_get_name(e->strpool_offset, (char *) v->value);
		memcpy(e->strpool_data + offset, v->value, n);
	}
}

uint8_t* elf64obj_build_strpool(elf64obj_t *e, size_t *psize)
{
	rbtree_t *namelist;
	void *argv;
	namelist = NULL;
	if (e->strpool_offset) hashmap_clear(e->strpool_offset);
	else e->strpool_offset = hashmap_alloc();
	if (e->strpool_data)
	{
		free(e->strpool_data);
		e->strpool_data = NULL;
	}
	e->strpool_size = 0;
	if (!e->strpool_offset) goto Err;
	argv = &namelist;
	hashmap_call(&e->symtab, (hashmap_func_call_f) elf64obj_build_namelist_func, &argv);
	if (!argv) goto Err;
	argv = e;
	rbtree_call(&namelist, (rbtree_func_call_t) elf64obj_build_strpool_offset_func, &argv);
	if (!argv) goto Err;
	if (e->strpool_size)
	{
		e->strpool_data = (uint8_t *) calloc(1, e->strpool_size);
		if (!e->strpool_data) goto Err;
		rbtree_call(&namelist, (rbtree_func_call_t) elf64obj_build_strpool_data_func, e);
	}
	End:
	if (namelist) rbtree_clear(&namelist);
	if (psize) *psize = e->strpool_size;
	return e->strpool_data;
	Err:
	e->strpool_size = 0;
	goto End;
}

// elf64obj_build_image
static void elf64obj_build_session_offset_progbits_func(hashmap_vlist_t *vl, elf64obj_t **e)
{
	Elf64_Shdr *s;
	const char *name;
	size_t offset;
	if (*e && vl->name && (s = (Elf64_Shdr *) vl->value))
	{
		if (s->sh_type == SHT_PROGBITS)
		{
			name = vl->name;
			offset = (*e)->image_size;
			if (hashmap_set_name((*e)->session_offset, name, (void *)(uintptr_t) offset, NULL))
			{
				(*e)->image_size = (offset + s->sh_size + 0x0f) & ~0x0ful;
				return ;
			}
			*e = NULL;
		}
	}
}

static void elf64obj_build_image_takeup_common_func(hashmap_vlist_t *vl, elf64obj_t *e)
{
	elfobj_sym_t *s;
	if (vl->name && (s = (elfobj_sym_t *) vl->value) && s->size)
	{
		if ((s->bind == elfobj_sym_local || s->bind == elfobj_sym_global) && !s->session_index)
		{
			s->offset += e->image_size;
		}
	}
}

static void elf64obj_build_session_offset_nobits_func(hashmap_vlist_t *vl, elf64obj_t **e)
{
	Elf64_Shdr *s;
	const char *name;
	size_t offset;
	if (*e && vl->name && (s = (Elf64_Shdr *) vl->value))
	{
		if (s->sh_type == SHT_NOBITS)
		{
			name = vl->name;
			offset = (*e)->image_takeup;
			if (hashmap_set_name((*e)->session_offset, name, (void *)(uintptr_t) offset, NULL))
			{
				(*e)->image_takeup = (offset + s->sh_size + 0x0f) & ~0x0ful;
				return ;
			}
			*e = NULL;
		}
	}
}

static void elf64obj_build_image_data_func(hashmap_vlist_t *vl, elf64obj_t *e)
{
	Elf64_Shdr *s;
	size_t offset;
	if (vl->name)
	{
		s = (Elf64_Shdr *) hashmap_get_name(&e->session, vl->name);
		if (s->sh_type == SHT_PROGBITS)
		{
			offset = (size_t)(uintptr_t) vl->value;
			memcpy(e->image_data + offset, e->objdata + s->sh_offset, s->sh_size);
		}
	}
}

uint8_t* elf64obj_build_image(elf64obj_t *e, size_t *psize, size_t *ptakeup)
{
	void *argv;
	if (e->session_offset) hashmap_clear(e->session_offset);
	else e->session_offset = hashmap_alloc();
	if (e->image_data)
	{
		free(e->image_data);
		e->image_data = NULL;
	}
	e->image_size = e->image_takeup = 0;
	if (!e->session_offset) goto Err;
	argv = e;
	hashmap_call(&e->need_session, (hashmap_func_call_f) elf64obj_build_session_offset_progbits_func, &argv);
	if (!argv) goto Err;
	hashmap_call(&e->symtab, (hashmap_func_call_f) elf64obj_build_image_takeup_common_func, e);
	e->image_takeup = (e->image_size + e->common_size + 0x0f) & ~0x0ful;
	hashmap_call(&e->need_session, (hashmap_func_call_f) elf64obj_build_session_offset_nobits_func, &argv);
	if (!argv) goto Err;
	if (e->image_size)
	{
		e->image_data = (uint8_t *) malloc(e->image_size);
		if (!e->image_data) goto Err;
		memset(e->image_data, 0xcccccccc, e->image_size);
		hashmap_call(e->session_offset, (hashmap_func_call_f) elf64obj_build_image_data_func, e);
	}
	End:
	if (psize) *psize = e->image_size;
	if (ptakeup) *ptakeup = e->image_takeup;
	return e->image_data;
	Err:
	e->image_size = e->image_takeup = 0;
	goto End;
}

// elf64obj_build_import
static void elf64obj_import_free_func(hashmap_vlist_t *vl)
{
	elfobj_rela_list_t *rl, *p;
	if ((rl = (elfobj_rela_list_t *) vl->value))
	{
		while ((p = rl))
		{
			rl = p->next;
			free(p);
		}
	}
}

static int elf64obj_build_import_by_rela(elf64obj_t *e, Elf64_Rela *rela, size_t rela_offset)
{
	elfobj_sym_t *sym;
	elfobj_rela_list_t *rl;
	hashmap_vlist_t *vl;
	char *name;
	sym = (elfobj_sym_t *) hashmap_get_head(&e->symtab, ELF64_R_SYM(rela->r_info));
	if (!sym) goto Err;
	name = elfobj_get_name(e->symtab_namelist, e->symtab_nlsize, e->symtab_list[sym->index].st_name);
	rl = (elfobj_rela_list_t *) malloc(sizeof(elfobj_rela_list_t));
	if (!rl) goto Err;
	rl->next = NULL;
	rl->sym = sym;
	rl->offset = rela_offset + rela->r_offset;
	rl->addend = rela->r_addend;
	if (name) rl->name_offset = (uint32_t)(uintptr_t) hashmap_get_name(e->strpool_offset, name);
	rl->type = ELF64_R_TYPE(rela->r_info);
	if (sym->bind == elfobj_sym_undef) ++e->isym_number;
	vl = hashmap_find_head(e->import, sym->index);
	if (vl)
	{
		rl->next = (elfobj_rela_list_t *) vl->value;
		vl->value = rl;
	}
	else if (!hashmap_set_head(e->import, sym->index, rl, elf64obj_import_free_func))
	{
		free(rl);
		goto Err;
	}
	return 0;
	Err:
	return -1;
}

int elf64obj_build_import(elf64obj_t *e)
{
	elfobj_rela_session_list_t *rslist;
	Elf64_Rela *rela;
	size_t n;
	char *name;
	if (e->import) hashmap_clear(e->import);
	else e->import = hashmap_alloc();
	e->isym_number = 0;
	if (!e->import) goto Err;
	rslist = e->rslist;
	while (rslist)
	{
		rela = rslist->list;
		n = rslist->number;
		if (n)
		{
			name = elfobj_get_name(e->session_namelist, e->session_nlsize, e->session_header[rslist->shndx].sh_name);
			if (!name) goto Err;
			while (n)
			{
				if (elf64obj_build_import_by_rela(e, rela, (size_t)(uintptr_t) hashmap_get_name(e->session_offset, name)))
					goto Err;
				++rela;
				--n;
			}
		}
		rslist = rslist->next;
	}
	return 0;
	Err:
	return -1;
}

// elf64obj_build_export
static void elf64obj_build_export_func(hashmap_vlist_t *vl, size_t *n)
{
	elfobj_sym_t *s;
	if (vl->name && (s = (elfobj_sym_t *) vl->value))
	{
		if (s->bind == elfobj_sym_global) ++*n;
	}
}

void elf64obj_build_export(elf64obj_t *e)
{
	e->esym_number = 0;
	hashmap_call(&e->symtab, (hashmap_func_call_f) elf64obj_build_export_func, &e->esym_number);
}

// elf64obj_build_link_self
static int elf64obj_build_link_self_func(hashmap_vlist_t *vl, elf64obj_t **pe)
{
	elfobj_rela_list_t *rl;
	elf64obj_t *e;
	elfobj_sym_t *v;
	Elf64_Sym *s;
	char *name;
	size_t offset;
	rl = (elfobj_rela_list_t *) vl->value;
	if ((e = *pe) && rl->sym->bind != elfobj_sym_undef)
	{
		s = e->symtab_list + vl->head;
		if (s->st_shndx == SHN_COMMON)
		{
			v = hashmap_get_head(&e->symtab, vl->head);
			if (!v) goto Err;
			offset = v->offset;
		}
		else
		{
			name = elfobj_get_name(e->session_namelist, e->session_nlsize, e->session_header[s->st_shndx].sh_name);
			if (!name) goto Err;
			offset = (size_t)(uintptr_t) hashmap_get_name(e->session_offset, name) + s->st_value;
		}
		if (offset >= e->image_takeup) goto Err;
		while (rl)
		{
			if (rl->offset >= e->image_size) goto Err;
			if (m_x86_64_dylink_set(rl->type, e->image_data + rl->offset, rl->addend, e->image_data + offset, NULL))
				goto Err;
			rl = rl->next;
		}
		return 1;
	}
	return 0;
	Err:
	*pe = NULL;
	return 0;
}

int elf64obj_build_link_self(elf64obj_t *e)
{
	void *argv;
	argv = e;
	hashmap_isfree(e->import, (hashmap_func_isfree_f) elf64obj_build_link_self_func, &argv);
	return argv?0:-1;
}

// elf64obj_build_dylink
static void elf64obj_build_dylink_set_strpool_offset_func(hashmap_vlist_t *vl, size_t offset)
{
	vl->value = (uint8_t *) vl->value + offset;
}

static void elf64obj_build_dylink_set_import_func(hashmap_vlist_t *vl, dylink_isym_t **pih)
{
	elfobj_rela_list_t *rl;
	dylink_isym_t *ih;
	if ((rl = (elfobj_rela_list_t *) vl->value))
	{
		ih = *pih;
		while (rl)
		{
			if (rl->name_offset)
			{
				ih = *pih;
				++*pih;
				ih->name_offset = rl->name_offset;
				ih->type = rl->type;
				ih->offset = rl->offset;
				ih->addend = rl->addend;
			}
			rl = rl->next;
		}
	}
}

static void elf64obj_build_dylink_set_export_func(hashmap_vlist_t *vl, void *argv[2])
{
	elfobj_sym_t *v;
	dylink_esym_t **peh, *eh;
	elf64obj_t *e;
	char *name;
	peh = (dylink_esym_t **) argv[0];
	e = (elf64obj_t *) argv[1];
	if (e && vl->name && (v = (elfobj_sym_t *) vl->value) && v->bind == elfobj_sym_global)
	{
		eh = *peh;
		++*peh;
		eh->name_offset = (uint32_t)(uintptr_t) hashmap_get_name(e->strpool_offset, vl->name);
		name = elfobj_get_name(e->session_namelist, e->session_nlsize, e->session_header[v->session_index].sh_name);
		if (!name) goto Err;
		eh->offset = v->offset + (size_t)(uintptr_t) hashmap_get_name(e->session_offset, name);
	}
	return ;
	Err:
	argv[1] = NULL;
}

uint8_t* elf64obj_build_dylink(elf64obj_t *e, size_t *psize)
{
	uint8_t *r;
	dylink_header_t *h;
	void *ph;
	size_t size, strpool_offset, image_offset, isym_offset, esym_offset;
	void *argv[2];
	r = NULL;
	if (!elf64obj_build_strpool(e, NULL)) goto Err;
	hashmap_call(e->strpool_offset, (hashmap_func_call_f)(void *) elf64obj_build_dylink_set_strpool_offset_func, (void *)(uintptr_t) sizeof(dylink_header_t));
	if (!elf64obj_build_image(e, NULL, NULL)) goto Err;
	if (elf64obj_build_import(e)) goto Err;
	elf64obj_build_export(e);
	if (elf64obj_build_link_self(e)) goto Err;
	size = sizeof(dylink_header_t);
	strpool_offset = size;
	size = (size + e->strpool_size + 0x0f) & ~0x0ful;
	image_offset = size;
	size += e->image_size;
	isym_offset = size;
	size += e->isym_number * sizeof(dylink_isym_t);
	esym_offset = size;
	size += e->esym_number * sizeof(dylink_esym_t);
	r = (uint8_t *) calloc(1, size);
	if (!r) goto Err;
	h = (dylink_header_t *) r;
	memcpy(h->machine, dylink_mechine_x86_64, sizeof(dylink_mechine_x86_64) - 1);
	h->version = 1;
	h->header_size = sizeof(dylink_header_t);
	h->img_takeup = e->image_takeup;
	h->img_offset = (uint32_t) image_offset;
	h->img_size = (uint32_t) e->image_size;
	h->strpool_offset = (uint32_t) strpool_offset;
	h->strpool_size = (uint32_t) e->strpool_size;
	h->isym_offset = isym_offset;
	h->isym_number = e->isym_number;
	h->esym_offset = esym_offset;
	h->esym_number = e->esym_number;
	memcpy(r + strpool_offset, e->strpool_data, e->strpool_size);
	memcpy(r + image_offset, e->image_data, e->image_size);
	ph = r + isym_offset;
	hashmap_call(e->import, (hashmap_func_call_f) elf64obj_build_dylink_set_import_func, &ph);
	ph = r + esym_offset;
	argv[0] = &ph;
	argv[1] = e;
	hashmap_call(&e->symtab, (hashmap_func_call_f) elf64obj_build_dylink_set_export_func, argv);
	if (!argv[1]) goto Err;
	if (psize) *psize = size;
	return r;
	Err:
	if (r) free(r);
	if (psize) *psize = 0;
	return NULL;
}

static const char* elfobj_str_session_type(unsigned int t)
{
	switch (t)
	{
		case SHT_NULL:
			return "null";
		case SHT_PROGBITS:
			return "progbits";
		case SHT_SYMTAB:
			return "systab";
		case SHT_STRTAB:
			return "strtab";
		case SHT_RELA:
			return "rela";
		case SHT_HASH:
			return "hash";
		case SHT_DYNAMIC:
			return "dynamic";
		case SHT_NOTE:
			return "note";
		case SHT_NOBITS:
			return "nobits";
		case SHT_REL:
			return "rel";
		case SHT_SHLIB:
			return "shlib";
		case SHT_DYNSYM:
			return "dynsym";
		case SHT_INIT_ARRAY:
			return "init_array";
		case SHT_FINI_ARRAY:
			return "fini_array";
		case SHT_PREINIT_ARRAY:
			return "preinit_array";
		case SHT_GROUP:
			return "group";
		case SHT_SYMTAB_SHNDX:
			return "symtab_shndx";
		case SHT_NUM:
			return "num";
		case SHT_LOOS:
			return "low-os";
		case SHT_GNU_ATTRIBUTES:
			return "gnu_attr";
		case SHT_GNU_HASH:
			return "gnu_hash";
		case SHT_GNU_LIBLIST:
			return "gnu_liblist";
		case SHT_CHECKSUM:
			return "checksum";
		case SHT_SUNW_move:
			return "sunw_move";
		case SHT_SUNW_COMDAT:
			return "sunw_comdat";
		case SHT_SUNW_syminfo:
			return "sunw_syminfo";
		case SHT_GNU_verdef:
			return "gnu_verdef";
		case SHT_GNU_verneed:
			return "gnu_verneed";
		case SHT_HIOS:
			return "high-os";
		case SHT_LOPROC:
			return "low-proc";
		case SHT_HIPROC:
			return "high-proc";
		case SHT_LOUSER:
			return "low-user";
		case SHT_HIUSER:
			return "high-user";
		default:
			return "unknow";
	};
}

void elf64obj_dump_session(elf64obj_t *e)
{
	Elf64_Shdr *s;
	char *name;
	uint32_t i, n;
	if (e)
	{
		s = e->session_header;
		for (i = 0, n = (uint32_t) e->session_number; i < n; ++i)
		{
			name = elfobj_get_name(e->session_namelist, e->session_nlsize, s[i].sh_name);
			printf(
				"session[%2u] type: %-12s, offset: %8" PRIu64 ", size: %8" PRIu64 ", %c(%s)\n",
				i,
				elfobj_str_session_type(s[i].sh_type),
				s[i].sh_offset,
				s[i].sh_size,
				(name && hashmap_get_name(&e->need_session, name))?'*':' ',
				name
			);
		}
	}
}

static const char* elfobj_str_symtab_bind(unsigned char b)
{
	switch (b)
	{
		case STB_LOCAL:
			return "local";
		case STB_GLOBAL:
			return "global";
		case STB_WEAK:
			return "weak";
		case STB_NUM:
			return "number";
		case STB_LOOS:
			return "low-os";
		case STB_HIOS:
			return "high-os";
		case STB_LOPROC:
			return "low-proc";
		case STB_HIPROC:
			return "high-proc";
		default:
			return "unknow";
	};
}

static const char* elfobj_str_symtab_type(unsigned char t)
{
	switch (t)
	{
		case STT_NOTYPE:
			return "notype";
		case STT_OBJECT:
			return "object";
		case STT_FUNC:
			return "func";
		case STT_SECTION:
			return "session";
		case STT_FILE:
			return "file";
		case STT_COMMON:
			return "common";
		case STT_TLS:
			return "tls";
		case STT_NUM:
			return "number";
		case STT_LOOS:
			return "low-os";
		case STT_HIOS:
			return "high-os";
		case STT_LOPROC:
			return "low-proc";
		case STT_HIPROC:
			return "high-proc";
		default:
			return "unknow";
	};
}

void elf64obj_dump_symtab(elf64obj_t* e)
{
	Elf64_Sym *s;
	uint32_t i, n;
	if (e)
	{
		for (i = 0, n = (uint32_t) e->symtab_number; i < n; ++i)
		{
			s = e->symtab_list + i;
			printf(
				"sym[%3u] at: %6" PRIu64 ", size: %6" PRIu64 ", b: %10s, t: %10s %c%-24s: [%04x] %s\n",
				i,
				s->st_value,
				s->st_size,
				elfobj_str_symtab_bind(ELF64_ST_BIND(s->st_info)),
				elfobj_str_symtab_type(ELF64_ST_TYPE(s->st_info)),
				hashmap_get_head(&e->symtab, i)?'*':' ',
				elfobj_get_name(e->symtab_namelist, e->symtab_nlsize, s->st_name),
				s->st_shndx,
				s->st_shndx < e->session_number?
					elfobj_get_name(e->session_namelist, e->session_nlsize, e->session_header[s->st_shndx].sh_name):
					""
			);
		}
	}
}

void elf64obj_dump_rela(elf64obj_t* e)
{
	elfobj_rela_session_list_t *rslist;
	Elf64_Rela *r;
	Elf64_Sym *s;
	char *rela_sname, *session;
	uint32_t i, n, index;
	if (e)
	{
		rslist = e->rslist;
		while (rslist)
		{
			rela_sname = elfobj_get_name(e->session_namelist, e->session_nlsize, e->session_header[rslist->shndx].sh_name);
			for (i = 0, n = (uint32_t) rslist->number; i < n; ++i)
			{
				r = rslist->list + i;
				index = ELF64_R_SYM(r->r_info);
				s = NULL;
				session = NULL;
				if (index < e->symtab_number)
				{
					s = e->symtab_list + index;
					if (s->st_shndx < e->session_number)
						session = elfobj_get_name(e->session_namelist, e->session_nlsize, e->session_header[s->st_shndx].sh_name);
				}
				printf(
					"rela%s[%3u] pos: %6" PRIu64 ", type: %2u, addend: %6" PRId64 " [%3u] %-24s@[%04x]%s\n",
					rela_sname,
					i,
					r->r_offset,
					(uint32_t) ELF64_R_TYPE(r->r_info),
					r->r_addend,
					index,
					s?elfobj_get_name(e->symtab_namelist, e->symtab_nlsize, s->st_name):"(unknow)",
					s?s->st_shndx:0,
					session?session:""
				);
			}
			rslist = rslist->next;
		}
	}
}

void dylink_dump(uint8_t *r, size_t size)
{
	dylink_header_t *h;
	dylink_isym_t *ih;
	dylink_esym_t *eh;
	uint32_t i, n;
	if (size >= sizeof(dylink_header_t))
	{
		h = (dylink_header_t *) r;
		printf(
			"         machine: %s\n"
			"         version: %u\n"
			"     header_size: %u\n"
			"      img_takeup: %lu\n"
			"      img_offset: %u\n"
			"        img_size: %u\n"
			"  strpool_offset: %u\n"
			"    strpool_size: %u\n"
			"     isym_offset: %u\n"
			"     isym_number: %u\n"
			"     esym_offset: %u\n"
			"     esym_number: %u\n",
			h->machine,
			h->version,
			h->header_size,
			h->img_takeup,
			h->img_offset,
			h->img_size,
			h->strpool_offset,
			h->strpool_size,
			h->isym_offset,
			h->isym_number,
			h->esym_offset,
			h->esym_number
		);
		if (h->img_offset + h->img_size > size) printf("waring: h->img_offset + h->img_size > size\n");
		if (h->strpool_offset + h->strpool_size > size) printf("waring: h->strpool_offset + h->strpool_size > size\n");
		if (h->isym_offset + h->isym_number * sizeof(dylink_isym_t) > size) printf("waring: h->isym_offset + h->isym_number * sizeof(dylink_isym_t) > size\n");
		else
		{
			ih = (dylink_isym_t *) (r + h->isym_offset);
			n = h->isym_number;
			printf("isym[%u]\n", n);
			for (i = 0; i < n; ++i)
			{
				printf(
					"\t[%u]: type: %2u, offset: %8ld, addend: %6ld, (%4u): %s\n",
					i,
					ih->type,
					ih->offset,
					ih->addend,
					ih->name_offset,
					(ih->name_offset >= h->strpool_offset && ih->name_offset < h->strpool_offset + h->strpool_size)?
						(char *) (r + ih->name_offset):
						"???"
				);
				++ih;
			}
		}
		if (h->esym_offset + h->esym_number * sizeof(dylink_esym_t) > size) printf("waring: h->esym_offset + h->esym_number * sizeof(dylink_esym_t) > size\n");
		else
		{
			eh = (dylink_esym_t *) (r + h->esym_offset);
			n = h->esym_number;
			printf("esym[%u]\n", n);
			for (i = 0; i < n; ++i)
			{
				printf(
					"\t[%u]: offset: %8d, (%4u): %s\n",
					i,
					eh->offset,
					eh->name_offset,
					(eh->name_offset >= h->strpool_offset && eh->name_offset < h->strpool_offset + h->strpool_size)?
						(char *) (r + eh->name_offset):
						"???"
				);
				++eh;
			}
		}
	}
}

