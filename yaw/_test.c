#include "yaw.h"
#include <pthread.h>
#include <stdio.h>

static void verify_yaw_thread_func(yaw_s *restrict yaw)
{
	((uintptr_t *) yaw->data)[0] = (uintptr_t) refer_save_number(yaw);
	while (yaw->running)
		yaw_msleep(50);
	if (yaw->pri && yaw->data)
		((uintptr_t *) yaw->data)[1] = (uintptr_t) refer_save_number(yaw->pri);
}

int verify_yaw(void)
{
	yaw_s *yaw;
	refer_string_t string;
	uintptr_t sn[3];
	sn[0] = sn[1] = sn[2] = 0;
	string = refer_dump_string("verify_yaw");
	if (string)
	{
		yaw = yaw_alloc_and_start(verify_yaw_thread_func, string, sn);
		if (yaw)
		{
			yaw_stop(yaw);
			yaw_wait(yaw);
			sn[2] = (uintptr_t) refer_save_number(yaw);
			refer_free(yaw);
		}
		refer_free(string);
	}
	return (sn[0] == 2 && sn[1] == 2 && sn[2] == 1)?0:-1;
}

static void verify_yaw_not_wait_thread_func(yaw_s *restrict yaw)
{
	yaw_signal_s *signal;
	signal = (yaw_signal_s *) yaw->pri;
	while (yaw->running)
		yaw_msleep(50);
	yaw_signal_set(signal, 1);
	yaw_signal_wake(signal, ~0);
}

int verify_yaw_not_wait(void)
{
	yaw_signal_s *signal;
	yaw_s *yaw;
	yaw = NULL;
	signal = yaw_signal_alloc();
	if (signal)
	{
		yaw_signal_set(signal, 0);
		yaw = yaw_alloc_and_start(verify_yaw_not_wait_thread_func, signal, NULL);
		if (yaw)
		{
			yaw_stop(yaw);
			refer_free(yaw);
			yaw_signal_wait(signal, 0);
		}
		refer_free(signal);
	}
	return (signal && yaw)?0:-1;
}

static void verify_yaw_detach_thread_func(yaw_s *restrict yaw)
{
	yaw_signal_s *signal;
	signal = (yaw_signal_s *) yaw->pri;
	if (yaw_detach_self(yaw))
		*(uintptr_t *) yaw->data = 1;
	while (yaw->running)
		yaw_msleep(50);
	yaw_signal_set(signal, 1);
	yaw_signal_wake(signal, ~0);
}

int verify_yaw_detach(void)
{
	yaw_signal_s *signal;
	yaw_s *yaw;
	uintptr_t detach_okay;
	yaw = NULL;
	detach_okay = 0;
	signal = yaw_signal_alloc();
	if (signal)
	{
		yaw_signal_set(signal, 0);
		yaw = yaw_alloc_and_start(verify_yaw_detach_thread_func, signal, &detach_okay);
		if (yaw)
		{
			yaw_stop(yaw);
			refer_free(yaw);
			yaw_signal_wait(signal, 0);
		}
		refer_free(signal);
	}
	return (signal && yaw && detach_okay)?0:-1;
}

static void verify_proxy(uintptr_t *restrict fail_number, const char *restrict verfiy_name, int (*verfiy_func)(void))
{
	#define c_green "\e[32m"
	#define c_red   "\e[31m"
	#define c_0     "\e[0m"
	printf("\t%s ...\n", verfiy_name);
	if (!verfiy_func())
	{
		// okay
		printf("[" c_green "okay" c_0 "]\t%s\n", verfiy_name);
	}
	else
	{
		// fail
		*fail_number += 1;
		printf("[" c_red "fail" c_0 "]\t%s\n", verfiy_name);
	}
}

int main(void)
{
	uintptr_t fail_number;
	fail_number = 0;
	#define v(_n)  verify_proxy(&fail_number, #_n, verify_##_n)
	v(yaw);
	v(yaw_not_wait);
	v(yaw_detach);
	#undef v
	return fail_number?-1:0;
}
