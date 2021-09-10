#ifndef _udns_inner_type_inner_
#define _udns_inner_type_inner_

#include <stdint.h>
#include <stddef.h>

uintptr_t udns_inner_unicode4utf8(uint32_t *restrict unicode, uintptr_t unicode_size, const char *restrict utf8, uintptr_t utf8_size);
uintptr_t udns_inner_unicode2utf8(const uint32_t *restrict unicode, uintptr_t unicode_size, char *restrict utf8, uintptr_t utf8_size);

typedef struct udns_inner_punycode_point_t {
	uintptr_t point;
	uintptr_t lmin;
	uintptr_t rmin;
} udns_inner_punycode_point_t;

uintptr_t udns_inner_punycode4unicode(uint8_t *restrict punycode, uintptr_t punycode_size, const uint32_t *unicode, uintptr_t unicode_size, udns_inner_punycode_point_t *restrict unicode_point);
uintptr_t udns_inner_punycode2unicode(const uint8_t *restrict punycode, uintptr_t punycode_size, uint32_t *unicode, uintptr_t unicode_size);

typedef struct udns_inner_labels_t {
	uint8_t data[512];
} udns_inner_labels_t;

udns_inner_labels_t* udns_inner_labels4string(udns_inner_labels_t *restrict r, const char *restrict s, uintptr_t *restrict size);
udns_inner_labels_t* udns_inner_labels2string(udns_inner_labels_t *restrict r, const uint8_t *restrict p, uintptr_t size, uintptr_t *restrict pos, uintptr_t *restrict length);

#endif
