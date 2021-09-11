#include "inner.h"
#include <string.h>

#define label_size          64
#define limit_label_length  63

typedef struct udns_inner_label_ctx_t {
	uintptr_t n_rdata;
	char rdata[label_size];
} udns_inner_label_ctx_t;

static udns_inner_label_ctx_t* udns_inner_label_get(udns_inner_label_ctx_t *restrict r, const char *restrict s, const char **restrict ps)
{
	char *restrict t;
	uintptr_t i, n;
	r->n_rdata = 0;
	if ((t = strchr(s, '.')))
		n = (uintptr_t) t - (uintptr_t) s;
	else n = strlen(s);
	if (ps) *ps = s + n;
	if (!n) goto label_okay;
	if (n <= limit_label_length)
	{
		for (i = 0; i < n; ++i)
		{
			if (s[i] & 0x80)
				goto label_punycode;
			r->rdata[i] = s[i];
		}
		label_okay:
		r->n_rdata = n;
		return r;
		label_punycode:
		{
			uint32_t unicode[label_size];
			udns_inner_punycode_point_t point[label_size];
			if ((n = udns_inner_unicode4utf8(unicode, label_size, s, n)))
			{
				r->rdata[0] = 'x';
				r->rdata[1] = 'n';
				r->rdata[2] = '-';
				r->rdata[3] = '-';
				if ((n = udns_inner_punycode4unicode((uint8_t *) r->rdata + 4, label_size - 4, unicode, n, point)) &&
					(n += 4) <= limit_label_length)
					goto label_okay;
			}
		}
	}
	return NULL;
}

static udns_inner_label_ctx_t* udns_inner_label_set(udns_inner_label_ctx_t *restrict r, const uint8_t *restrict p, uintptr_t size)
{
	if (size > limit_label_length)
		goto label_fail;
	if (size >= 4 && p[0] == 'x' && p[1] == 'n' && p[2] == '-' && p[3] == '-')
		goto label_punycode;
	if (size)
		memcpy(r->rdata, p, size);
	label_okay:
	r->n_rdata = size;
	return r;
	label_punycode:
	{
		uint32_t unicode[label_size];
		p += 4;
		size -= 4;
		if (!size)
			goto label_okay;
		if ((size = udns_inner_punycode2unicode(p, size, unicode, label_size)) && size <= limit_label_length)
		{
			if ((size = udns_inner_unicode2utf8(unicode, size, r->rdata, label_size)) && size <= limit_label_length)
				goto label_okay;
		}
	}
	label_fail:
	return NULL;
}

udns_inner_labels_t* udns_inner_labels4string(udns_inner_labels_t *restrict r, const char *restrict s, uintptr_t *restrict size)
{
	udns_inner_label_ctx_t ctx;
	uintptr_t n;
	n = 0;
	do {
		if (!udns_inner_label_get(&ctx, s, (const char **) &s))
			goto label_fail;
		if (*s == '.') ++s;
		if (n + ctx.n_rdata >= sizeof(r->data))
			goto label_fail;
		r->data[n++] = (uint8_t) ctx.n_rdata;
		if (ctx.n_rdata)
			memcpy(r->data + n, ctx.rdata, ctx.n_rdata);
		n += ctx.n_rdata;
	} while (ctx.n_rdata);
	if (size) *size = n;
	return r;
	label_fail:
	return NULL;
}

udns_inner_labels_t* udns_inner_labels2string(udns_inner_labels_t *restrict r, const uint8_t *restrict p, uintptr_t size, uintptr_t *restrict pos, uintptr_t *restrict length)
{
	udns_inner_label_ctx_t ctx;
	uintptr_t i, n, l;
	i = n = 0;
	if (pos) i = *pos;
	while (i < size && (l = (uintptr_t) p[i++]))
	{
		if (i + l >= size)
			goto label_fail;
		if (!udns_inner_label_set(&ctx, p + i, l))
			goto label_fail;
		i += l;
		if (n + ctx.n_rdata >= sizeof(r->data))
			goto label_fail;
		if (ctx.n_rdata)
			memcpy(r->data + n, ctx.rdata, ctx.n_rdata);
		n += ctx.n_rdata;
		r->data[n++] = '.';
	}
	if (n) --n;
	r->data[n] = 0;
	if (pos) *pos = i;
	if (length) *length = n;
	return r;
	label_fail:
	return NULL;
}
