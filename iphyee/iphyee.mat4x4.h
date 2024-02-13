#ifndef _iphyee_mat4x4_h_
#define _iphyee_mat4x4_h_

#include <stdint.h>

typedef struct iphyee_vec4_t iphyee_vec4_t;
typedef struct iphyee_mat4x4_t iphyee_mat4x4_t;

struct iphyee_vec4_t  {
	float v[4];
};
struct iphyee_mat4x4_t  {
	float m[4][4];  // m[row][col]
};

void iphyee_vec4_cpy(iphyee_vec4_t *restrict dst, const iphyee_vec4_t *restrict src);
void iphyee_vec4_set(iphyee_vec4_t *restrict dst, float x, float y, float z);
void iphyee_vec4_npos(iphyee_vec4_t *npos, const iphyee_vec4_t *pos4);
void iphyee_vec4_nvec(iphyee_vec4_t *nvec, const iphyee_vec4_t *vec4);
float iphyee_vec4_len3(const iphyee_vec4_t *restrict src);
void iphyee_vec4_quat(iphyee_vec4_t *quat, const iphyee_vec4_t *nvec_rot_axis, float ra);
void iphyee_vec4_mul4(iphyee_vec4_t *dst, const iphyee_vec4_t *src1, const iphyee_vec4_t *src2);
float iphyee_vec4_dot3(const iphyee_vec4_t *restrict src1, const iphyee_vec4_t *restrict src2);
void iphyee_vec4_cross(iphyee_vec4_t *restrict dst, const iphyee_vec4_t *restrict src1, const iphyee_vec4_t *restrict src2);

void iphyee_mat4x4_cpy(iphyee_mat4x4_t *restrict dst, const iphyee_mat4x4_t *restrict src);
void iphyee_mat4x4_mul(iphyee_mat4x4_t *restrict dst, const iphyee_mat4x4_t *restrict src1, const iphyee_mat4x4_t *restrict src2);
void iphyee_mat4x4_vec4(iphyee_vec4_t *restrict dst, const iphyee_mat4x4_t *restrict src1, const iphyee_vec4_t *restrict src2);

void iphyee_mat4x4_set_e(iphyee_mat4x4_t *restrict dst);
void iphyee_mat4x4_fix_kk(iphyee_mat4x4_t *restrict dst, float kx, float ky, float kz);
void iphyee_mat4x4_set_kk(iphyee_mat4x4_t *restrict dst, float kx, float ky, float kz);
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
void iphyee_mat4x4_fix_quat(iphyee_mat4x4_t *restrict dst, const iphyee_vec4_t *restrict quat);
void iphyee_mat4x4_set_quat(iphyee_mat4x4_t *restrict dst, const iphyee_vec4_t *restrict quat);

#endif
