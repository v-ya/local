#ifndef _random_h_
#define _random_h_

#include <refer.h>

typedef struct random_s random_s;
typedef uint32_t (*random_get_f)(register random_s *restrict r);
typedef uint32_t (*random_set_f)(register random_s *restrict r, uint32_t seed);
typedef void (*random_init_f)(register random_s *restrict r, uint32_t seed);

struct random_s {
	random_get_f get;
	random_set_f set;
	random_init_f init;
};

#define random_get(_r)          ((_r)->get(_r))
#define random_set(_r, _seed)   ((_r)->set(_r, _seed))
#define random_init(_r, _seed)  (_r)->init(_r, _seed)

/// @brief 生成序列化随机数发生器
/// @param level  [0, 16) 序列缓冲长度 = (1u << level)
/// @param method != NULL, 推荐使用 "*>3^~"
///                index   = last + 1
///                  r32   = [last--]
///               ## loop begin ##  build by method
///               ~: r32   = ~r32
///               *: r32  *= [last--]
///               ^: r32  ^= [last--]
///               &: r32  &= [last--]
///               |: r32  |= [last--]
///               +: r32  += [last--]
///               -: r32  -= [last--]
///              <n: r32   = r32 r<< n
///              >n: r32   = r32 r>> n
///               ## loop end ##
///                  r32  ^= seed
///               [index] = r32
/// @return (random_s *)
random_s* random_alloc_sequence(uint32_t level, const char *restrict method);

#endif
