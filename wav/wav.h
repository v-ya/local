#ifndef _wav_h_
#define _wav_h_

#include <stdint.h>

typedef struct wav_header_t {
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
} __attribute__((packed)) wav_header_t;

typedef struct wav_load_t wav_load_t;

int wav_save_double(const char *path, double *v[], uint32_t frames, uint32_t channels, uint32_t samefre);
int wav_save_float(const char *path, float *v[], uint32_t frames, uint32_t channels, uint32_t samefre);
int wav_save_int32(const char *path, int32_t *v[], uint32_t frames, uint32_t channels, uint32_t samefre);
wav_load_t* wav_load(const char *path);
wav_load_t* pcms32le_load(const char *path, uint32_t channels, uint32_t samefre);
void wav_free(wav_load_t *wl);
int wav_save_load(const char *path, wav_load_t *wl);
uint32_t wav_load_channels(wav_load_t *wl);
uint32_t wav_load_sampfre(wav_load_t *wl);
uint32_t wav_load_frames(wav_load_t *wl);
void* wav_load_data(wav_load_t *wl);
uint32_t wav_load_get_double(wav_load_t *wl, double v[], uint32_t frames, uint32_t channel);
uint32_t wav_load_get_float(wav_load_t *wl, float v[], uint32_t frames, uint32_t channel);
uint32_t wav_load_get_int32(wav_load_t *wl, int32_t v[], uint32_t frames, uint32_t channel);


#endif
