.globl _miko_std_spec_data_
.data
.type _miko_std_spec_data_, @object
.align 16
	_miko_std_spec_data_:
	.incbin "miko.std/miko.std.spec"
	_miko_std_spec_tail_:
.size _miko_std_spec_data_, .-_miko_std_spec_data_

.globl _miko_std_spec_size_
.data
.type _miko_std_spec_size_, @object
.size _miko_std_spec_size_, 4
.align 16
	_miko_std_spec_size_:
	.long _miko_std_spec_tail_ - _miko_std_spec_data_
