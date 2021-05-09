#include "av1_env.h"
#include "av1_ref.h"
#include "obu/obu_sequence_header.h"
#include <stdlib.h>

typedef struct av1_env_inst_t {
	av1_env_t env;
	av1_ref_t ref;
	av1_obu_sequence_header_t sequence_header;
} av1_env_inst_t;

av1_env_t* av1_env_alloc(void)
{
	register av1_env_inst_t *restrict r;
	r = (av1_env_inst_t *) malloc(sizeof(av1_env_inst_t));
	if (r)
	{
		memset(&r->env, 0, sizeof(r->env));
		if (av1_obu_sequence_header_init(r->env.sequence_header = &r->sequence_header) &&
			av1_ref_init(r->env.ref = &r->ref))
			return &r->env;
		free(r);
	}
	return NULL;
}

void av1_env_free(av1_env_t *restrict env)
{
	if (env) free(env);
}

