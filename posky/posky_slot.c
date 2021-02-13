#include "posky_slot.h"
#include "posky_group.h"
#include <memory.h>
#include <stdlib.h>

uint64_t* posky_slot_array_init(posky_slot_t *restrict array, uint32_t size)
{
	uint64_t *restrict map;
	uint32_t i;
	memset(array, 0, sizeof(posky_slot_t) * size);
	for (i = 0; i < size; ++i)
		array[i].index = i;
	map = (uint64_t *) calloc(1, ((size + 63) >> 6) << 3);
	if (map && (i = size & 63))
		map[size >> 6] = ~(uint64_t) 0 << i;
	return map;
}

void posky_slot_array_uini(posky_slot_t *restrict array, uint32_t size)
{
	uint32_t i;
	for (i = 0; i < size; ++i)
		posky_slot_clear(array + i);
}

static inline posky_slot_t* posky_slot_gen_empty_init(posky_slot_t *restrict slot)
{
	if (!++slot->version) ++slot->version;
	slot->address = ((uint64_t) slot->version << 32) | slot->index;
	return slot;
}

posky_slot_t* posky_slot_gen_empty(posky_slot_t *restrict array, uint64_t *restrict map, uint32_t size)
{
	register uint64_t mask;
	uint32_t i, shift;
	size = (size + 63) >> 6;
	for (i = 0; i < size; ++map, ++i)
	{
		if ((mask = ~*map))
		{
			array += i << 6;
			shift = 0;
			while (!(mask & 0xff))
			{
				mask >>= 8;
				shift += 8;
			}
			while (!(mask & 1))
			{
				mask >>= 1;
				++shift;
			}
			*map |= (uint64_t) 1 << shift;
			return posky_slot_gen_empty_init(array + shift);
		}
	}
	return NULL;
}

void posky_slot_delete(posky_slot_t *restrict array, uint64_t *restrict map, uint32_t size, uint32_t index)
{
	if (index < size)
	{
		posky_slot_clear(array + index);
		map[index >> 6] &= ~((uint64_t) 1 << (index & 63));
	}
}

void posky_slot_clear(posky_slot_t *restrict slot)
{
	slot->address = 0;
	if (slot->adorable)
	{
		refer_free(slot->adorable);
		slot->adorable = NULL;
	}
	if (slot->group)
		posky_group_list_delete_by_slot(slot->group);
}
