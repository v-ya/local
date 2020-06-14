#include "pcmplay.h"
#include <wav.h>
#include <stdio.h>

int main(int argc, const char *argv[])
{
	if (argc == 2)
	{
		wav_load_t *wl;
		pcmplay_s *pp;
		void *data;
		uint32_t frames;
		int r;
		r = -1;
		wl = wav_load(argv[1]);
		if (wl)
		{
			pp = pcmplay_alloc(wav_load_channels(wl), wav_load_sampfre(wl), pcmplay_format_s32);
			if (pp)
			{
				data = wav_load_data(wl);
				frames = wav_load_frames(wl);
				pcmplay_write(pp, data, frames);
				pcmplay_wait(pp);
				refer_free(pp);
				pcmplay_clear();
				r = 0;
			}
			wav_free(wl);
		}
		return r;
	}
	else printf("%s <wav-path>\n", argv[0]);
	return 0;
}

