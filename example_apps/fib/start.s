/*
 * For this example, we'll have a super basic but self contained program with
 * its own linker script.
 * We won't let the toolchain add it's own stuff by manually invoking the linker
 */


	.text
	.align	2
	
	.globl	_start
    .globl  _exit

_start:
	li	sp, 0x80000000
	li	s0, 0

	call	main
	call    _exit

_exit:
	call    _exit

