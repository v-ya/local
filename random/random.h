#ifndef _random_h_
#define _random_h_

#include <refer.h>

typedef union random_s random_s;
typedef struct random32_s random32_s;
typedef struct random64_s random64_s;

typedef uint32_t (*random32_get_f)(register random_s *restrict r);
typedef uint32_t (*random32_set_f)(register random_s *restrict r, uint32_t seed);
typedef void (*random32_init_f)(register random_s *restrict r, uint32_t seed);

typedef uint64_t (*random64_get_f)(register random_s *restrict r);
typedef uint64_t (*random64_set_f)(register random_s *restrict r, uint64_t seed);
typedef void (*random64_init_f)(register random_s *restrict r, uint64_t seed);

typedef random_s* (*random_dump_f)(register random_s *restrict r);

struct random32_s {
	random32_get_f get;
	random32_set_f set;
	random32_init_f init;
	random_dump_f dump;
};

struct random64_s {
	random64_get_f get;
	random64_set_f set;
	random64_init_f init;
	random_dump_f dump;
};

union random_s {
	random32_s r32;
	random64_s r64;
};

#define random32_get(_r)          ((_r)->r32.get(_r))
#define random32_set(_r, _seed)   ((_r)->r32.set(_r, _seed))
#define random32_init(_r, _seed)  (_r)->r32.init(_r, _seed)
#define random32_dump(_r)         ((_r)->r32.dump(_r))

#define random64_get(_r)          ((_r)->r64.get(_r))
#define random64_set(_r, _seed)   ((_r)->r64.set(_r, _seed))
#define random64_init(_r, _seed)  (_r)->r64.init(_r, _seed)
#define random64_dump(_r)         ((_r)->r64.dump(_r))

/// @brief 生成序列化随机数发生器
/// @note  目前只支持 x86_64
/// @param level  [0, 16) 序列缓冲长度 = (1u << level)
/// @param method != NULL, 推荐 "*>^~"
///               时间(s)/(2^30 times)  "*^~" "*>n^~" "*>^~" "*>^<~"
///                                    (2.4 :  2.6  :  2.7  :  3.0  :  4.5(rand())  :  7.6(std::mt19937))
///                index    = last + 1
///                  r32    = [last--]
///               ## loop begin ##  build by method
///               ~: r32    = ~r32
///               *: r32   *= [last--]
///               ^: r32   ^= [last--]
///               &: r32   &= [last--]
///               |: r32   |= [last--]
///               +: r32   += [last--]
///               -: r32   -= [last--]
///               <: r32 r<<= [last--] & 0x31
///               >: r32 r>>= [last--] & 0x31
///              <n: r32 r<<= n
///              >n: r32 r>>= n
///               ## loop end ##
///                  r32   ^= seed
///               [index]   = r32
/// @return (random_s *)
random_s* random32_alloc_sequence(uint32_t level, const char *restrict method);

/// @brief 生成序列化随机数发生器
/// @note  目前只支持 x86_64
/// @param level  [0, 16) 序列缓冲长度 = (1u << level)
/// @param method != NULL, 推荐 "*>^~"
///               时间(s)/(2^30 times)  "*^~" "*>n^~" "*>^~" "*>^<~"
///                                    (2.1 :  2.3  :  2.4  :  2.8  :  4.5(rand())  :  4.8(std::mt19937_64))
///                index    = last + 1
///                  r64    = [last--]
///               ## loop begin ##  build by method
///               ~: r64    = ~r64
///               *: r64   *= [last--]
///               ^: r64   ^= [last--]
///               &: r64   &= [last--]
///               |: r64   |= [last--]
///               +: r64   += [last--]
///               -: r64   -= [last--]
///               <: r64 r<<= [last--] & 0x63
///               >: r64 r>>= [last--] & 0x63
///              <n: r64 r<<= n
///              >n: r64 r>>= n
///               ## loop end ##
///                  r64   ^= seed
///               [index]   = r64
/// @return (random_s *)
random_s* random64_alloc_sequence(uint32_t level, const char *restrict method);

#endif
