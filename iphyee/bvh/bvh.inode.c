#include "bvh.h"

void iphyee_bvh_inode_initial(iphyee_bvh_inode_t *restrict inode)
{
	inode->parent = ~(uint32_t) 0;
	inode->next = ~(uint32_t) 0;
	inode->child = ~(uint32_t) 0;
	inode->nleaf = 0;
	inode->layer = 0;
	inode->tri3index = ~(uint32_t) 0;
}
