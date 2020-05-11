#ifndef _wavelike_h_
#define _wavelike_h_

#include <stdint.h>

double wavelike_next_zpos(double v[], uint32_t frames, double last);
double wavelike_sample(double v[], uint32_t frames, double p);
double wavelike_loadness(double v[], uint32_t frames, double s, double t);
double wavelike_likely(double v[], uint32_t frames, double s1, double t1, double a1, double s2, double t2, double a2);
double wavelike_next(double v[], uint32_t frames, double last, double *t, double *unlike);
double wavelike_first(double v[], uint32_t frames, double *pos, double *t, double *unlike);

#endif
