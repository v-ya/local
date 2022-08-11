#ifndef _image_view_inst_h_
#define _image_view_inst_h_

#include <refer.h>

typedef struct inst_s inst_s;

inst_s* inst_alloc(const char *restrict path, uint32_t multicalc, uint32_t bgcolor, uint32_t shm_enable);
inst_s* inst_begin(inst_s *restrict r);
void inst_free(inst_s *restrict r);
void inst_wait(inst_s *restrict r);

#endif
