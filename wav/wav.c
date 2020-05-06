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

struct wav_load_s {
	wav_header_s header;
	int32_t data[];
}  __attribute__((packed));

wav_load_s* wav_load(const char *path)
{
	wav_load_s *w;
	FILE *fp;
	size_t size;
	w = NULL;
	fp = fopen(path, "rb");
	if (fp)
	{
		fseek(fp, 0, SEEK_END);
		size = ftell(fp);
		if (size >= sizeof(wav_header_s))
		{
			w = (wav_load_s *) malloc(size);
			if (w)
			{
				fseek(fp, 0, SEEK_SET);
				if (fread(w, 1, size, fp) != size)
				{
					Err:
					free(w);
					w = NULL;
				}
				else
				{
					if (w->header.riff != (uint32_t) 'FFIR') goto Err;
					if (w->header.length > size - 8) w->header.length = size - 8;
					if (w->header.wave != (uint32_t) 'EVAW') goto Err;
					if (w->header.fmt != (uint32_t) ' tmf') goto Err;
					if (w->header.fmt_length != 16) goto Err;
					if (w->header.encode != 1) goto Err;
					if (!w->header.channels || !w->header.sampfre) goto Err;
					if (w->header.bits != 32) goto Err;
					if (w->header.data != (uint32_t) 'atad') goto Err;
					if (w->header.data_length > w->header.length - (sizeof(wav_header_s) - 8))
						w->header.data_length = w->header.length - (sizeof(wav_header_s) - 8);
				}
			}
		}
		fclose(fp);
	}
	return w;
}

wav_load_s* pcms32le_load(const char *path, uint32_t channels, uint32_t samefre)
{
	wav_load_s *w;
	FILE *fp;
	size_t size;
	w = NULL;
	fp = fopen(path, "rb");
	if (fp)
	{
		fseek(fp, 0, SEEK_END);
		size = ftell(fp);
		if (size >= sizeof(wav_header_s))
		{
			w = (wav_load_s *) malloc(size + sizeof(wav_header_s));
			if (w)
			{
				memset(&w->header, 0, sizeof(wav_header_s));
				fseek(fp, 0, SEEK_SET);
				if (fread(w->data, 1, size, fp) != size)
				{
					free(w);
					w = NULL;
				}
				else
				{
					w->header.channels = channels;
					w->header.sampfre = samefre;
					w->header.data_length = (uint32_t) size;
				}
			}
		}
		fclose(fp);
	}
	return w;
}

void wav_free(wav_load_s *wl)
{
	if (wl) free(wl);
}

uint32_t wav_load_channels(wav_load_s *wl)
{
	return wl->header.channels;
}

uint32_t wav_load_sampfre(wav_load_s *wl)
{
	return wl->header.sampfre;
}

uint32_t wav_load_frames(wav_load_s *wl)
{
	return wl->header.data_length / sizeof(uint32_t) / wl->header.channels;
}

uint32_t wav_load_get_double(wav_load_s *wl, double v[], uint32_t frames, uint32_t channel)
{
	uint32_t n;
	int32_t *p;
	n = 0;
	if (channel < wl->header.channels)
	{
		n = wl->header.data_length / sizeof(uint32_t) / wl->header.channels;
		if (frames < n) n = frames;
		p = wl->data + channel;
		channel = wl->header.channels;
		for (frames = 0; frames < n; ++frames)
		{
			v[frames] = (double) *p / 0x7fffffffu;
			p += channel;
		}
	}
	return n;
}

uint32_t wav_load_get_float(wav_load_s *wl, float v[], uint32_t frames, uint32_t channel)
{
	uint32_t n;
	int32_t *p;
	n = 0;
	if (channel < wl->header.channels)
	{
		n = wl->header.data_length / sizeof(uint32_t) / wl->header.channels;
		if (frames < n) n = frames;
		p = wl->data + channel;
		channel = wl->header.channels;
		for (frames = 0; frames < n; ++frames)
		{
			v[frames] = (float) *p / 0x7fffffffu;
			p += channel;
		}
	}
	return n;
}

uint32_t wav_load_get_int32(wav_load_s *wl, int32_t v[], uint32_t frames, uint32_t channel)
{
	uint32_t n;
	int32_t *p;
	n = 0;
	if (channel < wl->header.channels)
	{
		n = wl->header.data_length / sizeof(uint32_t) / wl->header.channels;
		if (frames < n) n = frames;
		p = wl->data + channel;
		channel = wl->header.channels;
		for (frames = 0; frames < n; ++frames)
		{
			v[frames] = *p;
			p += channel;
		}
	}
	return n;
}
