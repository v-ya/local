#include "tpoll.h"
#include "socket.h"
#include <zlist.h>
#include <unistd.h>

typedef struct transport_inner_poll_item_s {
	zlist_list_t inode;  // n => timestamp_kill
	transport_s *tp;
	transport_poll_do_f func;
	refer_t pri;
} transport_inner_poll_item_s;

struct transport_inner_poll_s {
	zlist_t list;               // (int64_t timestamp_kill) => (transport_poll_item_s *)
	struct epoll_event *events; // epoll_wait() need
	int events_size;
	int epoll;
	uintptr_t size;
	uintptr_t number;
};

static void transport_inner_poll_item_free_func(transport_inner_poll_item_s *restrict r)
{
	if (r->tp) refer_free(r->tp);
	if (r->pri) refer_free(r->pri);
}

static void transport_inner_poll_list_free_func(zlist_list_t *restrict list, void *pri)
{
	refer_free(list);
}

static void transport_inner_poll_free_func(transport_inner_poll_s *restrict r)
{
	if (~r->epoll) close(r->epoll);
	zlist_uini(&r->list, transport_inner_poll_list_free_func, NULL);
}

static transport_inner_poll_item_s* transport_inner_poll_item_alloc(uint64_t timestamp_kill, transport_s *restrict tp, transport_poll_do_f func, refer_t pri)
{
	transport_inner_poll_item_s *restrict r;
	if ((r = (transport_inner_poll_item_s *) refer_alloc(sizeof(transport_inner_poll_item_s))))
	{
		r->inode.n = (int64_t) timestamp_kill;
		r->tp = (transport_s *) refer_save(tp);
		r->func = func;
		r->pri = refer_save(pri);
		refer_set_free(r, (refer_free_f) transport_inner_poll_item_free_func);
	}
	return r;
}

transport_inner_poll_s* transport_inner_poll_alloc(uintptr_t size)
{
	transport_inner_poll_s *restrict r;
	if (size && size <= INT32_MAX &&
		(r = (transport_inner_poll_s *) refer_alloc(sizeof(transport_inner_poll_s) + sizeof(struct epoll_event) * size)))
	{
		r->epoll = ~0;
		if (zlist_init(&r->list, 16))
		{
			refer_set_free(r, (refer_free_f) transport_inner_poll_free_func);
			r->events = (struct epoll_event *) (r + 1);
			r->events_size = (int) size;
			r->epoll = epoll_create(r->events_size);
			r->size = size;
			r->number = 0;
			if (~r->epoll)
				return r;
		}
		refer_free(r);
	}
	return NULL;
}

transport_inner_poll_s* transport_inner_poll_insert(transport_inner_poll_s *restrict tpoll, transport_s *restrict tp, transport_poll_do_f func, refer_t pri, uint64_t timestamp_kill, uint32_t events)
{
	transport_inner_poll_item_s *restrict item;
	struct epoll_event e;
	if (tpoll->number < tpoll->size)
	{
		if (timestamp_kill > (uint64_t) INT64_MAX)
			timestamp_kill = INT64_MAX;
		if ((item = transport_inner_poll_item_alloc(timestamp_kill, tp, func, pri)))
		{
			if (zlist_insert(&tpoll->list, &item->inode, NULL))
			{
				e.events = events;
				e.data.ptr = item;
				if (!epoll_ctl(tpoll->epoll, EPOLL_CTL_ADD, *(transport_inner_socket_t *) tp->p_socket, &e))
				{
					tpoll->number += 1;
					return tpoll;
				}
				zlist_remove(&tpoll->list, &item->inode);
			}
			refer_free(item);
		}
	}
	return NULL;
}

uintptr_t transport_inner_poll_number(transport_inner_poll_s *restrict tpoll)
{
	return tpoll->number;
}

static void transport_inner_poll_remove_item(transport_inner_poll_s *restrict tpoll, transport_inner_poll_item_s *restrict item)
{
	epoll_ctl(tpoll->epoll, EPOLL_CTL_DEL, *(transport_inner_socket_t *) item->tp->p_socket, NULL);
	zlist_remove(&tpoll->list, &item->inode);
	refer_free(item);
	tpoll->number -= 1;
}

uintptr_t transport_inner_poll_get_okay(transport_inner_poll_s *restrict tpoll, uint64_t timeout_ms, refer_t *restrict pri_data, uintptr_t pri_size)
{
	struct epoll_event *events;
	transport_inner_poll_item_s *restrict item;
	uintptr_t i, n;
	int number;
	n = 0;
	if (pri_size > tpoll->size)
		pri_size = tpoll->size;
	number = epoll_wait(tpoll->epoll, events = tpoll->events, (int) pri_size, (int) timeout_ms);
	if (~number)
	{
		pri_size = (uintptr_t) number;
		for (i = 0; i < pri_size; ++i)
		{
			item = (transport_inner_poll_item_s *) events[i].data.ptr;
			if (item->func(item->tp, item->pri) != transport_poll_status_continue)
			{
				pri_data[n++] = refer_save(item->pri);
				transport_inner_poll_remove_item(tpoll, item);
			}
		}
	}
	return n;
}

uintptr_t transport_inner_poll_get_timeout(transport_inner_poll_s *restrict tpoll, uint64_t timestamp_kill, refer_t *restrict pri_data, uintptr_t pri_size)
{
	transport_inner_poll_item_s *restrict item, *restrict next;
	uintptr_t n;
	n = 0;
	if ((item = (transport_inner_poll_item_s *) zlist_find_a(&tpoll->list)))
	{
		while (n < pri_size && (uint64_t) item->inode.n <= timestamp_kill)
		{
			pri_data[n++] = refer_save(item->pri);
			next = (transport_inner_poll_item_s *) item->inode.b;
			transport_inner_poll_remove_item(tpoll, item);
			if (!(item = next)) break;
		}
	}
	return n;
}
