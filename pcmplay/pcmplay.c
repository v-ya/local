#include "pcmplay.h"
#include <alsa/asoundlib.h>

struct pcmplay_s {
	snd_pcm_t *pcm;
	uint32_t channels;
	uint32_t sampfre;
	uint32_t bpf;
	uint32_t fpw;
};

static void pcmplay_free_func(pcmplay_s *restrict r)
{
	if (r->pcm)
	{
		snd_pcm_drop(r->pcm);
		snd_pcm_drain(r->pcm);
		snd_pcm_close(r->pcm);
	}
}

int pcmplay_uini(void)
{
	return snd_config_update_free_global();
}

pcmplay_s* pcmplay_alloc(uint32_t channels, uint32_t sampfre, pcmplay_format_t format)
{
	register pcmplay_s *restrict r;
	if (channels && sampfre && format >= 0 && format < pcmplay_format_max)
	{
		r = (pcmplay_s *) refer_alloz(sizeof(pcmplay_s));
		if (r)
		{
			refer_set_free(r, (refer_free_f) pcmplay_free_func);
			if (!snd_pcm_open(&r->pcm, "default", SND_PCM_STREAM_PLAYBACK, 0))
			{
				snd_pcm_hw_params_t *params;
				snd_pcm_format_t pfm;
				snd_pcm_uframes_t frames;
				int dir;
				params = NULL;
				snd_pcm_hw_params_alloca(&params);
				if (params)
				{
					r->channels = channels;
					r->sampfre = sampfre;
					switch (format)
					{
						case pcmplay_format_s32:
							pfm = SND_PCM_FORMAT_S32;
							r->bpf = channels << 2;
							break;
						case pcmplay_format_float:
							pfm = SND_PCM_FORMAT_FLOAT;
							r->bpf = channels << 2;
							break;
						case pcmplay_format_double:
							pfm = SND_PCM_FORMAT_FLOAT64;
							r->bpf = channels << 3;
							break;
						default:
							pfm = SND_PCM_FORMAT_UNKNOWN;
							break;
					}
					dir = 0;
					snd_pcm_hw_params_any(r->pcm, params);
					if (r->bpf && !snd_pcm_hw_params_set_access(r->pcm, params, SND_PCM_ACCESS_RW_INTERLEAVED) &&
						!snd_pcm_hw_params_set_format(r->pcm, params, pfm) &&
						!snd_pcm_hw_params_set_channels(r->pcm, params, channels) &&
						!snd_pcm_hw_params_set_rate_near(r->pcm, params, &sampfre, &dir) &&
						!snd_pcm_hw_params(r->pcm, params))
					{
						frames = 0;
						snd_pcm_hw_params_get_period_size(params, &frames, &dir);
						if ((r->fpw = frames)) return r;
					}
				}
			}
			refer_free(r);
		}
	}
	return NULL;
}

uint32_t pcmplay_period_frames(pcmplay_s *restrict pp)
{
	return pp->fpw;
}

int pcmplay_prepare(pcmplay_s *restrict pp)
{
	return snd_pcm_prepare(pp->pcm);
}

void pcmplay_write(pcmplay_s *restrict pp, void *restrict data, uint32_t frames)
{
	snd_pcm_sframes_t r;
	while (frames > pp->fpw)
	{
		r = snd_pcm_writei(pp->pcm, data, pp->fpw);
		if (r == -EPIPE)
		{
			if (!snd_pcm_prepare(pp->pcm))
				snd_pcm_writei(pp->pcm, data, pp->fpw);
		}
		data = (char *) data + (pp->fpw * pp->bpf);
		frames -= pp->fpw;
	}
	r = snd_pcm_writei(pp->pcm, data, frames);
	if (r == -EPIPE)
	{
		if (!snd_pcm_prepare(pp->pcm))
			snd_pcm_writei(pp->pcm, data, frames);
	}
}

int32_t pcmplay_write_once(pcmplay_s *restrict pp, register void *restrict *restrict data, register uint32_t *restrict frames)
{
	uint32_t n;
	int32_t r;
	if ((n = *frames) > pp->fpw) n = pp->fpw;
	r = snd_pcm_writei(pp->pcm, *data, n);
	if (r == -EPIPE)
	{
		if (!snd_pcm_prepare(pp->pcm))
			r = snd_pcm_writei(pp->pcm, *data, n);
	}
	*frames -= n;
	*(char *restrict *restrict) data += n * pp->bpf;
	return r;
}

int pcmplay_wait(pcmplay_s *restrict pp)
{
	return snd_pcm_drain(pp->pcm);
}

int32_t pcmplay_get_avail(pcmplay_s *restrict pp)
{
	return snd_pcm_avail_update(pp->pcm);
}
