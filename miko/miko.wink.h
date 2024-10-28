#ifndef _miko_wink_h_
#define _miko_wink_h_

#include <refer.h>
#include <mlog.h>

typedef const void* miko_wink_t;
typedef struct miko_wink_gomi_s miko_wink_gomi_s;
typedef struct miko_wink_report_t miko_wink_report_t;
typedef struct miko_wink_report_s miko_wink_report_s;

typedef void (*miko_wink_report_f)(miko_wink_report_s *restrict r, miko_wink_report_t *restrict report);

struct miko_wink_report_t {
	uint64_t timestamp_msec_start;
	uint64_t timestamp_msec_stop;
	uintptr_t former_root_inode;
	uintptr_t former_lost_inode;
	uintptr_t latter_root_inode;
	uintptr_t latter_lost_inode;
	uintptr_t visible_visit_inode;
	uintptr_t visible_wink_inode;
	uintptr_t visible_lost_inode;
	uintptr_t visible_free_inode;
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

// create wink chunk
miko_wink_t miko_wink_alloc(miko_wink_gomi_s *restrict gomi, uintptr_t size);
// create wink chunk, fill zero
miko_wink_t miko_wink_alloz(miko_wink_gomi_s *restrict gomi, uintptr_t size);

// set wink chunk free func
void miko_wink_set_free(miko_wink_t wink, refer_free_f free_func);

// link once parent => child, okay return child, else return NULL
miko_wink_t miko_wink_link(miko_wink_t parent, miko_wink_t child);

// unlink once parent => child
void miko_wink_unlink(miko_wink_t parent, miko_wink_t child);

// set wink chunk lose, if it is none-linked, allow miko.wink.gomi free it
void miko_wink_lost(miko_wink_t wink);

#endif
