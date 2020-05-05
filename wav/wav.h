#ifndef _wav_h_
#define _wav_h_

#include <stdint.h>

typedef struct wav_header_s {
	uint32_t riff;
	uint32_t length;
	uint32_t wave;
	uint32_t fmt;
	uint32_t fmt_length;
	uint16_t encode;
	uint16_t channels;
	uint32_t sampfre;
	uint32_t bytesps;
	uint16_t bytespf;
	uint16_t bits;
	uint32_t data;
	uint32_t data_length;
} __attribute__((packed)) wav_header_s;

int wav_save_double(const char *path, double *v[], uint32_t frames, uint32_t channels, uint32_t samefre);
int wav_save_float(const char *path, float *v[], uint32_t frames, uint32_t channels, uint32_t samefre);
int wav_save_int32(const char *path, int32_t *v[], uint32_t frames, uint32_t channels, uint32_t samefre);

#endif
