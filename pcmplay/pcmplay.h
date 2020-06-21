#ifndef _pcmplay_h_
#define _pcmplay_h_

#include <refer.h>

typedef struct pcmplay_s pcmplay_s;
typedef enum pcmplay_format_t {
	pcmplay_format_s32,
	pcmplay_format_float,
	pcmplay_format_double,
	pcmplay_format_max
} pcmplay_format_t;

int pcmplay_clear(void);
pcmplay_s* pcmplay_alloc(uint32_t channels, uint32_t sampfre, pcmplay_format_t format);
uint32_t pcmplay_period_frames(pcmplay_s *restrict pp);
int pcmplay_prepare(pcmplay_s *restrict pp);
void pcmplay_write(pcmplay_s *restrict pp, void *restrict data, uint32_t frames);
int32_t pcmplay_write_once(pcmplay_s *restrict pp, void *restrict *restrict data, uint32_t *restrict frames);
int pcmplay_wait(pcmplay_s *restrict pp);
int32_t pcmplay_get_avail(pcmplay_s *restrict pp);

#endif
