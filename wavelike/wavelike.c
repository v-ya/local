#include "wavelike.h"
#include <math.h>

double wavelike_next_zpos(double v[], uint32_t frames, double last)
{
	uint32_t n;
	if (last < 0) n = 0;
	else n = (uint32_t) last + 1;
	if (n < frames)
	{
		if (v[n] > 0)
		{
			++n;
			while (n < frames)
			{
				if (v[n] <= 0)
				{
					return n + v[n] / (v[n - 1] - v[n]);
				}
				++n;
			}
		}
		else if (v[n] < 0)
		{
			++n;
			while (n < frames)
			{
				if (v[n] >= 0)
				{
					return n - v[n] / (v[n] - v[n - 1]);
				}
				++n;
			}
		}
		else return n;
	}
	return frames + 1;
}

double wavelike_sample(double v[], uint32_t frames, double p)
{
	uint32_t i;
	i = (uint32_t) p;
	if (i + 1 < frames)
	{
		p -= i;
		return p * v[i] + (1 - p) * v[i + 1];
	}
	return 0;
}

double wavelike_loadness(double v[], uint32_t frames, double s, double t)
{
	uint32_t i, n;
	i = (uint32_t) s + 1;
	n = (uint32_t) (s + t) + 1;
	s = 0;
	if (n > frames) n = frames;
	while (i < n)
	{
		t = fabs(v[i]);
		if (t > s) s = t;
		++i;
	}
	return s;
}

double wavelike_likely(double v[], uint32_t frames, double s1, double t1, double a1, double s2, double t2, double a2)
{
	double r, c;
	uint32_t i, n;
	i = (uint32_t) s1 + 1;
	n = (uint32_t) (s1 + t1) + 1;
	t2 /= t1;
	s2 -= s1 * t2;
	a2 /= a1;
	a1 *= sqrt((n - i) * a2);
	r = 0;
	if (n < frames)
	{
		while (i < n)
		{
			c = v[i] - wavelike_sample(v, frames, i * t2 + s2) / a2;
			r += c * c;
			++i;
		}
		return sqrt(r) / a1;
	}
	return 1;
}

double wavelike_next(double v[], uint32_t frames, double last, double *t, double *unlike)
{
	double p, q, n1, n2, a1, a2, ul;
	double m_ul, m_t;
	m_ul = 1;
	m_t = 0;
	p = frames + 1;
	if (last + *t < frames)
	{
		p = last + *t;
		n1 = p + *t * 0.8;
		n2 = p + *t * 1.2;
		if (n2 > frames) n2 = frames;
		a1 = wavelike_loadness(v, frames, last, *t);
		q = wavelike_next_zpos(v, frames, n1);
		while (q < n2)
		{
			a2 = wavelike_loadness(v, frames, p, q - p);
			ul = wavelike_likely(v, frames, last, *t, a1, p, q - p, a2);
			if (ul < m_ul)
			{
				m_ul = ul;
				m_t = q - p;
			}
			q = wavelike_next_zpos(v, frames, q);
		}
	}
	if (m_ul > 0.8)
	{
		m_ul = 1;
		m_t = 0;
		p = frames + 1;
	}
	*t = m_t;
	if (unlike) *unlike = m_ul;
	return p;
}

double wavelike_first(double v[], uint32_t frames, double *pos, double *t, double *unlike)
{
	double p, q, n1, n2, tt, ul;
	double m_ul, m_t;
	m_ul = 1;
	m_t = 0;
	p = wavelike_next_zpos(v, frames, pos?*pos:-1);
	if (p < frames)
	{
		n1 = p + *t * 0.5;
		n2 = p + *t * 1.5;
		if (n2 > frames) n2 = frames;
		q = wavelike_next_zpos(v, frames, n1);
		while (q < n2)
		{
			tt = q - p;
			wavelike_next(v, frames, p, &tt, &ul);
			if (ul < m_ul)
			{
				m_ul = ul;
				m_t = q - p;
			}
			q = wavelike_next_zpos(v, frames, q);
		}
	}
	if (m_ul > 0.8)
	{
		m_ul = 1;
		m_t = 0;
		p = frames + 1;
	}
	*t = m_t;
	if (unlike) *unlike = m_ul;
	return p;
}
