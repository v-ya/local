#ifndef _posky_h_
#define _posky_h_

#include <refer.h>
#include <queue/queue.h>

// address
// bit  0-31: only id
// bit 32-63: 0 => group, other => adorable version
#define posky_address_none  0

typedef struct posky_s posky_s;
typedef struct posky_adorable_s posky_adorable_s;
typedef struct posky_candy_s posky_candy_s;

typedef const posky_candy_s* (*posky_feeding_f)(refer_t lady, const posky_candy_s *restrict candy);

struct posky_candy_s {
	uint64_t target;
	uint64_t report;
	uint32_t type;
	uint32_t level;
	uint64_t stamp;
};

posky_s* posky_alloc(queue_s *we, size_t adorable_size);
posky_s* posky_alloc_ring(size_t adorable_size, size_t queue_size);

void posky_set_monster_filter(posky_s *restrict posky, posky_feeding_f feeding, refer_t monster);
void posky_set_monster_none(posky_s *restrict posky, posky_feeding_f feeding, refer_t monster);
void posky_set_monster_fail(posky_s *restrict posky, posky_feeding_f feeding, refer_t monster);
void posky_set_adorable_monster_filter(posky_s *restrict posky, posky_feeding_f feeding, refer_t monster);
void posky_set_adorable_monster_fail(posky_s *restrict posky, posky_feeding_f feeding, refer_t monster);

// candy => we
const posky_candy_s* posky_feeding(posky_s *restrict posky, const posky_candy_s *restrict candy);
// we => candy
const posky_candy_s* posky_catch_gift(posky_s *posky);
// candy =route> me
const posky_candy_s* posky_feeding_adorable(posky_s *restrict posky, const posky_candy_s *restrict candy);
// loop (we => candy =route> me)
void posky_clear_gift(posky_s *restrict posky);

// delete all lonely adorable (refer number <= 1)
void posky_clear_lonely_adorable(posky_s *restrict posky);

uint64_t posky_create_group(posky_s *restrict posky, const char *restrict name);
void posky_delete_group(posky_s *restrict posky, const char *restrict name, uint64_t address);
uint64_t posky_address_group(posky_s *restrict posky, const char *restrict name);
const char* posky_name_group(posky_s *restrict posky, uint64_t address);

posky_s* posky_join_group(posky_s *restrict posky, const char *restrict group_name, uint64_t group_address, const char *restrict adorable_name, uint64_t adorable_address);
void posky_leave_group(posky_s *restrict posky, const char *restrict group_name, uint64_t group_address, const char *restrict adorable_name, uint64_t adorable_address);
posky_adorable_s* posky_exist_group(posky_s *restrict posky, const char *restrict group_name, uint64_t group_address, const char *restrict adorable_name, uint64_t adorable_address);

// posky_adorable_s* need refer_free
posky_adorable_s* posky_create_adorable(posky_s *restrict posky, const char *restrict name, queue_s *me, refer_t lady);
posky_adorable_s* posky_create_adorable_ring(posky_s *restrict posky, const char *restrict name, refer_t lady, size_t queue_size);

void posky_delete_adorable(posky_s *restrict posky, const char *restrict name, uint64_t address);
posky_adorable_s* posky_find_adorable(posky_s *restrict posky, const char *restrict name, uint64_t address);
uint64_t posky_address_adorable(posky_s *restrict posky, const char *restrict name);
const char* posky_name_adorable(posky_s *restrict posky, uint64_t address);

uint64_t posky_adorable_address(posky_adorable_s *restrict adorable);
posky_adorable_s* posky_adorable_set_type_way(posky_adorable_s *restrict adorable, uint32_t type, posky_feeding_f feeding);
void posky_adorable_set_type_finally(posky_adorable_s *restrict adorable, posky_feeding_f feeding);
void posky_adorable_set_monster_filter(posky_adorable_s *restrict adorable, posky_feeding_f feeding, refer_t monster);
void posky_adorable_set_monster_fail(posky_adorable_s *restrict adorable, posky_feeding_f feeding, refer_t monster);

// candy => we
const posky_candy_s* posky_adorable_feeding(posky_adorable_s *restrict adorable, const posky_candy_s *restrict candy);
// candy => me
const posky_candy_s* posky_adorable_surprise(posky_adorable_s *adorable, const posky_candy_s *restrict candy);
// me => candy
const posky_candy_s* posky_adorable_catch_gift(posky_adorable_s *adorable);
// candy => do
const posky_candy_s* posky_adorable_open_gift(posky_adorable_s *restrict adorable, const posky_candy_s *restrict candy);
// loop (me => candy => do)
void posky_adorable_clear_gift(posky_adorable_s *restrict adorable);

#endif
