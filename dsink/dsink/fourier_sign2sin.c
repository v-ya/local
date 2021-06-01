#define _DEFAULT_SOURCE
#include "inner.h"
#include <math.h>

/*
预设:
	sign(x) = 1;  if x = [(2*k)*pi, (2*k+1)*pi)
	sign(x) = -1; if x = [(2*k+1)*pi, (2*k+2)*pi)
		周期 T = 2*pi
	sin(x)
		周期 T = 2*pi
	sign(rank, q, x) = sign((2*p*rank/T)*x + q)
	sin(rank, q, x)  = sin((2*p*rank/T)*x + q)

f(rank_sign, rank_sin, q_sign, q_sin, x) = sign(rank, q, x) * sin(rank, q, x);
	其中 rank_sign, rank_sin 均为正整数 (N+)
	S 为 f 在 [0, T) 上的积分

则:
	if (rank_sin / rank_sign) 为奇整数
		S = cos(q_sin - (q_sign * rank_sin / rank_sign)) * (2 * rank_sign * T / (rank_sin * pi));
	else S = 0;

sign(rank_sign, q_sign, x) 在 [0, T) 域内的 fourier 级数展开系数为:
	A = S * 2 / T;
		sin(rank_sin, 0, x)    => Asin
		sin(rank_sin, pi/2, x) => Acos

前 15 阶类似于:
	S: q_sign = 0, C: q_sign = pi/2
	s: q_sin  = 0, c: q_sin  = pi/2
	A => --[rank_sign, rank_sin]:

	Ss[ 1,  1]:  1.273240
	Cc[ 1,  1]:  1.273240

	Ss[ 1,  3]:  0.424413
	Cc[ 1,  3]: -0.424413

	Ss[ 1,  5]:  0.254648
	Cc[ 1,  5]:  0.254648

	Ss[ 1,  7]:  0.181891
	Cc[ 1,  7]: -0.181891

	Ss[ 1,  9]:  0.141471
	Cc[ 1,  9]:  0.141471

	Ss[ 1, 11]:  0.115749
	Cc[ 1, 11]: -0.115749

	Ss[ 1, 13]:  0.097942
	Cc[ 1, 13]:  0.097942

	Ss[ 1, 15]:  0.084883
	Cc[ 1, 15]: -0.084883

	Ss[ 2,  2]:  1.273240
	Cc[ 2,  2]:  1.273240

	Ss[ 2,  6]:  0.424413
	Cc[ 2,  6]: -0.424413

	Ss[ 2, 10]:  0.254648
	Cc[ 2, 10]:  0.254648

	Ss[ 2, 14]:  0.181891
	Cc[ 2, 14]: -0.181891

	Ss[ 3,  3]:  1.273240
	Cc[ 3,  3]:  1.273240

	Ss[ 3,  9]:  0.424413
	Cc[ 3,  9]: -0.424413

	Ss[ 3, 15]:  0.254648
	Cc[ 3, 15]:  0.254648

	Ss[ 4,  4]:  1.273240
	Cc[ 4,  4]:  1.273240

	Ss[ 4, 12]:  0.424413
	Cc[ 4, 12]: -0.424413

	Ss[ 5,  5]:  1.273240
	Cc[ 5,  5]:  1.273240

	Ss[ 5, 15]:  0.424413
	Cc[ 5, 15]: -0.424413

	Ss[ 6,  6]:  1.273240
	Cc[ 6,  6]:  1.273240

	Ss[ 7,  7]:  1.273240
	Cc[ 7,  7]:  1.273240

	Ss[ 8,  8]:  1.273240
	Cc[ 8,  8]:  1.273240

	Ss[ 9,  9]:  1.273240
	Cc[ 9,  9]:  1.273240

	Ss[10, 10]:  1.273240
	Cc[10, 10]:  1.273240

	Ss[11, 11]:  1.273240
	Cc[11, 11]:  1.273240

	Ss[12, 12]:  1.273240
	Cc[12, 12]:  1.273240

	Ss[13, 13]:  1.273240
	Cc[13, 13]:  1.273240

	Ss[14, 14]:  1.273240
	Cc[14, 14]:  1.273240

	Ss[15, 15]:  1.273240
	Cc[15, 15]:  1.273240
*/

double dsink_inner_fourier_sign2sin(uint32_t rank_sign, uint32_t rank_sin, double q_sign, double q_sin)
{
	uint32_t k;
	if (!(rank_sin % rank_sign) && ((k = rank_sin / rank_sign) & 1))
		return cos(q_sin - (q_sign * k)) * 4 / (M_PI * k);
	return 0;
}
