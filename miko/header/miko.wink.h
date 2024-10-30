#ifndef _miko_wink_h_
#define _miko_wink_h_

#include <refer.h>
#include <mlog.h>

typedef const void* miko_wink_t;
typedef struct miko_wink_gomi_s miko_wink_gomi_s;
typedef struct miko_wink_search_s miko_wink_view_s;
typedef struct miko_wink_report_t miko_wink_report_t;
typedef struct miko_wink_report_s miko_wink_report_s;

typedef void (*miko_wink_init_f)(void *restrict wink);
typedef void (*miko_wink_free_f)(void *restrict wink);
typedef miko_wink_view_s* (*miko_wink_view_f)(miko_wink_view_s *restrict view, miko_wink_t wink);
typedef void (*miko_wink_report_f)(miko_wink_report_s *restrict r, miko_wink_report_t *restrict report);

struct miko_wink_report_t {
	uint64_t timestamp_msec_start;
	uint64_t timestamp_msec_stop;
	uintptr_t former_root_inode;
	uintptr_t former_lost_inode;
	uintptr_t latter_lost_inode;
	uintptr_t latter_free_inode;
	uintptr_t layer_done_count;
};

struct miko_wink_report_s {
	miko_wink_report_f report;
};

// create miko.wink.gomi
miko_wink_gomi_s* miko_wink_gomi_alloc(void);

// call and wait miko.wink.gomi done gomi, okay return report
miko_wink_gomi_s* miko_wink_gomi_call_gomi(miko_wink_gomi_s *restrict gomi, miko_wink_report_t *restrict report);
// call and wait miko.wink.gomi set miko_gomi_msec
miko_wink_gomi_s* miko_wink_gomi_call_cycle(miko_wink_gomi_s *restrict gomi, uintptr_t miko_gomi_msec);
// call and wait miko.wink.gomi set report
miko_wink_gomi_s* miko_wink_gomi_call_report(miko_wink_gomi_s *restrict gomi, miko_wink_report_s *restrict report);

// set miko.wink.gomi used default report
miko_wink_gomi_s* miko_wink_gomi_default_report(miko_wink_gomi_s *restrict gomi, mlog_s *restrict mlog, uintptr_t only_modify_output);

miko_wink_view_s* miko_wink_view_add(miko_wink_view_s *restrict r, miko_wink_t wink);
miko_wink_view_s* miko_wink_view_add_array(miko_wink_view_s *restrict r, miko_wink_t *restrict wink_array, uintptr_t wink_count);

miko_wink_t miko_wink_alloc(miko_wink_gomi_s *restrict gomi, uintptr_t size, miko_wink_init_f init, miko_wink_view_f view, miko_wink_free_f free);
miko_wink_t miko_wink_alloz(miko_wink_gomi_s *restrict gomi, uintptr_t size, miko_wink_init_f init, miko_wink_view_f view, miko_wink_free_f free);
miko_wink_t miko_wink_ref(miko_wink_t wink);
miko_wink_t miko_wink_unref(miko_wink_t wink);
miko_wink_t miko_wink_used(miko_wink_t wink);
void miko_wink_rlock_lock(miko_wink_t wink);
void miko_wink_rlock_unlock(miko_wink_t wink);
void miko_wink_wlock_lock(miko_wink_t wink);
void miko_wink_wlock_unlock(miko_wink_t wink);

// struct demo1 {
// 	miko_wink_t wink1;
// 	miko_wink_t wink2;
// };
// 
// struct demo1* demo1_alloc(miko_wink_t wink1, miko_wink_t wink2)
// {
// 	struct demo1 *r;
// 	if ((r = (struct demo1 *) malloc(sizeof(struct demo1))))
// 	{
// 		r->wink1 = miko_wink_ref(wink1);
// 		r->wink2 = miko_wink_ref(wink2);
// 		return r;
// 	}
// 	return NULL;
// }
// 
// void demo1_free(struct demo1 *restrict r)
// {
// 	if (r->wink1) miko_wink_unref(r->wink1);
// 	if (r->wink2) miko_wink_unref(r->wink2);
// }
// 
// struct demo2 {
// 	miko_wink_t wink1;
// 	miko_wink_t wink2;
// };
// 
// static miko_wink_view_s* demo2_view_func(miko_wink_view_s *restrict view, struct demo2 *restrict r)
// {
// 	if (miko_wink_view_add(view, r->wink1) &&
// 		miko_wink_view_add(view, r->wink2))
// 		return view;
// 	return NULL;
// }
// 
// static void demo2_init_func(struct demo2 *restrict r)
// {
// 	r->wink1 = NULL;
// 	r->wink2 = NULL;
// }
// 
// static void demo2_free_func(struct demo2 *restrict r)
// {
// }
// 
// struct demo2* demo2_alloc(miko_wink_gomi_s *restrict gomi, miko_wink_t wink1, miko_wink_t wink2)
// {
// 	struct demo2 *r;
// 	if ((r = (struct demo2 *) miko_wink_alloc(
// 		gomi, sizeof(struct demo2),
// 		(miko_wink_init_f) demo2_init_func,
// 		(miko_wink_view_f) demo2_view_func,
// 		(miko_wink_free_f) demo2_free_func)))
// 	{
// 		// this is okay
// 		miko_wink_used(r->wink1 = wink1);
// 		miko_wink_used(r->wink2 = wink2);
// 		// this is not okay
// 		// r->wink1 = miko_wink_used(wink1);
// 		// r->wink2 = miko_wink_used(wink2);
// 		return r;
// 	}
// 	return NULL;
// }

// ep. A -link> x, B -unlink> x
// (user) push x to search
// (gomi) gomi okay
// (gomi) new gomi start
// (gomi) searched A or unsearch A
// (user) A -link> x
// (user) B -unlink> x
// (gomi) search B
// (gomi) free x
// must confirm free before search x
// (gomi) searched A or unsearch A
// (user) A -link> x
// (gomi) search okay (searched B)
// (gomi) free lost (B -link> x)
// (user) push x to search
// (user) B -unlink> x
// (gomi) will next to search

#endif
