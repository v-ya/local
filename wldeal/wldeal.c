#define _DEFAULT_SOURCE
#include "wldeal.h"
#include <stdlib.h>
#include <dylink.h>
#include <wav.h>
#include <wavelike.h>

#define _string_symbol(_s)   #_s
#define string_symbol(_s)   _string_symbol(_s)

void dylink_pool_set_preset(dylink_pool_t *dyp)
{
	#define _dylink_pool_set_preset(f)  dylink_pool_set_func(dyp, #f, f)
	// stdio
	_dylink_pool_set_preset(puts);
	_dylink_pool_set_preset(printf);
	_dylink_pool_set_preset(sprintf);
	_dylink_pool_set_preset(fopen);
	_dylink_pool_set_preset(fclose);
	_dylink_pool_set_preset(fread);
	_dylink_pool_set_preset(fwrite);
	_dylink_pool_set_preset(fseek);
	_dylink_pool_set_preset(ftell);

	// math
	_dylink_pool_set_preset(acos);
	_dylink_pool_set_preset(asin);
	_dylink_pool_set_preset(atan);
	_dylink_pool_set_preset(atan2);
	_dylink_pool_set_preset(cos);
	_dylink_pool_set_preset(sin);
	_dylink_pool_set_preset(tan);
	_dylink_pool_set_preset(cosh);
	_dylink_pool_set_preset(sinh);
	_dylink_pool_set_preset(tanh);
	_dylink_pool_set_preset(acosh);
	_dylink_pool_set_preset(asinh);
	_dylink_pool_set_preset(atanh);
	_dylink_pool_set_preset(exp);
	_dylink_pool_set_preset(frexp);
	_dylink_pool_set_preset(ldexp);
	_dylink_pool_set_preset(log);
	_dylink_pool_set_preset(log10);
	_dylink_pool_set_preset(modf);
	_dylink_pool_set_preset(pow);
	_dylink_pool_set_preset(sqrt);
	_dylink_pool_set_preset(hypot);
	_dylink_pool_set_preset(ceil);
	_dylink_pool_set_preset(fabs);
	_dylink_pool_set_preset(floor);
	_dylink_pool_set_preset(fmod);

	// refer
	_dylink_pool_set_preset(refer_alloc);
	_dylink_pool_set_preset(refer_alloz);
	_dylink_pool_set_preset(refer_free);
	_dylink_pool_set_preset(refer_set_free);
	_dylink_pool_set_preset(refer_save);
	_dylink_pool_set_preset(refer_save_number);

	// json
	_dylink_pool_set_preset(json_free);
	_dylink_pool_set_preset(json_decode);
	_dylink_pool_set_preset(json_length);
	_dylink_pool_set_preset(json_encode_intext);
	_dylink_pool_set_preset(json_encode);
	_dylink_pool_set_preset(json_create_null);
	_dylink_pool_set_preset(json_create_boolean);
	_dylink_pool_set_preset(json_create_integer);
	_dylink_pool_set_preset(json_create_floating);
	_dylink_pool_set_preset(json_create_string);
	_dylink_pool_set_preset(json_create_array);
	_dylink_pool_set_preset(json_create_object);
	_dylink_pool_set_preset(json_array_find);
	_dylink_pool_set_preset(json_object_find);
	_dylink_pool_set_preset(json_array_get);
	_dylink_pool_set_preset(json_object_get);
	_dylink_pool_set_preset(json_array_set);
	_dylink_pool_set_preset(json_object_set);
	_dylink_pool_set_preset(json_array_delete);
	_dylink_pool_set_preset(json_object_delete);
	_dylink_pool_set_preset(json_load);
	_dylink_pool_set_preset(json_find);
	_dylink_pool_set_preset(json_set);
	_dylink_pool_set_preset(json_get_null);
	_dylink_pool_set_preset(json_get_boolean);
	_dylink_pool_set_preset(json_get_integer);
	_dylink_pool_set_preset(json_get_floating);
	_dylink_pool_set_preset(json_get_string);
	_dylink_pool_set_preset(json_get_array);
	_dylink_pool_set_preset(json_get_object);
	_dylink_pool_set_preset(json_copy);

	// note_details
	_dylink_pool_set_preset(note_details_alloc);
	_dylink_pool_set_preset(note_details_gen);
	_dylink_pool_set_preset(note_details_get);

	// scatterplot
	_dylink_pool_set_preset(scatterplot_alloc);
	_dylink_pool_set_preset(scatterplot_set_image);
	_dylink_pool_set_preset(scatterplot_set_view);
	_dylink_pool_set_preset(scatterplot_clear);
	_dylink_pool_set_preset(scatterplot_pos);
	_dylink_pool_set_preset(scatterplot_save);

	#undef _dylink_pool_set_preset
}

static uint64_t argv_s2u(const char *s)
{
	uint64_t u;
	unsigned int i;
	i = 8;
	u = 0;
	while (i && *s)
	{
		u = (u << 8) | *(unsigned char *) s;
		++s;
		--i;
	}
	return u;
}

static size_t argv_size(const char *s)
{
	char *t;
	size_t r;
	t = NULL;
	r = strtoul(s, &t, 10);
	if (t)
	{
		switch (*t)
		{
			case 'G':
			case 'g':
				r <<= 10;
				// fall through
			case 'M':
			case 'm':
				r <<= 10;
				// fall through
			case 'K':
			case 'k':
				r <<= 10;
				break;
		}
	}
	return r;
}

static int dylink_pool_report_func(void *pri, dylink_pool_report_type_t type, const char *symbol, void *ptr, void **plt)
{
	if (type == dylink_pool_report_type_import_fail)
		printf("[dylink-pool] import <%s> fail\n", symbol);
	else if (type == dylink_pool_report_type_export_fail)
		printf("[dylink-pool] export <%s> fail\n", symbol);
	return 0;
}

int wavlike_loop_deal(double v[], uint32_t frames, uint32_t sampfre, double t, uint32_t ndmax, wavelike_kernal_deal_f deal, refer_t pri)
{
	note_details_s *nd;
	wavelike_kernal_arg_t arg;
	double p, ul;
	uint32_t up, ut, i;
	i = 0;
	nd = note_details_alloc(ndmax);
	if (nd)
	{
		arg.time = (double) frames / sampfre;
		t *= sampfre;
		p = wavelike_first(v, frames, NULL, &t, &ul);
		while (p < frames)
		{
			up = (uint32_t) (p + 0.5);
			ut = (uint32_t) (p + t + 0.5) - up;
			if (ut && up + ut <= frames)
			{
				arg.a = wavelike_loadness(v, frames, p, t);
				note_details_get(nd, v + up, ut);
				arg.offset = p / sampfre;
				arg.length = t / sampfre;
				arg.basefre = sampfre / t;
				deal(pri, i, nd, &arg);
			}
			p = wavelike_next(v, frames, p, &t, &ul);
			++i;
		}
		refer_free(nd);
	}
	return !i;
}

int main(int argc, const char **argv)
{
	dylink_pool_t *dyp;
	const char *sym_kalloc;
	const char *sym_kdeal;
	wavelike_kernal_alloc_f kalloc;
	wavelike_kernal_deal_f kdeal;
	refer_t kpri;
	wav_load_t *wl;
	double *v;
	const char *input;
	const char *kernal;
	double basefre;
	size_t xmsize;
	uint32_t ndmax, channel, sampfre, frames;
	int i;
	sym_kalloc = string_symbol(symbol_kernal_alloc);
	sym_kdeal = string_symbol(symbol_kernal_deal);
	input = kernal = NULL;
	basefre = 0;
	xmsize = 1 << 20;
	ndmax = 128;
	channel = 0;
	if (argc > 1)
	{
		i = 1;
		while (i < argc)
		{
			if (*argv[i] == '-')
			{
				switch (argv_s2u(argv[i] + 1))
				{
					case 'm':
						if (++i >= argc) goto Err;
						xmsize = argv_size(argv[i]);
						break;
					case 'k':
						if (++i >= argc) goto Err;
						kernal = argv[i];
						break;
					case 'bf':
						if (++i >= argc) goto Err;
						basefre = strtod(argv[i], NULL);
						break;
					case 'dm':
						if (++i >= argc) goto Err;
						ndmax = strtoul(argv[i], NULL, 10);
						break;
					case 'c':
						if (++i >= argc) goto Err;
						channel = strtoul(argv[i], NULL, 10);
						break;
					case 'h':
					case (((uint64_t) '-' << 32) | (uint64_t) 'help'):
						goto label_help;
					case '-':
						if (++i >= argc) goto Err;
						goto label_input;
					default:
						goto Err;
				}
			}
			else
			{
				label_input:
				input = argv[i++];
				break;
			}
			++i;
			
		}
		if (!input || !kernal || !xmsize || basefre <= 0 || !ndmax) goto Err;
		argc -= i;
		argv += i;
		if (!argc) argv = NULL;
		i = -1;
		dyp = dylink_pool_alloc(dylink_mechine_x86_64, m_x86_64_dylink_set, m_x86_64_dylink_plt_set, xmsize);
		if (dyp)
		{
			dylink_pool_set_preset(dyp);
			dylink_pool_set_report(dyp, dylink_pool_report_func, NULL);
			if (!dylink_pool_load_file(dyp, kernal))
			{
				kalloc = (wavelike_kernal_alloc_f) dylink_pool_get_symbol(dyp, sym_kalloc, NULL);
				kdeal = (wavelike_kernal_deal_f) dylink_pool_get_symbol(dyp, sym_kdeal, NULL);
				if (kalloc && kdeal)
				{
					wl = wav_load(input);
					if (wl)
					{
						frames = wav_load_frames(wl);
						sampfre = wav_load_sampfre(wl);
						if (frames && sampfre)
						{
							if (channel < wav_load_channels(wl))
							{
								v = (double *) malloc(sizeof(double) * frames);
								if (v && (frames = wav_load_get_double(wl, v, frames, channel)))
								{
									kpri = kalloc(argc, argv);
									if (!wavlike_loop_deal(v, frames, sampfre, 1 / basefre, ndmax, kdeal, kpri))
										i = 0;
									else printf("wavlike loop fail\n");
									if (kpri) refer_free(kpri);
								}
								else printf("load wav[%s] channel[%u] fail\n", input, channel);
								if (v) free(v);
							}
							else printf("wav[%s] channel(%u) >= wav.channels(%u)\n", input, channel, wav_load_channels(wl));
						}
						else printf("wav[%s] frams = %u, sampfre = %u\n", input, frames, sampfre);
						wav_free(wl);
					}
					else printf("load wav[%s] fail\n", input);
				}
				else printf(
					"miss global function: %s%s%s\n",
					kalloc?"":sym_kalloc,
					(kalloc || kdeal)?"":", ",
					kdeal?"":sym_kdeal
				);
			}
			else printf("load kernal[%s] fail\n", kernal);
			dylink_pool_free(dyp);
		}
		else printf("alloc dylink pool fail\n");
	}
	else
	{
		label_help:
		printf("%s [-m xmsize (1M)] <-k kernal.dy> <-bf first-basefre (>0)> [-dm ndmax (128)] [-c channel (0)] <wav> [ ... ]\n", argv[0]);
		i = 0;
	}
	return i;
	Err:
	printf("please use '%s -h/--help'\n", argv[0]);
	return -1;
}
