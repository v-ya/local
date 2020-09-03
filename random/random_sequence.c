#include "random.h"
#include <dylink.h>
#include <memory.h>

typedef struct random_sequence_s {
	random_s random;
	uint8_t *data;
	size_t size;
	uint32_t mask;
	uint32_t index;
	uint32_t seq[];
} random_sequence_s;

static random_set_f random_build_func_x86_64(register uint8_t *restrict data, register size_t size, register const char *restrict method)
{
	#define offset_index  (offsetof(random_sequence_s, index))
	#define offset_mask   (offsetof(random_sequence_s, mask))
	#define offset_seq    (offsetof(random_sequence_s, seq))
	static const uint8_t rcode_head_x86_64[] = {
		// rdi random_s * => seq
		// esi seed       => mask
		// ecx -          => index
		// r8d -          => seed
		// r9d -          => index++ save
		0x41, 0x89, 0xf0,
		// 00000000  4189F0            mov r8d,esi
		0x8b, 0x47, offset_index,
		// 00000003  8B47--            mov eax,[rdi+0x--]  <= index
		0x89, 0xc1,
		// 00000006  89C1              mov ecx,eax
		0x83, 0xc0, 0x01,
		// 00000008  83C001            add eax,byte +0x1
		0x89, 0x47, offset_index,
		// 0000000B  8947--            mov [rdi+0x--],eax  => index
		0x41, 0x89, 0xc1,
		// 0000000E  4189C1            mov r9d,eax
		0x8b, 0x77, offset_mask,
		// 00000011  8B77--            mov esi,[rdi+0x--]  <= mask
		0x21, 0xf1,
		// 00000014  21F1              and ecx,esi
		0x48, 0x83, 0xc7, offset_seq,
		// 00000016  4883C7--          add rdi,byte +0x--  <= seq
		0x8b, 0x04, 0x8f,
		// 0000001A  8B048F            mov eax,[rdi+rcx*4]
	};
	#undef offset_index
	#undef offset_mask
	#undef offset_seq
	static const uint8_t rcode_tail_x86_64[] = {
		// rdi seq
		// esi mask
		// ecx -
		// r8d seed
		// r9d index
		0x44, 0x31, 0xc0,
		// 00000000  4431C0            xor eax,r8d
		0x44, 0x21, 0xce,
		// 00000003  4421CE            and esi,r9d
		0x89, 0x04, 0xb7,
		// 00000006  8904B7            mov [rdi+rsi*4],eax
		0xc3,
		// 00000009  C3                ret
	};
	random_set_f func;
	register uint32_t n;
	uint8_t ror = 0;
	func = (random_set_f) data;
	if (size < (sizeof(rcode_head_x86_64) + sizeof(rcode_tail_x86_64)))
		goto label_fail;
	size -= sizeof(rcode_head_x86_64) + sizeof(rcode_tail_x86_64);
	memcpy(data, rcode_head_x86_64, sizeof(rcode_head_x86_64));
	data += sizeof(rcode_head_x86_64);
	while (*method)
	{
		// 00000000  01F1              add ecx,esi
		// 00000002  21F1              and ecx,esi
		if (size < 4) goto label_fail;
		size -= 4;
		*data++ = 0x01;
		*data++ = 0xf1;
		*data++ = 0x21;
		*data++ = 0xf1;
		switch (*method++)
		{
			case '~':
				// 00000004  F7D0              not eax
				if (size < 2) goto label_fail;
				size -= 2;
				*data++ = 0xf7;
				*data++ = 0xd0;
				break;
			case '*':
				// 00000004  F7248F            mul dword [rdi+rcx*4]
				if (size < 3) goto label_fail;
				size -= 3;
				*data++ = 0xf7;
				*data++ = 0x24;
				*data++ = 0x8f;
				break;
			case '^':
				// 00000004  33048F            xor eax,[rdi+rcx*4]
				if (size < 3) goto label_fail;
				size -= 3;
				*data++ = 0x33;
				*data++ = 0x04;
				*data++ = 0x8f;
				break;
			case '&':
				// 00000004  23048F            and eax,[rdi+rcx*4]
				if (size < 3) goto label_fail;
				size -= 3;
				*data++ = 0x23;
				*data++ = 0x04;
				*data++ = 0x8f;
				break;
			case '|':
				// 00000004  0B048F            or eax,[rdi+rcx*4]
				if (size < 3) goto label_fail;
				size -= 3;
				*data++ = 0x0b;
				*data++ = 0x04;
				*data++ = 0x8f;
				break;
			case '+':
				// 00000004  03048F            add eax,[rdi+rcx*4]
				if (size < 3) goto label_fail;
				size -= 3;
				*data++ = 0x03;
				*data++ = 0x04;
				*data++ = 0x8f;
				break;
			case '-':
				// 00000004  2B048F            sub eax,[rdi+rcx*4]
				if (size < 3) goto label_fail;
				size -= 3;
				*data++ = 0x2b;
				*data++ = 0x04;
				*data++ = 0x8f;
				break;
			case '>':
				ror = 0x08;
				// fall through
			case '<':
				n = 1;
				if (*method >= '0' && *method <= '9')
				{
					n = (uint32_t)(*method++ - '0');
					if (*method >= '0' && *method <= '9')
						n = n * 10 + (uint32_t)(*method++ - '0');
				}
				if (!(n &= 31)) goto label_clear_break;
				// 00000004  C1C0--            rol eax,byte 0x--
				// 00000004  C1C8--            ror eax,byte 0x--
				if (size < 3) goto label_fail;
				size -= 3;
				*data++ = 0xc1;
				*data++ = 0xc0 | ror;
				*data++ = n;
				label_clear_break:
				ror = 0;
				break;
			default:
				goto label_fail;
		}
	}
	memcpy(data, rcode_tail_x86_64, sizeof(rcode_tail_x86_64));
	return func;
	label_fail:
	return NULL;
}

static void random_sequence_free_func(register random_sequence_s *restrict r)
{
	if (r->data) xmem_free(r->data, r->size);
}

static uint32_t random_sequence_get_func(register random_s *restrict r)
{
	return r->set(r, 0);
}

static void random_sequence_init_func(register random_sequence_s *restrict r, register uint32_t seed)
{
	register uint32_t n;
	n = r->mask + 1;
	memset(r->seq, 0, n);
	do {
		r->random.set(&r->random, seed);
		seed = (seed >> 1) | (seed << 31);
	} while (--n);
}

random_s* random_alloc_sequence(uint32_t level, const char *restrict method)
{
	if (level < 16 && method)
	{
		register random_sequence_s *restrict r;
		level = 1u << level;
		r = (random_sequence_s *) refer_alloz(sizeof(random_sequence_s) + sizeof(uint32_t) * level);
		if (r)
		{
			refer_set_free(r, (refer_free_f) random_sequence_free_func);
			r->data = (uint8_t *) xmem_alloc(r->size = 4096);
			r->mask = level - 1;
			if ((r->random.set = random_build_func_x86_64(r->data, r->size, method)))
			{
				r->random.get = random_sequence_get_func;
				r->random.init = (random_init_f) random_sequence_init_func;
				return &r->random;
			}
			refer_free(r);
		}
	}
	return NULL;
}
