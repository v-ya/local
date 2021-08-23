#include "ip.h"
#include <stdlib.h>

uint8_t* transport_inner_ipv4_s2a(const char *restrict s_ipv4, uint8_t a_ipv4[4])
{
	a_ipv4[0] = (uint8_t) strtol(s_ipv4, (char **) &s_ipv4, 10);
	if (*s_ipv4++ != '.')
		goto label_fail;
	a_ipv4[1] = (uint8_t) strtol(s_ipv4, (char **) &s_ipv4, 10);
	if (*s_ipv4++ != '.')
		goto label_fail;
	a_ipv4[2] = (uint8_t) strtol(s_ipv4, (char **) &s_ipv4, 10);
	if (*s_ipv4++ != '.')
		goto label_fail;
	a_ipv4[3] = (uint8_t) strtol(s_ipv4, (char **) &s_ipv4, 10);
	if (!*s_ipv4)
		return a_ipv4;
	label_fail:
	return NULL;
}
