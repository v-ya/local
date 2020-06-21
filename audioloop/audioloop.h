#ifndef _audioloop_h_
#define _audioloop_h_

#include <stdint.h>
#include <pthread.h>

typedef struct aloop_input_s {
	int32_t *data;
	uint32_t size;
	uint32_t sampfre;
	uint32_t channels;
	uint32_t frames;
} aloop_input_s;

typedef struct aloop_s {
	struct pcmplay_s *player;
	uint32_t sampfre;
	uint32_t channels;
	aloop_input_s *next;
	aloop_input_s *ai1;
	aloop_input_s *ai2;
	aloop_input_s *ai3;
	volatile uint32_t status;
	uint32_t usleep_time;
	pthread_t thread;
} aloop_s;

aloop_input_s* aloop_input_alloc(void);
aloop_input_s* aloop_input_set_frames(register aloop_input_s *restrict ai, uint32_t n);
aloop_input_s* aloop_input_set_double(register aloop_input_s *restrict ai, double *restrict *restrict v, uint32_t channels, uint32_t sampfre, uint32_t frames);

aloop_s* aloop_alloc(void);
aloop_s* aloop_update_double(aloop_s *restrict r, double *restrict *restrict v, uint32_t channels, uint32_t sampfre, uint32_t frames);

void aloop_clear(void);

#endif
