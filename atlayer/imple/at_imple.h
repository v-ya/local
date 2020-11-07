#ifndef _at_imple_h_
#define _at_imple_h_

#include "../at_layer.h"
#include <stddef.h>

enum at_op_base_type_e {
	at_op$none,        // 0: 空实现，空语句作用域
	at_op$var,         // 0: 语句参数，主要用作 node-run 时节点返回变量 (寄存器、内存、常数)
	at_op$define,      // 0: 作用域参数定义，中间节点，在最终翻译运行前应失效
	at_op$scope,       // 1: 作用域，snode: [0 实际作用域]
	at_op$scope$start, // 0: 作用域开始
	at_op$scope$end,   // 0: 作用域结束
	at_op$barrier,     // 0: 屏障，编译器层
	at_op$fence,       // 0: 栏栅，处理器层
	at_op$load,        // 0: 加载变量，一般返回 at_op$var 节点，可以外部干预选择加载左/右变量
	at_op$store,       // 2: 储存变量，snode: [0 左值, 1 右值]
	at_op$label,       // 0: 标签，中间节点
	at_op$goto,        // 0: 无条件跳转
	at_op$branch,      // 3: 分支语句，snode: [0 条件, 1 真作用域, 2 假作用域]
	at_op$switch,      // 2+ 批量分支语句，snode: [0 条件, 1 线性作用域, ... 分支条件->goto label]
	at_op$call,        // 0+ 函数调用，snode: [... 参数列表]
	at_op$return,      // 1: 函数返回，snode: [0 返回值]
	at_op$calc$0,      // 0: 通用 0 元操作符，可能中间节点，snode: [... 操作数]
	at_op$calc$1,      // 1: 通用 1 元操作符，可能中间节点，snode: [... 操作数]
	at_op$calc$2,      // 2: 通用 2 元操作符，可能中间节点，snode: [... 操作数]
	at_op$calc$n,      // 3+ 通用多元操作符，可能中间节点，snode: [... 操作数]
	at_op_max          // 预定义操作类型数目
};

at_layer_t* at_layer_op_set_imple(at_layer_t *restrict layer);

#endif
