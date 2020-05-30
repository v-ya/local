#ifndef _phoneme_buffer_h_
#define _phoneme_buffer_h_

#include <stdint.h>
#include <refer.h>
#include <note.h>

typedef struct phoneme_buffer_s {
	double *buffer;
	uint32_t sampfre;
	uint32_t frames_max;
	uint32_t frames;
	uint32_t frames_size;
} phoneme_buffer_s;

phoneme_buffer_s* phoneme_buffer_alloc(uint32_t sampfre, uint32_t frames);
phoneme_buffer_s* phoneme_buffer_set_sampfre(phoneme_buffer_s *restrict pb, uint32_t sampfre);
phoneme_buffer_s* phoneme_buffer_set_frames(phoneme_buffer_s *restrict pb, double t);
phoneme_buffer_s* phoneme_buffer_gen_note(phoneme_buffer_s *restrict pb, note_s *restrict note, double pos, double length, double volume, double basefre);

#endif
