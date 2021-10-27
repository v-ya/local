#ifndef _kiya_source_builder_sync_h_
#define _kiya_source_builder_sync_h_

struct pocket_verify_s;

typedef enum sync_status_t {
	sync_status_same,
	sync_status_copy,
	sync_status_delete,
	sync_status_fail
} sync_status_t;

// @return: 0 continue, other interrupt
typedef int (*sync_report_f)(void *pri, sync_status_t status, const char *restrict dst_rpath, const char *restrict src_rpath);

int sync_pocket_ignore_time(const struct pocket_verify_s *restrict verify, const char *restrict dst_dir, const char *restrict src_dir, sync_report_f report_func, void *pri);

#endif
