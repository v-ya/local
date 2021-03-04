#include "script_header.h"
#include "script_parse.h"

static script_t* header_tag(script_t *restrict script, pocket_saver_s *restrict saver, const char *s)
{
	if (pocket_saver_set_tag(saver, s))
		return script;
	return NULL;
}

static script_t* header_version(script_t *restrict script, pocket_saver_s *restrict saver, const char *s)
{
	if (pocket_saver_set_version(saver, s))
		return script;
	return NULL;
}

static script_t* header_author(script_t *restrict script, pocket_saver_s *restrict saver, const char *s)
{
	if (pocket_saver_set_author(saver, s))
		return script;
	return NULL;
}

static script_t* header_time(script_t *restrict script, pocket_saver_s *restrict saver, const char *s)
{
	if (pocket_saver_set_time(saver, s))
		return script;
	return NULL;
}

static script_t* header_desc(script_t *restrict script, pocket_saver_s *restrict saver, const char *s)
{
	if (pocket_saver_set_description(saver, s))
		return script;
	return NULL;
}

static script_t* header_flag(script_t *restrict script, pocket_saver_s *restrict saver, const char *s)
{
	if (pocket_saver_set_flag(saver, s))
		return script;
	return NULL;
}

static script_t* header_verify(script_t *restrict script, pocket_saver_s *restrict saver, const char *s)
{
	if (pocket_saver_set_verify(saver, s))
		return script;
	return NULL;
}

hashmap_t* script_header_init(hashmap_t *restrict h)
{
	#define d(name) hashmap_set_name(h, #name, header_##name, NULL)
	if (
		d(tag) &&
		d(version) &&
		d(author) &&
		d(time) &&
		d(desc) &&
		d(flag) &&
		d(verify)
	) return h;
	return NULL;
	#undef d
}

script_t* script_header_parse(script_t *restrict script, pocket_saver_s *restrict saver, const char *restrict *restrict p)
{
	const char *restrict s, *restrict key;
	json_inode_t *v;
	script_header_f func;
	v = NULL;
	if (*(s = *p) == ':')
	{
		++s;
		skip_space(s);
		if ((key = parse_key(script->buffer, &s)) &&
			(func = hashmap_get_name(&script->header, key)))
		{
			skip_space(s);
			if (*s == '=')
			{
				++s;
				skip_space(s);
				if (*s == '\"')
				{
					if ((s = json_decode(*p = s, &v)) && func(script, saver, v->value.string))
					{
						json_free(v);
						label_ok:
						skip_space(s);
						*p = s;
						return script;
					}
				}
				else if (s[0] == 'n' && s[1] == 'u' && s[2] == 'l' && s[3] == 'l')
				{
					s += 4;
					if (func(script, saver, NULL))
						goto label_ok;
				}
			}
		}
		if (v) json_free(v);
		if (s) *p = s;
	}
	return NULL;
}
