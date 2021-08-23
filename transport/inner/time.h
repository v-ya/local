#ifndef _transport_inner_time_h_
#define _transport_inner_time_h_

#include <stdint.h>
#include <time.h>
#include <sys/time.h>

// timespec

struct timespec* transport_inner_timespec_now(struct timespec *restrict r);
struct timespec* transport_inner_timespec_diff(struct timespec *r, const struct timespec *a, const struct timespec *b);
struct timespec* transport_inner_timespec_by_ms(struct timespec *restrict r, uint64_t ms);
struct timespec* transport_inner_timespec_by_us(struct timespec *restrict r, uint64_t us);
uint64_t transport_inner_timespec_to_ms(struct timespec *restrict r);
uint64_t transport_inner_timespec_to_us(struct timespec *restrict r);

// timeval

struct timeval* transport_inner_timeval_by_ms(struct timeval *restrict r, uint64_t ms);
struct timeval* transport_inner_timeval_by_us(struct timeval *restrict r, uint64_t us);
uint64_t transport_inner_timeval_to_ms(struct timeval *restrict r);
uint64_t transport_inner_timeval_to_us(struct timeval *restrict r);

#endif
