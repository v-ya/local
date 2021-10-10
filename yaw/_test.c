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

static void verify_yaw_locker_proxy(yaw_do_f func1, yaw_do_f func2, yaw_lock_s *locker1, yaw_lock_s *locker2, volatile uintptr_t *status)
{
	yaw_s *y1, *y2;
	y1 = yaw_alloc(func1, locker1);
	y2 = yaw_alloc(func2, locker2);
	if (y1 && y2 &&
		yaw_start(y1, (void *) status) &&
		yaw_start(y2, (void *) status))
	{
		yaw_wait(y1);
		yaw_wait(y2);
		refer_free(y1);
		refer_free(y2);
		return ;
	}
	if (y1) refer_free(y1);
	if (y2) refer_free(y2);
}

static void verify_yaw_locker_mutex_thread1_func(yaw_s *restrict yaw)
{
	volatile uintptr_t *restrict status;
	yaw_lock_s *mutex;
	status = (volatile uintptr_t *) yaw->data;
	mutex = (yaw_lock_s *) yaw->pri;
	yaw_lock_lock(mutex);
	yaw_msleep(200);
	yaw_lock_unlock(mutex);
	yaw_msleep(200);
	if (!yaw_lock_try(mutex))
	{
		if (yaw_lock_wait(mutex, 200000))
		{
			yaw_lock_unlock(mutex);
			*status += 1;
		}
	}
	else yaw_lock_unlock(mutex);
}

static void verify_yaw_locker_mutex_thread2_func(yaw_s *restrict yaw)
{
	volatile uintptr_t *restrict status;
	yaw_lock_s *mutex;
	status = (volatile uintptr_t *) yaw->data;
	mutex = (yaw_lock_s *) yaw->pri;
	yaw_msleep(100);
	if (!yaw_lock_try(mutex))
	{
		if (yaw_lock_wait(mutex, 200000))
		{
			yaw_lock_unlock(mutex);
			*status += 1;
		}
	}
	else yaw_lock_unlock(mutex);
	yaw_lock_lock(mutex);
	yaw_msleep(200);
	yaw_lock_unlock(mutex);
}

int verify_yaw_locker_mutex(void)
{
	yaw_lock_s *mutex;
	volatile uintptr_t status;
	status = 0;
	if ((mutex = yaw_lock_alloc_mutex()))
	{
		verify_yaw_locker_proxy(
			verify_yaw_locker_mutex_thread1_func,
			verify_yaw_locker_mutex_thread2_func,
			mutex, mutex, &status
		);
		refer_free(mutex);
	}
	return (status == 2)?0:-1;
}

static void verify_yaw_locker_spin_thread_func(yaw_s *restrict yaw)
{
	volatile uintptr_t *restrict status;
	yaw_lock_s *spin;
	status = (volatile uintptr_t *) yaw->data;
	spin = (yaw_lock_s *) yaw->pri;
	for (uintptr_t i = 0; i < 1000; ++i)
	{
		yaw_lock_lock(spin);
		*status += 1;
		yaw_lock_unlock(spin);
	}
}

int verify_yaw_locker_spin(void)
{
	yaw_lock_s *spin;
	volatile uintptr_t status;
	status = 0;
	if ((spin = yaw_lock_alloc_spin()))
	{
		verify_yaw_locker_proxy(
			verify_yaw_locker_spin_thread_func,
			verify_yaw_locker_spin_thread_func,
			spin, spin, &status
		);
		refer_free(spin);
	}
	return (status == 2000)?0:-1;
}

typedef struct verify_yaw_locker_rwlock_param_s {
	yaw_signal_s *signal;
	yaw_lock_s *r;
	yaw_lock_s *w;
	volatile uintptr_t join_res;
	volatile uintptr_t uni;
	volatile uintptr_t p;
} verify_yaw_locker_rwlock_param_s;

static void verify_yaw_locker_rwlock_param_free(verify_yaw_locker_rwlock_param_s *restrict r)
{
	if (r->signal)
		refer_free(r->signal);
	if (r->r)
		refer_free(r->r);
	if (r->w)
		refer_free(r->w);
}

static verify_yaw_locker_rwlock_param_s* verify_yaw_locker_rwlock_param_alloc(uintptr_t tn)
{
	verify_yaw_locker_rwlock_param_s *restrict r;
	if ((r = (verify_yaw_locker_rwlock_param_s *) refer_alloz(sizeof(verify_yaw_locker_rwlock_param_s))))
	{
		refer_set_free(r, (refer_free_f) verify_yaw_locker_rwlock_param_free);
		if ((r->signal = yaw_signal_alloc()) &&
			!yaw_lock_alloc_rwlock(&r->r, &r->w))
		{
			r->join_res = tn;
			return r;
		}
		refer_free(r);
	}
	return NULL;
}

static void verify_yaw_locker_rwlock_load(uintptr_t *restrict pdata, uintptr_t uni, uintptr_t p, uintptr_t *restrict p_last)
{
	*pdata += uni - *p_last * (*p_last - 1);
	*p_last = p;
}

static void verify_yaw_locker_rwlock_store(volatile uintptr_t *restrict uni, volatile uintptr_t *restrict p)
{
	*uni += *p * 2;
	*p += 1;
}

static void verify_yaw_locker_rwlock_thread_func(yaw_s *restrict yaw)
{
	verify_yaw_locker_rwlock_param_s *restrict param;
	uintptr_t *restrict pdata;
	uintptr_t i, j, p;
	param = (verify_yaw_locker_rwlock_param_s *) yaw->pri;
	pdata = (uintptr_t *) yaw->data;
	p = 0;
	// 1000 ++
	for (j = 0; j < 10; ++j)
	{
		for (i = 0; i < 100; ++i)
		{
			yaw_lock_lock(param->r);
			verify_yaw_locker_rwlock_load(pdata, param->uni, param->p, &p);
			yaw_lock_unlock(param->r);
			yaw_lock_lock(param->w);
			verify_yaw_locker_rwlock_store(&param->uni, &param->p);
			yaw_lock_unlock(param->w);
		}
		for (i = 0; i < 100; ++i)
		{
			yaw_lock_lock(param->r);
			verify_yaw_locker_rwlock_load(pdata, param->uni, param->p, &p);
			yaw_lock_unlock(param->r);
		}
	}
	if (__sync_sub_and_fetch(&param->join_res, 1))
		yaw_signal_wait(param->signal, 0);
	{
		yaw_signal_set(param->signal, 1);
		yaw_signal_wake(param->signal, ~0);
	}
	verify_yaw_locker_rwlock_load(pdata, param->uni, param->p, &p);
}

int verify_yaw_locker_rwlock(void)
{
	yaw_s *ys[16];
	uintptr_t pdata[sizeof(ys) / sizeof(*ys)];
	verify_yaw_locker_rwlock_param_s *param;
	uintptr_t i, n, N;
	int ret;
	ret = -1;
	if ((param = verify_yaw_locker_rwlock_param_alloc(n = sizeof(ys) / sizeof(*ys))))
	{
		for (i = 0; i < n; ++i)
			ys[i] = NULL, pdata[i] = 0;
		for (i = 0; i < n; ++i)
		{
			if (!(ys[i] = yaw_alloc(verify_yaw_locker_rwlock_thread_func, param)))
				goto label_fail;
		}
		for (i = 0; i < n; ++i)
		{
			if (!(ys[i] = yaw_start(ys[i], pdata + i)))
				goto label_fail;
		}
		yaw_signal_wait(param->signal, 0);
		for (i = 0; i < n; ++i)
			yaw_wait(ys[i]);
		for (i = 0; i < n; ++i)
		{
			if (pdata[i] != param->uni)
				goto label_fail;
		}
		N = n * 1000;
		N = N * (N - 1);
		if (param->uni != N)
			goto label_fail;
		ret = 0;
		label_fail:
		for (i = 0; i < n; ++i)
		{
			if (ys[i])
				refer_free(ys[i]);
		}
		refer_free(param);
	}
	return ret;
}


static void verify_yaw_signal_thread_func(yaw_s *restrict yaw)
{
	volatile uintptr_t *restrict N;
	yaw_signal_s *signal;
	N = (volatile uintptr_t *) yaw->data;
	signal = (yaw_signal_s *) yaw->pri;
	yaw_signal_wait(signal, 0);
	if (yaw_signal_get(signal) == 1)
		__sync_fetch_and_add(N, 1);
	yaw_signal_wait(signal, 1);
	if (yaw_signal_get(signal) == 2)
		__sync_fetch_and_add(N, 1);
	yaw_signal_wait(signal, 2);
	if (yaw_signal_get(signal) == 3)
		__sync_fetch_and_add(N, 1);
}

int verify_yaw_signal(void)
{
	yaw_s *ys[8];
	yaw_signal_s *signal;
	uintptr_t i, n, N;
	int ret;
	ret = -1;
	if ((signal = yaw_signal_alloc()))
	{
		N = 0;
		n = sizeof(ys) / sizeof(*ys);
		for (i = 0; i < n; ++i)
			ys[i] = NULL;
		for (i = 0; i < n; ++i)
		{
			if (!(ys[i] = yaw_alloc(verify_yaw_signal_thread_func, signal)))
				goto label_fail;
		}
		yaw_signal_set(signal, 0);
		for (i = 0; i < n; ++i)
		{
			if (!(ys[i] = yaw_start(ys[i], &N)))
				goto label_fail;
		}
		yaw_msleep(200);
		yaw_signal_set(signal, 1);
		yaw_signal_wake(signal, 3);
		yaw_msleep(200);
		yaw_signal_set(signal, 2);
		yaw_signal_wake(signal, 4);
		yaw_msleep(200);
		yaw_signal_set(signal, 3);
		yaw_signal_wake(signal, ~0);
		for (i = 0; i < n; ++i)
			yaw_wait(ys[i]);
		if (N != (3 + 4 + n))
			goto label_fail;
		ret = 0;
		label_fail:
		for (i = 0; i < n; ++i)
		{
			if (ys[i])
				refer_free(ys[i]);
		}
		refer_free(signal);
	}
	return ret;
}

static void verify_proxy(uintptr_t *restrict fail_number, const char *restrict verify_name, int (*verify_func)(void))
{
	#define c_green "\e[32m"
	#define c_red   "\e[31m"
	#define c_0     "\e[0m"
	printf("\t%s ...\n", verify_name);
	if (!verify_func())
	{
		// okay
		printf("[" c_green "okay" c_0 "]\t%s\n", verify_name);
	}
	else
	{
		// fail
		*fail_number += 1;
		printf("[" c_red "fail" c_0 "]\t%s\n", verify_name);
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
	v(yaw_locker_mutex);
	v(yaw_locker_spin);
	v(yaw_locker_rwlock);
	v(yaw_signal);
	#undef v
	return fail_number?-1:0;
}
