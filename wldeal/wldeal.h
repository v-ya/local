#ifndef _wldeal_h_
#define _wldeal_h_

#include <stdint.h>
#include <refer.h>
#include <note_details.h>

int console_print(const char *restrict __format, ...) __attribute__((format(printf, 1, 2)));

typedef refer_t (*wavlike_kernal_alloc_f)(int argc, const char *argv[]);
typedef void (*wavlike_kernal_deal_f)(refer_t pri, uint32_t i, note_details_s *nd, double pos, double t, double a, double bf);

#define symbol_kernal_alloc  kernal_alloc
#define symbol_kernal_deal   kernal_deal

#endif
