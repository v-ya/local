#include "../../phoneme_script.h"

static dyl_used phoneme_script_s *phoneme_script;
dyl_export(phoneme_script, this.var.ps);
static dyl_used phoneme_pool_s *phoneme_pool;
dyl_export(phoneme_pool, this.var.pp);

static dyl_used phoneme_script_s* pthis_init(phoneme_script_s *restrict ps)
{
	phoneme_script = ps;
	phoneme_pool = ps->phoneme_pool;
	return ps;
}
dyl_export(pthis_init, this.init);

void* dypool_symbol(dylink_pool_t *dp, const char *symbol, void ***plt);
dyl_alias(dypool.symbol, dypool_symbol);

static dyl_used void* pthis_symbol(const char *restrict symbol)
{
	return dypool_symbol(phoneme_pool->dypool, symbol, NULL);
}
dyl_export(pthis_symbol, this.func.symbol);

static dyl_used note_envelope_f pthis_envelope(const char *restrict symbol)
{
	return (note_envelope_f) hashmap_get_name(&phoneme_pool->envelope, symbol);
}
dyl_export(pthis_envelope, this.func.envelope);

static dyl_used note_base_frequency_f pthis_basefre(const char *restrict symbol)
{
	return (note_base_frequency_f) hashmap_get_name(&phoneme_pool->basefre, symbol);
}
dyl_export(pthis_basefre, this.func.basefre);

static dyl_used note_details_f pthis_details(const char *restrict symbol)
{
	return (note_details_f) hashmap_get_name(&phoneme_pool->details, symbol);
}
dyl_export(pthis_details, this.func.details);

static dyl_used phoneme_arg2pri_f pthis_arg2pri(const char *restrict symbol)
{
	return (phoneme_arg2pri_f) hashmap_get_name(&phoneme_pool->arg2pri, symbol);
}
dyl_export(pthis_arg2pri, this.func.arg2pri);
