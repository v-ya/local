#include "random.h"
#include <dylink.h>
#include <memory.h>

typedef struct random_sequence_method_s {
	uint8_t *data;
	size_t size;
	void *get;
	void *set;
} random_sequence_method_s;

typedef struct random_sequence_s {
	random_s random;
	random_sequence_method_s *method;
	uint32_t mask;
	uint32_t index;
	uint32_t seq[];
} random_sequence_s;

static random_sequence_method_s* random32_build_func_x86_64(register random_sequence_method_s *restrict r, register const char *restrict method)
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
		0x31, 0xf6,
		//-00000002  31F6              xor esi,esi
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
	register uint8_t *restrict data;
	register size_t size;
	register uint32_t n;
	register uint8_t c;
	data = r->data;
	size = r->size;
	r->get = data;
	r->set = data + 2;
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
		switch ((c = *method++))
		{
			case '~':
				// 00000000  F7D0              not eax
				size += 2;
				data -= 4;
				*data++ = 0xf7;
				*data++ = 0xd0;
				break;
			case '*':
			case '^':
			case '&':
			case '|':
			case '+':
			case '-':
				// 00000004  F7248F            mul dword [rdi+rcx*4]
				// 00000004  33048F            xor eax,[rdi+rcx*4]
				// 00000004  23048F            and eax,[rdi+rcx*4]
				// 00000004  0B048F            or eax,[rdi+rcx*4]
				// 00000004  03048F            add eax,[rdi+rcx*4]
				// 00000004  2B048F            sub eax,[rdi+rcx*4]
				if (size < 3) goto label_fail;
				size -= 3;
				data[1] = 0x04;
				data[2] = 0x8f;
				switch (c)
				{
					case '*': data[0] = 0xf7; data[1] = 0x24; break;
					case '^': data[0] = 0x33; break;
					case '&': data[0] = 0x23; break;
					case '|': data[0] = 0x0b; break;
					case '+': data[0] = 0x03; break;
					case '-': data[0] = 0x2b; break;
				}
				data += 3;
				break;
			case '<':
			case '>':
				if (c == '>') c = 0x08;
				else c = 0x00;
				if (*method >= '0' && *method <= '9')
				{
					n = (uint32_t)(*method++ - '0');
					if (*method >= '0' && *method <= '9')
						n = n * 10 + (uint32_t)(*method++ - '0');
					if ((n &= 31))
					{
						// 00000000  C1C0--            rol eax,byte 0x--
						// 00000000  C1C8--            ror eax,byte 0x--
						size += 1;
						data -= 4;
						*data++ = 0xc1;
						*data++ = 0xc0 | c;
						*data++ = n;
					}
				}
				else
				{
					// 00000000  89CA              mov edx,ecx
					// 00000002  8B0C8F            mov ecx,[rdi+rcx*4]
					// 00000005  83E11f            and ecx,byte +0x1f
					// 00000008  D3C0              rol eax,cl
					// 00000008  D3C8              ror eax,cl
					// 0000000A  89D1              mov ecx,edx
					if (size < 12) goto label_fail;
					size -= 12;
					*data++ = 0x89;
					*data++ = 0xca;
					*data++ = 0x8b;
					*data++ = 0x0c;
					*data++ = 0x8f;
					*data++ = 0x83;
					*data++ = 0xe1;
					*data++ = 0x1f;
					*data++ = 0xd3;
					*data++ = 0xc0 | c;
					*data++ = 0x89;
					*data++ = 0xd1;
				}
				break;
			default:
				goto label_fail;
		}
	}
	memcpy(data, rcode_tail_x86_64, sizeof(rcode_tail_x86_64));
	return r;
	label_fail:
	return NULL;
}

static random_sequence_method_s* random64_build_func_x86_64(register random_sequence_method_s *restrict r, register const char *restrict method)
{
	#define offset_index  (offsetof(random_sequence_s, index))
	#define offset_mask   (offsetof(random_sequence_s, mask))
	#define offset_seq    (offsetof(random_sequence_s, seq))
	static const uint8_t rcode_head_x86_64[] = {
		// rdi random_s * => seq
		// rsi seed       => mask
		// ecx -          => index
		// r8  -          => seed
		// r9d -          => index++ save
		0x31, 0xf6,
		//-00000002  31F6              xor esi,esi
		0x49, 0x89, 0xf0,
		// 00000000  4989F0            mov r8,rsi
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
		0x48, 0x8b, 0x04, 0xcf,
		// 0000001A  488B04CF          mov rax,[rdi+rcx*8]
	};
	#undef offset_index
	#undef offset_mask
	#undef offset_seq
	static const uint8_t rcode_tail_x86_64[] = {
		// rdi seq
		// esi mask
		// ecx -
		// r8  seed
		// r9d index
		0x4c, 0x31, 0xc0,
		// 00000000  4C31C0            xor rax,r8
		0x44, 0x21, 0xce,
		// 00000003  4421CE            and esi,r9d
		0x48, 0x89, 0x04, 0xf7,
		// 00000006  488904F7          mov [rdi+rsi*8],rax
		0xc3,
		// 0000000A  C3                ret
	};
	register uint8_t *restrict data;
	register size_t size;
	register uint32_t n;
	register uint8_t c;
	data = r->data;
	size = r->size;
	r->get = data;
	r->set = data + 2;
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
		switch ((c = *method++))
		{
			case '~':
				// 00000000  48F7D0            not rax
				size += 1;
				data -= 4;
				*data++ = 0x48;
				*data++ = 0xf7;
				*data++ = 0xd0;
				break;
			case '*':
			case '^':
			case '&':
			case '|':
			case '+':
			case '-':
				// 00000004  48F724CF          mul qword [rdi+rcx*8]
				// 00000004  483304CF          xor rax,[rdi+rcx*8]
				// 00000004  482304CF          and rax,[rdi+rcx*8]
				// 00000004  480B04CF          or rax,[rdi+rcx*8]
				// 00000004  480304CF          add rax,[rdi+rcx*8]
				// 00000004  482B04CF          sub rax,[rdi+rcx*8]
				if (size < 4) goto label_fail;
				size -= 4;
				data[0] = 0x48;
				data[2] = 0x04;
				data[3] = 0xcf;
				switch (c)
				{
					case '*': data[1] = 0xf7; data[2] = 0x24; break;
					case '^': data[1] = 0x33; break;
					case '&': data[1] = 0x23; break;
					case '|': data[1] = 0x0b; break;
					case '+': data[1] = 0x03; break;
					case '-': data[1] = 0x2b; break;
				}
				data += 4;
				break;
			case '<':
			case '>':
				if (c == '>') c = 0x08;
				else c = 0x00;
				if (*method >= '0' && *method <= '9')
				{
					n = (uint32_t)(*method++ - '0');
					if (*method >= '0' && *method <= '9')
						n = n * 10 + (uint32_t)(*method++ - '0');
					if ((n &= 63))
					{
						// 00000000  48C1C0--          rol rax,byte 0x--
						// 00000000  48C1C8--          ror rax,byte 0x--
						data -= 4;
						*data++ = 0x48;
						*data++ = 0xc1;
						*data++ = 0xc0 | c;
						*data++ = n;
					}
				}
				else
				{
					// 00000000  89CA              mov edx,ecx
					// 00000002  488B0CCF          mov rcx,[rdi+rcx*8]
					// 00000006  83E13F            and ecx,byte +0x3f
					// 00000009  48D3C0            rol rax,cl
					// 00000009  48D3C8            ror rax,cl
					// 0000000c  89D1              mov ecx,edx
					if (size < 14) goto label_fail;
					size -= 14;
					*data++ = 0x89;
					*data++ = 0xca;
					*data++ = 0x48;
					*data++ = 0x8b;
					*data++ = 0x0c;
					*data++ = 0xcf;
					*data++ = 0x83;
					*data++ = 0xe1;
					*data++ = 0x3f;
					*data++ = 0x48;
					*data++ = 0xd3;
					*data++ = 0xc0 | c;
					*data++ = 0x89;
					*data++ = 0xd1;
				}
				break;
			default:
				goto label_fail;
		}
	}
	memcpy(data, rcode_tail_x86_64, sizeof(rcode_tail_x86_64));
	return r;
	label_fail:
	return NULL;
}

static void random_sequence_method_free_func(register random_sequence_method_s *restrict r)
{
	if (r->data) xmem_free(r->data, r->size);
}

static random_sequence_method_s* random_sequence_method_alloc(const char *restrict method, size_t size, uint32_t bits)
{
	register random_sequence_method_s *restrict r;
	r = (random_sequence_method_s *) refer_alloz(sizeof(random_sequence_method_s));
	if (r)
	{
		refer_set_free(r, (refer_free_f) random_sequence_method_free_func);
		r->data = (uint8_t *) xmem_alloc(r->size = size);
		switch (bits)
		{
			case 32:
				if (random32_build_func_x86_64(r, method))
					return r;
				break;
			case 64:
				if (random64_build_func_x86_64(r, method))
					return r;
				break;
		}
		refer_free(r);
	}
	return NULL;
}

static void random_sequence_free_func(register random_sequence_s *restrict r)
{
	if (r->method) refer_free(r->method);
}

static void random32_sequence_init_func(register random_sequence_s *restrict r, register uint32_t seed)
{
	register uint32_t n;
	n = r->mask + 1;
	r->index = 0;
	memset(r->seq, 0, sizeof(uint32_t) * n);
	do {
		r->random.r32.set(&r->random, seed);
		seed = (seed >> 1) | (seed << 31);
	} while (--n);
}

static void random64_sequence_init_func(register random_sequence_s *restrict r, register uint64_t seed)
{
	register uint32_t n;
	n = r->mask + 1;
	r->index = 0;
	memset(r->seq, 0, sizeof(uint64_t) * n);
	do {
		r->random.r64.set(&r->random, seed);
		seed = (seed >> 1) | (seed << 63);
	} while (--n);
}

static random_sequence_s* random_sequence_dump(register random_sequence_s *restrict r, uint32_t bits)
{
	register random_sequence_s *restrict rr;
	register size_t n;
	n = sizeof(random_sequence_s) + (bits >> 3) * (r->mask + 1);
	rr = (random_sequence_s *) refer_alloc(n);
	if (rr)
	{
		memcpy(rr, r, n);
		refer_set_free(rr, (refer_free_f) random_sequence_free_func);
		refer_save(rr->method);
		return rr;
	}
	return NULL;
}

static random_sequence_s* random32_sequence_dump_func(register random_sequence_s *restrict r)
{
	if (refer_get_free(r) == (refer_free_f) random_sequence_free_func)
		return random_sequence_dump(r, 32);
	return NULL;
}

static random_sequence_s* random64_sequence_dump_func(register random_sequence_s *restrict r)
{
	if (refer_get_free(r) == (refer_free_f) random_sequence_free_func)
		return random_sequence_dump(r, 64);
	return NULL;
}

random_s* random32_alloc_sequence(uint32_t level, const char *restrict method)
{
	if (level < 16 && method)
	{
		register random_sequence_s *restrict r;
		level = 1u << level;
		r = (random_sequence_s *) refer_alloz(sizeof(random_sequence_s) + sizeof(uint32_t) * level);
		if (r)
		{
			refer_set_free(r, (refer_free_f) random_sequence_free_func);
			r->method = random_sequence_method_alloc(method, 4096, 32);
			if (r->method)
			{
				r->mask = level - 1;
				r->random.r32.get = (random32_get_f) r->method->get;
				r->random.r32.set = (random32_set_f) r->method->set;
				r->random.r32.init = (random32_init_f) random32_sequence_init_func;
				r->random.r32.dump = (random_dump_f) random32_sequence_dump_func;
				return &r->random;
			}
			refer_free(r);
		}
	}
	return NULL;
}

random_s* random64_alloc_sequence(uint32_t level, const char *restrict method)
{
	if (level < 16 && method)
	{
		register random_sequence_s *restrict r;
		level = 1u << level;
		r = (random_sequence_s *) refer_alloz(sizeof(random_sequence_s) + sizeof(uint64_t) * level);
		if (r)
		{
			refer_set_free(r, (refer_free_f) random_sequence_free_func);
			r->method = random_sequence_method_alloc(method, 4096, 64);
			if (r->method)
			{
				r->mask = level - 1;
				r->random.r64.get = (random64_get_f) r->method->get;
				r->random.r64.set = (random64_set_f) r->method->set;
				r->random.r64.init = (random64_init_f) random64_sequence_init_func;
				r->random.r64.dump = (random_dump_f) random64_sequence_dump_func;
				return &r->random;
			}
			refer_free(r);
		}
	}
	return NULL;
}
