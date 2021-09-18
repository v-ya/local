#include "ip.h"
#include <stdio.h>
#include <stdlib.h>

uint8_t* transport_inner_ipv4_s2a(const char *restrict s_ipv4, uint8_t a_ipv4[4], uint32_t *restrict port)
{
	a_ipv4[0] = (uint8_t) strtoul(s_ipv4, (char **) &s_ipv4, 10);
	if (*s_ipv4++ != '.')
		goto label_fail;
	a_ipv4[1] = (uint8_t) strtoul(s_ipv4, (char **) &s_ipv4, 10);
	if (*s_ipv4++ != '.')
		goto label_fail;
	a_ipv4[2] = (uint8_t) strtoul(s_ipv4, (char **) &s_ipv4, 10);
	if (*s_ipv4++ != '.')
		goto label_fail;
	a_ipv4[3] = (uint8_t) strtoul(s_ipv4, (char **) &s_ipv4, 10);
	if (!*s_ipv4)
	{
		label_okay:
		if (!port || (*port && *port < transport_inner_port_limit))
			return a_ipv4;
	}
	if (port && *s_ipv4 == ':')
	{
		*port = (uint32_t) strtoul(s_ipv4 + 1, (char **) &s_ipv4, 10);
		if (!*s_ipv4)
			goto label_okay;
	}
	label_fail:
	return NULL;
}

char* transport_inner_ipv4_s4a(char s_ipv4[16], const uint8_t a_ipv4[4])
{
	sprintf(s_ipv4, "%u.%u.%u.%u", a_ipv4[0], a_ipv4[1], a_ipv4[2], a_ipv4[3]);
	return s_ipv4;
}
