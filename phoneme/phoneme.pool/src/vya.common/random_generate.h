#ifndef _vya_common_random_generate_h_
#define _vya_common_random_generate_h_

#include <refer.h>
#include <json.h>

typedef struct random_src_s random_src_s;
typedef double (*random_src_generate_f)(random_src_s *restrict src);
struct random_src_s {
	random_src_generate_f rsrc_generate_func;
	// random src generate pri data
	// ...
};
typedef random_src_s* (*random_src_alloc_f)(json_inode_t *restrict arg);
#define random_src_generate_func(_symbol, _pri)  double _symbol(_pri src)
#define random_src_alloc_func(_symbol, _pri)     _pri _symbol(register json_inode_t *restrict arg)

typedef double (*random_generate_f)(random_src_s *restrict rsrc, double mu, double sigma, refer_t pri);
#define random_generate_func(_symbol, _pri)  double _symbol(register random_src_s *restrict rsrc, register double mu, register double sigma, _pri pri)

#define rsrc_generate(_src)  ((_src)->rsrc_generate_func(_src))

#endif
