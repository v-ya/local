#include "args.h"
#include "window.h"

typedef struct env_s {
	volatile uintptr_t is_close;
} env_s;

void we_close_func(env_s *restrict env, window_s *window)
{
	env->is_close = 1;
}

int main(int argc, const char *argv[])
{
	// args_t args;
	// if (args_get(&args, argc, argv))
	{
		window_s *restrict w;
		env_s *restrict env;
		env = (env_s *) refer_alloz(sizeof(env_s));
		if (env)
		{
			w = window_alloc(0, 0, 1960, 1080, 24);
			if (w)
			{
				window_set_event_data(w, env);
				window_register_event_close(w, (window_event_close_f) we_close_func);
				if (window_set_event(w, (const window_event_t []) {
					window_event_close, window_event_null}))
				{
					if (window_map(w))
					{
						while (!env->is_close)
						{
							window_usleep(5000);
							window_do_all_events(w);
						}
						window_unmap(w);
					}
				}
				refer_free(w);
			}
			refer_free(env);
		}
	}
	return 0;
}
