#include "miko.form.h"
#include <memory.h>

static miko_wink_view_s* miko_form_impl_view_func(miko_wink_view_s *restrict view, miko_form_impl_w *restrict form)
{
	const miko_form_t *restrict p;
	uintptr_t i, n;
	p = form->form.form;
	n = form->form.count;
	for (i = 0; view && i < n; ++i)
	{
		if (p[i].vtype == miko_major_vtype__wink)
			view = miko_wink_view_add(view, p[i].var.wink);
	}
	return view;
}

static void miko_form_impl_free_func(miko_form_impl_w *restrict r)
{
	miko_form_pop(&r->form, r->form.count);
	exbuffer_uini(&r->buffer);
}

miko_form_w* miko_form_alloc(miko_wink_gomi_s *restrict gomi, miko_count_t max_count)
{
	miko_form_impl_w *restrict r;
	if ((r = ((miko_form_impl_w *) miko_wink_alloz(gomi, sizeof(miko_form_impl_w), NULL,
		(miko_wink_view_f) miko_form_impl_view_func,
		(miko_wink_free_f) miko_form_impl_free_func))))
	{
		if (exbuffer_init(&r->buffer, max_count * sizeof(miko_form_t)))
		{
			r->form.form = (miko_form_t *) r->buffer.data;
			r->limit = max_count;
			return &r->form;
		}
		miko_wink_unref(r);
	}
	return NULL;
}

#define _impl_(_form_)  ((miko_form_impl_w *) (_form_))

miko_form_w* miko_form_set_limit(miko_form_w *restrict r, miko_count_t max_count)
{
	miko_form_t *restrict p;
	uintptr_t size;
	if (max_count >= _impl_(r)->form.count)
	{
		if ((size = max_count * sizeof(miko_form_t)) <= _impl_(r)->buffer.size)
			goto label_okay;
		miko_wink_wlock_lock(r);
		if ((p = (miko_form_t *) exbuffer_need(&_impl_(r)->buffer, max_count * sizeof(miko_form_t))))
		{
			_impl_(r)->form.form = p;
			_impl_(r)->limit = max_count;
		}
		miko_wink_wlock_unlock(r);
		if (p)
		{
			label_okay:
			return r;
		}
	}
	return NULL;
}

miko_form_w* miko_form_push_1_zero(miko_form_w *restrict r, miko_major_vtype_t vtype)
{
	miko_form_t *restrict p;
	uintptr_t pos;
	if ((pos = _impl_(r)->form.count) < _impl_(r)->limit)
	{
		p = _impl_(r)->form.form + pos;
		p->var.u64 = 0;
		p->vtype = vtype;
		_impl_(r)->form.count = pos + 1;
		return r;
	}
	return NULL;
}

miko_form_w* miko_form_push_1_copy(miko_form_w *restrict r, const miko_form_t *restrict value)
{
	miko_form_t *restrict p;
	uintptr_t pos;
	if ((pos = _impl_(r)->form.count) < _impl_(r)->limit)
	{
		p = _impl_(r)->form.form + pos;
		p->var = value->var;
		p->vtype = value->vtype;
		_impl_(r)->form.count = pos + 1;
		return r;
	}
	return NULL;
}

miko_form_w* miko_form_push_n_zero(miko_form_w *restrict r, miko_major_vtype_t vtype, miko_count_t count)
{
	miko_form_t *restrict p;
	uintptr_t pos;
	if (count && count <= _impl_(r)->limit - (pos = _impl_(r)->form.count))
	{
		p = _impl_(r)->form.form;
		count += pos;
		while (pos < count)
		{
			p[pos].var.u64 = 0;
			p[pos].vtype = vtype;
			pos += 1;
		}
		_impl_(r)->form.count = count;
		return r;
	}
	return NULL;
}

miko_form_w* miko_form_push_n_copy(miko_form_w *restrict r, const miko_form_t *restrict array, miko_count_t count)
{
	miko_form_t *restrict p;
	uintptr_t pos;
	if (count && count <= _impl_(r)->limit - (pos = _impl_(r)->form.count))
	{
		p = _impl_(r)->form.form;
		memcpy(p + pos, array, count * sizeof(miko_form_t));
		_impl_(r)->form.count = (count += pos);
		while (pos < count)
		{
			if (p[pos].vtype == miko_major_vtype__refer)
			{
				if (p[pos].var.refer)
					refer_save(p[pos].var.refer);
			}
			else if (p[pos].vtype == miko_major_vtype__wink)
			{
				if (p[pos].var.wink)
					miko_wink_used(p[pos].var.wink);
			}
			pos += 1;
		}
		return r;
	}
	return NULL;
}

void miko_form_pop(miko_form_w *restrict r, miko_count_t count)
{
	miko_form_t *restrict p;
	uintptr_t tail;
	if (count)
	{
		if (count > (tail = _impl_(r)->form.count))
			count = tail;
		count = tail - count;
		_impl_(r)->form.count = count;
		p = _impl_(r)->form.form;
		while (count < tail)
		{
			if (p[count].vtype == miko_major_vtype__refer && p[count].var.refer)
				refer_free(p[count].var.refer);
			count += 1;
		}
	}
}

void miko_form_keep_count(miko_form_w *restrict r, miko_count_t count)
{
	miko_form_t *restrict p;
	uintptr_t tail;
	if (count < (tail = _impl_(r)->form.count))
	{
		_impl_(r)->form.count = count;
		p = _impl_(r)->form.form;
		while (count < tail)
		{
			if (p[count].vtype == miko_major_vtype__refer && p[count].var.refer)
				refer_free(p[count].var.refer);
			count += 1;
		}
	}
}
