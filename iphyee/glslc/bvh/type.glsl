#ifndef _iphyee_glsl_bvh_type_
#define _iphyee_glsl_bvh_type_

#include "../standard/pointer.glsl"

struct iphyee_bvh_pos {
	float x;
	float y;
	float z;
};

struct iphyee_bvh_box {    // 包围盒
	iphyee_bvh_pos p1; // p1 < p2
	iphyee_bvh_pos p2; // p2 > p1
};

struct iphyee_bvh_info { // (同步/遍历/测试) 附加信息
	uint leaf_exist; // (ScTc) 当前节点下方结构性叶节点数量
	int layer_level; // (ScTc) 层次级别 (单一模型顶层节点为 0, 其下每层依次 +1, 模型顶层之上依次 -1 直至根节点)
	uint leaf_count; // (SvTc) 当前节点的叶节点线程计数器
	uint leaf_alive; // (SvTc) 当前节点下方有效的叶节点数量
};

struct iphyee_bvh_data { // 节点数据
	uint64_t target; // (ScTc) 非 0 则为引用模型
	uint tri3index;  // (ScTc) 引用模型的三角形索引
	uint pri_param;  // (ScTv) 对齐用，也可作 (target, tri3index) 的补充参数
};

_def_pointer_ iphyee_bvh_refer;
_def_array_(8) iphyee_bvh_refer { // 包围盒层次结构
	iphyee_bvh_refer parent;  // (ScTc) 指向上层节点
	iphyee_bvh_refer next;    // (ScTc) 指向同层下一节点 (该链表的 parent 均相同)
	iphyee_bvh_refer child;   // (ScTc) 指向下层节点 (指向链表)
	iphyee_bvh_info info;     // (S?Tc) 节点附加信息
	iphyee_bvh_box box;       // (SvTc) 该节点的包围盒 (其下所有节点的包围盒)
	iphyee_bvh_data data;     // (ScTc) 节点引用平面
};

// Sc sync-constant 同步更新阶段固定
// Sv sync-variable 同步更新阶段可变
// Tc tray-constant 命中测试阶段固定
// Tv tray-variable 命中测试阶段可变

// iphyee_bvh_refer this;
// iphyee_bvh_refer next;
// int layer;
// this = root;
// layer = this.layer;
// while (uint64_t(this)) {
//     if (this.layer >= layer && <hit this.box>) {
//         ...
//         next = this.child;
//         if (!uint64_t(next))
//             next = this.next;
//     }
//     else next = this.next;
//     if (!uint64_t(next))
//         next = this.parent;
//     layer = this.layer;
//     this = next;
// }

#endif
