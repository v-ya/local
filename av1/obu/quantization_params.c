#include "quantization_params.h"
#include "obu_sequence_header.h"

static inline av1_bits_reader_t* av1_bits_delta_q_read(av1_bits_reader_t *restrict r, int32_t *restrict value)
{
	int64_t q;
	uint8_t delta_coded;
	// (1) delta_coded
	if (!av1_bits_flag_read(r, &delta_coded))
		goto label_fail;
	if (delta_coded)
	{
		// (su(7)) delta_q
		if (!av1_bits_int_read(r, &q, 7))
			goto label_fail;
		*value = (int32_t) q;
	}
	else *value = 0;
	return r;
	label_fail:
	return NULL;
}

static inline av1_bits_writer_t* av1_bits_delta_q_write(av1_bits_writer_t *restrict w, int32_t value)
{
	uint8_t delta_coded;
	// (1) delta_coded
	if (!av1_bits_flag_write(w, delta_coded = !!value))
		goto label_fail;
	if (delta_coded)
	{
		// (su(7)) delta_q
		if (!av1_bits_int_write(w, value, 7))
			goto label_fail;
	}
	return w;
	label_fail:
	return NULL;
}

static inline uint64_t av1_bits_delta_q_bits(int32_t value)
{
	return value?8:1;
}

av1_quantization_params_t* av1_quantization_params_init(av1_quantization_params_t *restrict qparam)
{
	return (av1_quantization_params_t *) memset(qparam, 0, sizeof(*qparam));
}

av1_quantization_params_t* av1_quantization_params_read(av1_quantization_params_t *restrict qparam, av1_bits_reader_t *restrict reader, const struct av1_obu_sequence_header_t *restrict sh)
{
	uint64_t v;
	// (8) base_q_idx
	if (!av1_bits_uint_read(reader, &v, 8))
		goto label_fail;
	qparam->base_q_idx = (uint8_t) v;
	// DeltaQYDc = read_delta_q()
	if (!av1_bits_delta_q_read(reader, &qparam->delta_q_ydc))
		goto label_fail;
	if (av1_obu_sequence_header_NumPlanes(sh) > 1)
	{
		if (sh->color_config_separate_uv_delta_q)
		{
			// (1) diff_uv_delta
			if (!av1_bits_flag_read(reader, &qparam->diff_uv_delta))
				goto label_fail;
		}
		else qparam->diff_uv_delta = 0;
		// DeltaQUDc = read_delta_q()
		// DeltaQUAc = read_delta_q()
		if (!av1_bits_delta_q_read(reader, &qparam->delta_q_udc))
			goto label_fail;
		if (!av1_bits_delta_q_read(reader, &qparam->delta_q_uac))
			goto label_fail;
		if (sh->color_config_separate_uv_delta_q)
		{
			// DeltaQVDc = read_delta_q()
			// DeltaQVAc = read_delta_q()
			if (!av1_bits_delta_q_read(reader, &qparam->delta_q_vdc))
				goto label_fail;
			if (!av1_bits_delta_q_read(reader, &qparam->delta_q_vac))
				goto label_fail;
		}
		else
		{
			qparam->delta_q_vdc = qparam->delta_q_udc;
			qparam->delta_q_vac = qparam->delta_q_uac;
		}
	}
	else
	{
		qparam->delta_q_udc = 0;
		qparam->delta_q_uac = 0;
		qparam->delta_q_vdc = 0;
		qparam->delta_q_vac = 0;
	}
	// (1) using_qmatrix
	if (!av1_bits_flag_read(reader, &qparam->using_qmatrix))
		goto label_fail;
	if (qparam->using_qmatrix)
	{
		// (4) qm_y
		// (4) qm_u
		if (!av1_bits_uint_read(reader, &v, 8))
			goto label_fail;
		qparam->qm_y = (uint32_t) (v >> 4);
		qparam->qm_u = (uint32_t) (v & 0x0f);
		if (sh->color_config_separate_uv_delta_q)
		{
			// (4) qm_v
			if (!av1_bits_uint_read(reader, &v, 4))
				goto label_fail;
			qparam->qm_v = (uint32_t) v;
		}
		else qparam->qm_v = qparam->qm_u;
	}
	return qparam;
	label_fail:
	return NULL;
}

const av1_quantization_params_t* av1_quantization_params_write(const av1_quantization_params_t *restrict qparam, av1_bits_writer_t *restrict writer, const struct av1_obu_sequence_header_t *restrict sh)
{
	// (8) base_q_idx
	if (!av1_bits_uint_write(writer, qparam->base_q_idx, 8))
		goto label_fail;
	// DeltaQYDc = read_delta_q()
	if (!av1_bits_delta_q_write(writer, qparam->delta_q_ydc))
		goto label_fail;
	if (av1_obu_sequence_header_NumPlanes(sh) > 1)
	{
		if (sh->color_config_separate_uv_delta_q)
		{
			// (1) diff_uv_delta
			if (!av1_bits_flag_write(writer, qparam->diff_uv_delta))
				goto label_fail;
		}
		// DeltaQUDc = read_delta_q()
		// DeltaQUAc = read_delta_q()
		if (!av1_bits_delta_q_write(writer, qparam->delta_q_udc))
			goto label_fail;
		if (!av1_bits_delta_q_write(writer, qparam->delta_q_uac))
			goto label_fail;
		if (sh->color_config_separate_uv_delta_q)
		{
			// DeltaQVDc = read_delta_q()
			// DeltaQVAc = read_delta_q()
			if (!av1_bits_delta_q_write(writer, qparam->delta_q_vdc))
				goto label_fail;
			if (!av1_bits_delta_q_write(writer, qparam->delta_q_vac))
				goto label_fail;
		}
	}
	// (1) using_qmatrix
	if (!av1_bits_flag_write(writer, qparam->using_qmatrix))
		goto label_fail;
	if (qparam->using_qmatrix)
	{
		// (4) qm_y
		// (4) qm_u
		if (!av1_bits_uint_write(writer,
			((uint64_t) qparam->qm_y << 4) |
			((uint64_t) qparam->qm_u),
			8))
			goto label_fail;
		if (sh->color_config_separate_uv_delta_q)
		{
			// (4) qm_v
			if (!av1_bits_uint_write(writer, qparam->qm_v, 4))
				goto label_fail;
		}
	}
	return qparam;
	label_fail:
	return NULL;
}

uint64_t av1_quantization_params_bits(const av1_quantization_params_t *restrict qparam, const struct av1_obu_sequence_header_t *restrict sh)
{
	uint64_t size = 0;
	// (8) base_q_idx
	size += 8;
	// DeltaQYDc = read_delta_q()
	size += av1_bits_delta_q_bits(qparam->delta_q_ydc);
	if (av1_obu_sequence_header_NumPlanes(sh) > 1)
	{
		if (sh->color_config_separate_uv_delta_q)
		{
			// (1) diff_uv_delta
			size += 1;
		}
		// DeltaQUDc = read_delta_q()
		// DeltaQUAc = read_delta_q()
		size += av1_bits_delta_q_bits(qparam->delta_q_udc);
		size += av1_bits_delta_q_bits(qparam->delta_q_uac);
		if (sh->color_config_separate_uv_delta_q)
		{
			// DeltaQVDc = read_delta_q()
			// DeltaQVAc = read_delta_q()
			size += av1_bits_delta_q_bits(qparam->delta_q_vdc);
			size += av1_bits_delta_q_bits(qparam->delta_q_vac);
		}
	}
	// (1) using_qmatrix
	size += 1;
	if (qparam->using_qmatrix)
	{
		// (4) qm_y
		// (4) qm_u
		size += 8;
		if (sh->color_config_separate_uv_delta_q)
		{
			// (4) qm_v
			size += 4;
		}
	}
	return size;
}

void av1_quantization_params_dump(const av1_quantization_params_t *restrict qparam, mlog_s *restrict mlog)
{
	mlog_printf(mlog, "base_q_idx[0, 255]:   %u\n", qparam->base_q_idx);
	mlog_printf(mlog, "diff_uv_delta[0, 1]:  %u\n", qparam->diff_uv_delta);
	mlog_printf(mlog, "using_qmatrix[0, 1]:  %u\n", qparam->using_qmatrix);
	mlog_printf(mlog, "qm_y[0, 15]:          %u\n", qparam->qm_y);
	mlog_printf(mlog, "qm_u[0, 15]:          %u\n", qparam->qm_u);
	mlog_printf(mlog, "qm_v[0, 15]:          %u\n", qparam->qm_v);
	mlog_printf(mlog, "delta_q_ydc[-64, 63]: %d\n", qparam->delta_q_ydc);
	mlog_printf(mlog, "delta_q_udc[-64, 63]: %d\n", qparam->delta_q_udc);
	mlog_printf(mlog, "delta_q_uac[-64, 63]: %d\n", qparam->delta_q_uac);
	mlog_printf(mlog, "delta_q_vdc[-64, 63]: %d\n", qparam->delta_q_vdc);
	mlog_printf(mlog, "delta_q_vac[-64, 63]: %d\n", qparam->delta_q_vac);
}
