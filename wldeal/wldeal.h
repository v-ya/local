#ifndef _wldeal_h_
#define _wldeal_h_

#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include <refer.h>
#include <note_details.h>
#include <scatterplot.h>

typedef struct wavelike_kernal_arg_t {
	double time;     // 总时长
	double offset;   // 当前类周期波段 起始位置
	double length;   // 当前类周期波段 长度
	double a;        // 当前类周期波段 最大幅值
	double basefre;  // 当前类周期波段 基频
} wavelike_kernal_arg_t;

typedef refer_t (*wavelike_kernal_alloc_f)(int argc, const char *argv[]);
typedef void (*wavelike_kernal_deal_f)(refer_t pri, uint32_t i, note_details_s *nd, wavelike_kernal_arg_t *arg);

#define symbol_kernal_alloc  kernal_alloc
#define symbol_kernal_deal   kernal_deal

#endif
