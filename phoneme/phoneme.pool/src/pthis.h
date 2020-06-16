#ifndef _pthis_h_
#define _pthis_h_

#include <phoneme/phoneme_script.h>

extern phoneme_script_s *pthis_phoneme_script;
dyl_alias(this.var.ps, phoneme_script);
extern phoneme_pool_s *phis_phoneme_pool;
dyl_alias(this.var.pp, phoneme_pool);

void* pthis_symbol(const char *restrict symbol);
dyl_alias(this.func.symbol, pthis_symbol);

note_envelope_f pthis_envelope(const char *restrict symbol);
dyl_alias(this.func.envelope, pthis_envelope);

note_base_frequency_f pthis_basefre(const char *restrict symbol);
dyl_alias(this.func.basefre, pthis_basefre);

note_details_f pthis_details(const char *restrict symbol);
dyl_alias(this.func.details, pthis_details);

phoneme_arg2pri_f pthis_arg2pri(const char *restrict symbol);
dyl_alias(this.func.arg2pri, pthis_arg2pri);

#endif
