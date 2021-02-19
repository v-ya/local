.globl dy_core
.data
.type dy_core, @object
.align 16
	dy_core:
	.incbin "core/core.dy"
	dy_core_end:
.size dy_core, .-dy_core

.globl dy_core_size
.data
.type dy_core_size, @object
.size dy_core_size, 4
.align 16
	dy_core_size:
	.long dy_core_end - dy_core

