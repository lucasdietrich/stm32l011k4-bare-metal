.global _sinc
.section .text

_sinc:
	push {lr}
	add r0, #1
	pop {pc}

.section .data
