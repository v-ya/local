#ifndef _iphyee_glsl_bvh_type_
#define _iphyee_glsl_bvh_type_

#include "../standard/pointer.glsl"

struct iphyee_bvh_pos {
	float x;
	float y;
	float z;
};

struct iphyee_bvh_box {
	iphyee_bvh_pos p1; // p1 < p2
	iphyee_bvh_pos p2; // p2 > p1
};

_def_pointer_ iphyee_bvh_refer;
_def_array_(8) iphyee_bvh_refer { // 包围盒层次结构
	iphyee_bvh_refer parent;  // 指向上层节点
	iphyee_bvh_refer next;    // 指向同层下一节点 (该链表的 parent 均相同)
	iphyee_bvh_refer child;   // 指向下层节点 (指向链表)
	iphyee_bvh_box box;       // 该节点的包围盒 (其下所有节点的包围盒)
	uint64_t target;          // 非 0 则为引用模型
	uint tri3index;           // 引用模型的三角形索引
	int layer;                // 层次级别 (单一模型顶层节点为 0, 其下每层依次 +1, 模型顶层之上依次 -1 直至根节点)
};

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
