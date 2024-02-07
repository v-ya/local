#ifndef _iphyee_mat4x4_h_
#define _iphyee_mat4x4_h_

#include <stdint.h>

typedef struct iphyee_mat4x4_t iphyee_mat4x4_t;

struct iphyee_mat4x4_t  {
	float m[4][4];  // m[row][col]
};

void iphyee_mat4x4_mul(iphyee_mat4x4_t *restrict dst, const iphyee_mat4x4_t *restrict src1, const iphyee_mat4x4_t *restrict src2);

void iphyee_mat4x4_set_e(iphyee_mat4x4_t *restrict dst);
void iphyee_mat4x4_fix_tr(iphyee_mat4x4_t *restrict dst, float tx, float ty, float tz);
void iphyee_mat4x4_set_tr(iphyee_mat4x4_t *restrict dst, float tx, float ty, float tz);
void iphyee_mat4x4_fix_tx(iphyee_mat4x4_t *restrict dst, float tx);
void iphyee_mat4x4_set_tx(iphyee_mat4x4_t *restrict dst, float tx);
void iphyee_mat4x4_fix_ty(iphyee_mat4x4_t *restrict dst, float ty);
void iphyee_mat4x4_set_ty(iphyee_mat4x4_t *restrict dst, float ty);
void iphyee_mat4x4_fix_tz(iphyee_mat4x4_t *restrict dst, float tz);
void iphyee_mat4x4_set_tz(iphyee_mat4x4_t *restrict dst, float tz);
void iphyee_mat4x4_fix_rx(iphyee_mat4x4_t *restrict dst, float rx);
void iphyee_mat4x4_set_rx(iphyee_mat4x4_t *restrict dst, float rx);
void iphyee_mat4x4_fix_ry(iphyee_mat4x4_t *restrict dst, float ry);
void iphyee_mat4x4_set_ry(iphyee_mat4x4_t *restrict dst, float ry);
void iphyee_mat4x4_fix_rz(iphyee_mat4x4_t *restrict dst, float rz);
void iphyee_mat4x4_set_rz(iphyee_mat4x4_t *restrict dst, float rz);

#endif
