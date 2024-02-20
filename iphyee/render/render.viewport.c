#define _DEFAULT_SOURCE
#include "render.h"
#include <math.h>

iphyee_render_viewport_s* iphyee_render_viewport_alloc(void)
{
	return (iphyee_render_viewport_s *) refer_alloz(sizeof(iphyee_render_viewport_s));
}

void iphyee_render_viewport_set_position(iphyee_render_viewport_s *restrict r, float x, float y, float z)
{
	r->position.v[0] = x;
	r->position.v[1] = y;
	r->position.v[2] = z;
	r->position.v[3] = 1.0f;
}

void iphyee_render_viewport_set_vector_x(iphyee_render_viewport_s *restrict r, float x, float y, float z)
{
	iphyee_vec4_t v;
	v.v[0] = x;
	v.v[1] = y;
	v.v[2] = z;
	v.v[3] = 1.0f;
	iphyee_vec4_nvec(&r->vector_x, &v);
}

void iphyee_render_viewport_set_vector_y(iphyee_render_viewport_s *restrict r, float x, float y, float z)
{
	iphyee_vec4_t v;
	v.v[0] = x;
	v.v[1] = y;
	v.v[2] = z;
	v.v[3] = 1.0f;
	iphyee_vec4_nvec(&r->vector_y, &v);
}

void iphyee_render_viewport_set_view_k(iphyee_render_viewport_s *restrict r, float kx, float ky, float kz)
{
	r->view_k_x = kx;
	r->view_k_y = ky;
	r->view_k_z = kz;
}

void iphyee_render_viewport_set_depth(iphyee_render_viewport_s *restrict r, float view_depth)
{
	r->view_depth = view_depth;
}

void iphyee_render_viewport_set_screen(iphyee_render_viewport_s *restrict r, uint32_t screen_width, uint32_t screen_height)
{
	r->screen_width = (float) screen_width;
	r->screen_height = (float) screen_height;
	r->screen_side = (float) sqrt((double) screen_width * (double) screen_height);
}

iphyee_mat4x4_t* iphyee_render_viewport_get_transform(iphyee_render_viewport_s *restrict r)
{
	iphyee_mat4x4_t tf0t, tf0x, tf0y, tfvk, tfvp, tfvt;
	iphyee_mat4x4_t ma, mb;
	iphyee_vec4_t t, qx;
	iphyee_mat4x4_set_tr(&tf0t, -r->position.v[0], -r->position.v[1], -r->position.v[2]);
	iphyee_vec4_cpy(&t, &r->vector_x); t.v[0] += 1;
	iphyee_vec4_nvec(&t, &t);
	iphyee_vec4_quat(&qx, &t, (float) M_PI);
	iphyee_mat4x4_set_quat(&tf0x, &qx);
	iphyee_mat4x4_vec4(&t, &tf0x, &r->vector_y);
	iphyee_mat4x4_set_rx(&tf0y, -atan2f(t.v[2], t.v[1]));
	iphyee_mat4x4_set_kk(&tfvk, r->view_k_x * r->screen_side, r->view_k_y * r->screen_side, r->view_k_z);
	iphyee_mat4x4_set_e(&tfvp);
	tfvp.m[2][2] = 1;
	tfvp.m[2][3] = -r->view_depth;
	tfvp.m[3][2] = 1;
	tfvp.m[3][3] = 0;
	iphyee_mat4x4_set_tr(&tfvt, (float) r->screen_width / 2, (float) r->screen_height / 2, 0);
	iphyee_mat4x4_mul(&ma, &tf0x, &tf0t);
	iphyee_mat4x4_mul(&mb, &tf0y, &ma);
	iphyee_mat4x4_mul(&ma, &tfvk, &mb);
	iphyee_mat4x4_mul(&mb, &tfvp, &ma);
	iphyee_mat4x4_mul(&r->transform, &tfvt, &mb);
	return &r->transform;
}
