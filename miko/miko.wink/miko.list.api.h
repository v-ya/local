#ifndef _miko_wink_miko_list_api_h_
#define _miko_wink_miko_list_api_h_

#include "miko.list.h"

// insert
//          loop:
// (expect) *p => next
//          inode.next = next
// (maybe)  *p => new -> next
// (atomic) if (*p == inode.next)
// (atomic)     *p = inode;
//          else goto loop;
// (result) *p => inode -> next

// unlink
//          loop:
// (expect) *p => inode -> next
// (maybe)  *p => new -> inode -> next
// (atomic) if ((pre = *p) == inode)
// (atomic)     *p = inode.next;
//          else {
//              p = &pre.next;
//              goto loop;
//          }
// (result) *p => next

// allow multi call insert
// p must &miko.list
static inline void miko_list_insert(miko_list_t *volatile *p, miko_list_t *restrict inode)
{
	while (!__sync_bool_compare_and_swap(p, inode->next = *p, inode)) ;
}

// must single call unlink
// p must &inode.pre.next
static inline void miko_list_unlink(miko_list_t *volatile *p, miko_list_t *restrict inode)
{
	miko_list_t *restrict pre;
	while ((pre = __sync_val_compare_and_swap(p, inode, inode->next)) != inode)
		p = &pre->next;
}

#endif
