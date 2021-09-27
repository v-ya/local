#ifndef _yaw_h_
#define _yaw_h_

#include <refer.h>

typedef struct yaw_s yaw_s;
typedef struct yaw_lock_s yaw_lock_s;
typedef struct yaw_signal_s yaw_signal_s;

typedef void (*yaw_do_f)(yaw_s *restrict yaw);
typedef void (*yaw_lock_do_f)(yaw_lock_s *ylock);
typedef yaw_lock_s* (*yaw_lock_try_f)(yaw_lock_s *ylock);
typedef yaw_lock_s* (*yaw_lock_wait_f)(yaw_lock_s *ylock, uintptr_t usec);
typedef uint32_t (*yaw_signal_set_f)(yaw_signal_s *ys, uint32_t status);
typedef uint32_t (*yaw_signal_get_f)(yaw_signal_s *ys);
typedef yaw_signal_s* (*yaw_signal_wait_f)(yaw_signal_s *ys, uint32_t status);
typedef uint32_t (*yaw_signal_wake_f)(yaw_signal_s *ys, uint32_t number);
typedef yaw_signal_s* (*yaw_signal_wait_time_f)(yaw_signal_s *ys, uint32_t status, uintptr_t usec);
typedef uint32_t (*yaw_signal_sink_number_f)(yaw_signal_s *ys);

struct yaw_s {
	volatile uintptr_t running;
	yaw_do_f func;
	refer_t pri;
	void *data;
};

struct yaw_lock_s {
	yaw_lock_do_f lock;
	yaw_lock_do_f unlock;
	yaw_lock_try_f trylock;
	yaw_lock_wait_f waitlock;
};

struct yaw_signal_s {
	yaw_signal_set_f set;
	yaw_signal_get_f get;
	yaw_signal_wait_f wait;
	yaw_signal_wake_f wake;
	yaw_signal_wait_time_f wait_time;
	yaw_signal_sink_number_f sink_number;
};

yaw_s* yaw_alloc(yaw_do_f func, refer_t pri);
yaw_s* yaw_start(yaw_s *restrict yaw, const void *data);
yaw_s* yaw_alloc_and_start(yaw_do_f func, refer_t pri, const void *data);
void yaw_stop(yaw_s *restrict yaw);
yaw_s* yaw_wait(yaw_s *restrict yaw);
yaw_s* yaw_stop_and_wait(yaw_s *restrict yaw);

yaw_s* yaw_detach_self(yaw_s *restrict yaw);

void yaw_sleep(uintptr_t sec);
void yaw_msleep(uintptr_t msec);
void yaw_usleep(uintptr_t usec);
void yaw_nsleep(uintptr_t nsec);

yaw_lock_s* yaw_lock_alloc_mutex(void);
yaw_lock_s* yaw_lock_alloc_spin(void);
int yaw_lock_alloc_rwlock(yaw_lock_s *restrict *restrict read, yaw_lock_s *restrict *restrict write);

#define yaw_lock_lock(_yl)       (_yl)->lock(_yl)
#define yaw_lock_unlock(_yl)     (_yl)->unlock(_yl)
#define yaw_lock_try(_yl)        (_yl)->trylock(_yl)
#define yaw_lock_wait(_yl, _us)  (_yl)->waitlock(_yl, _us)

yaw_signal_s* yaw_signal_alloc(void);

#define yaw_signal_set(_ys, _s)             (_ys)->set(_ys, _s)
#define yaw_signal_get(_ys)                 (_ys)->get(_ys)
#define yaw_signal_wait(_ys, _s)            (_ys)->wait(_ys, _s)
#define yaw_signal_wake(_ys, _n)            (_ys)->wake(_ys, _n)
#define yaw_signal_wait_time(_ys, _s, _us)  (_ys)->wait_time(_ys, _s, _us)
#define yaw_signal_sink_number(_ys)         (_ys)->sink_number(_ys)

#endif
