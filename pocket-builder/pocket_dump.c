#include "pocket_dump.h"
#include <stdio.h>

static void pocket_dump_header(const pocket_header_t *restrict header)
{
	static const char *restrict none = "";
	#define s(k)  (header->k.string?header->k.string:none)
	printf("magic:    %s\n", (const char *) header);
	printf("verify:   %s\n", s(verify));
	printf("tag:      %s\n", s(tag));
	printf("version:  %s\n", s(version));
	printf("author:   %s\n", s(author));
	printf("time:     %s\n", s(time));
	printf("desc:     %s\n", s(description));
	printf("flag:     %s\n", s(flag));
	#undef s
}

static void pocket_dump_desc(const pocket_s *restrict pocket, const pocket_header_t *restrict header)
{
	const pocket_attr_t *restrict attr;
	if (header->description.string && (attr = pocket_system(pocket)))
	{
		attr = pocket_find_tag(pocket, attr, header->description.string, pocket_tag$text, NULL);
		if (attr) printf("[description]\n%s\n", (const char *) attr->data.ptr);
	}
}

static void pocket_dump_tree(const pocket_s *restrict pocket, const pocket_attr_t *restrict attr, uint32_t n)
{
	++n;
	printf("%*c%-48s | %10lu | %s\n",
		n << 1, 0,
		attr->name.string?attr->name.string:"",
		attr->size,
		attr->tag.string?attr->tag.string:"@null"
	);
	if (pocket_preset_tag(pocket, attr) == pocket_tag$index)
	{
		const pocket_attr_t *restrict c;
		uint64_t i;
		c = (const pocket_attr_t *) attr->data.ptr;
		for (i = 0; i < attr->size; ++i)
			pocket_dump_tree(pocket, c + i, n);
	}
}

void pocket_dump(const pocket_s *restrict pocket, dump_bits_t bits)
{
	const pocket_header_t *restrict header;
	const pocket_attr_t *restrict attr;
	header = pocket_header(pocket);
	if (bits & dump_bits_system)
		pocket_dump_header(header);
	if (bits & dump_bits_desc)
		pocket_dump_desc(pocket, header);
	if ((bits & dump_bits_system) && (attr = pocket_system(pocket)))
	{
		printf("[system]\n");
		pocket_dump_tree(pocket, attr, 0);
	}
	if ((bits & dump_bits_user) && (attr = pocket_user(pocket)))
	{
		printf("[user]\n");
		pocket_dump_tree(pocket, attr, 0);
	}
}

