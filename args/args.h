#ifndef _args_h_
#define _args_h_

#include <stddef.h>
#include <hashmap.h>

#define args_deal_func(_n, _t)  int _n(_t pri, int *index, const char *command, const char *value, int argc, const char *argv[])
typedef int (*args_deal_f)(void *pri, int *index, const char *command, const char *value, int argc, const char *argv[]);

hashmap_vlist_t* args_set_command(hashmap_t *a, const char *command, args_deal_f deal_func);
hashmap_vlist_t* args_set_index(hashmap_t *a, int index, args_deal_f deal_func);
int args_deal(int argc, const char *argv[], hashmap_t *a, args_deal_f default_func, void *pri);

#endif
