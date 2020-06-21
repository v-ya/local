#ifndef _phoneme_output_h_
#define _phoneme_output_h_

#include <stdint.h>
#include <pthread.h>
#include <refer.h>
#include <note.h>

typedef struct phoneme_buffer_t {
	double *buffer;
	size_t size;
} phoneme_buffer_t;

typedef struct phoneme_output_core_t {
	phoneme_buffer_t buffer;
	note_s *note;
	double volume;
	double basefre;
	uint32_t length;
	uint32_t pos;
	uint32_t sampfre;
	uint32_t usleep_time;
	volatile uint32_t running;
	volatile uint32_t busy;
	pthread_t thread;
	uintptr_t tid;
} phoneme_output_core_t;

typedef struct phoneme_output_s {
	phoneme_buffer_t output;
	uint32_t sampfre;
	uint32_t frames;
	uint32_t core_number;
	uint32_t core_index;
	phoneme_output_core_t core[];
} phoneme_output_s;

phoneme_output_s* phoneme_output_alloc(uint32_t sampfre, uint32_t frames, uint32_t core_number, uint32_t usleep_time);
phoneme_output_s* phoneme_output_set_priority(phoneme_output_s *restrict po, double nice);
phoneme_output_s* phoneme_output_clear(phoneme_output_s *restrict po);
phoneme_output_s* phoneme_output_set_sampfre(phoneme_output_s *restrict po, uint32_t sampfre);
phoneme_output_s* phoneme_output_set_frames(phoneme_output_s *restrict po, double t);
phoneme_output_s* phoneme_output_gen_note(phoneme_output_s *restrict po, note_s *restrict note, double pos, double length, double volume, double basefre);
phoneme_output_s* phoneme_output_join(phoneme_output_s *restrict po);

#endif
