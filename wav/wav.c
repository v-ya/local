#include "wav.h"
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

#define wav_header_size  44

static int wav_save_nf(const char *path, void *data, uint32_t frames, uint32_t channels, uint32_t samefre)
{
	static wav_header_s header = {
		.riff = 'FFIR',
		.length = 0,
		.wave = 'EVAW',
		.fmt = ' tmf',
		.fmt_length = 16,
		.encode = 1,
		.channels = 0,
		.sampfre = 0,
		.bytesps = 0,
		.bytespf = 0,
		.bits = 32,
		.data = 'atad',
		.data_length = 0
	};
	wav_header_s h;
	FILE *fp;
	int r;
	r = -1;
	memcpy(&h, &header, sizeof(h));
	h.length = ((frames * channels) << 2) + (sizeof(h) - 8);
	h.channels = channels;
	h.sampfre = samefre;
	h.bytesps = (samefre * channels) << 2;
	h.bytespf = channels << 2;
	h.data_length = (frames * channels) << 2;
	fp = fopen(path, "wb");
	if (fp)
	{
		if (fwrite(&h, 1, sizeof(h), fp) == sizeof(h))
		{
			if (fwrite(data, channels << 2, frames, fp) == frames)
				r = 0;
		}
		fclose(fp);
	}
	return r;
}

int wav_save_double(const char *path, double *v[], uint32_t frames, uint32_t channels, uint32_t samefre)
{
	int32_t *data, *p;
	uint32_t i, c;
	int r;
	r = -1;
	p = data = (int32_t *) malloc(((size_t) frames * channels) << 2);
	if (data)
	{
		for (i = 0; i < frames; ++i)
		{
			for (c = 0; c < channels; ++c)
			{
				*p++ = (int32_t) (v[c][i] * 0x7fffffffu);
			}
		}
		r = wav_save_nf(path, data, frames, channels, samefre);
		free(data);
	}
	return r;
}

int wav_save_float(const char *path, float *v[], uint32_t frames, uint32_t channels, uint32_t samefre)
{
	int32_t *data, *p;
	uint32_t i, c;
	int r;
	r = -1;
	p = data = (int32_t *) malloc(((size_t) frames * channels) << 2);
	if (data)
	{
		for (i = 0; i < frames; ++i)
		{
			for (c = 0; c < channels; ++c)
			{
				*p++ = (int32_t) ((double) v[c][i] * 0x7fffffffu);
			}
		}
		r = wav_save_nf(path, data, frames, channels, samefre);
		free(data);
	}
	return r;
}

int wav_save_int32(const char *path, int32_t *v[], uint32_t frames, uint32_t channels, uint32_t samefre)
{
	int32_t *data, *p;
	uint32_t i, c;
	int r;
	r = -1;
	p = data = (int32_t *) malloc(((size_t) frames * channels) << 2);
	if (data)
	{
		for (i = 0; i < frames; ++i)
		{
			for (c = 0; c < channels; ++c)
			{
				*p++ = v[c][i];
			}
		}
		r = wav_save_nf(path, data, frames, channels, samefre);
		free(data);
	}
	return r;
}
