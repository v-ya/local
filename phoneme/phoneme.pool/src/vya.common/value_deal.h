#ifndef _vya_common_value_deal_h_
#define _vya_common_value_deal_h_

#include <refer.h>
typedef double (*value_deal_f)(double v, refer_t pri);
#define value_deal_func(_symbol, _pri)  double _symbol(register double value, _pri pri)

#endif
