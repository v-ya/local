#ifndef _miko_base_miko_label_fail_h_
#define _miko_base_miko_label_fail_h_

#define miko_label_fail(_status_, _add_, _log_, _value_)  \
	label_fail_##_status_:\
	miko_log_add_key(_log_, miko_log_level__error, miko_status_##_status_, NULL);\
	goto label_fail

#endif
