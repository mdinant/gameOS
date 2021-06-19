	.file	"kb.c"
	.globl	kbdus
	.data
	.align 32
	.type	kbdus, @object
	.size	kbdus, 128
kbdus:
	.byte	0
	.byte	27
	.byte	49
	.byte	50
	.byte	51
	.byte	52
	.byte	53
	.byte	54
	.byte	55
	.byte	56
	.byte	57
	.byte	48
	.byte	45
	.byte	61
	.byte	8
	.byte	9
	.byte	113
	.byte	119
	.byte	101
	.byte	114
	.byte	116
	.byte	121
	.byte	117
	.byte	105
	.byte	111
	.byte	112
	.byte	91
	.byte	93
	.byte	10
	.byte	0
	.byte	97
	.byte	115
	.byte	100
	.byte	102
	.byte	103
	.byte	104
	.byte	106
	.byte	107
	.byte	108
	.byte	59
	.byte	39
	.byte	96
	.byte	0
	.byte	92
	.byte	122
	.byte	120
	.byte	99
	.byte	118
	.byte	98
	.byte	110
	.byte	109
	.byte	44
	.byte	46
	.byte	47
	.byte	0
	.byte	42
	.byte	0
	.byte	32
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	45
	.byte	0
	.byte	0
	.byte	0
	.byte	43
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.zero	38
	.text
	.globl	keyboard_handler
	.type	keyboard_handler, @function
keyboard_handler:
	subl	$28, %esp
	subl	$12, %esp
	pushl	$96
	call	_inportb_nasm
	addl	$16, %esp
	movb	%al, 15(%esp)
	movzbl	15(%esp), %eax
	testb	%al, %al
	js	.L3
	movzbl	15(%esp), %eax
	movzbl	kbdus(%eax), %eax
	movzbl	%al, %eax
	subl	$12, %esp
	pushl	%eax
	call	putch
	addl	$16, %esp
.L3:
	nop
	addl	$28, %esp
	ret
	.size	keyboard_handler, .-keyboard_handler
	.globl	keyboard_install
	.type	keyboard_install, @function
keyboard_install:
	subl	$12, %esp
	subl	$8, %esp
	pushl	$keyboard_handler
	pushl	$1
	call	irq_install_handler
	addl	$16, %esp
	nop
	addl	$12, %esp
	ret
	.size	keyboard_install, .-keyboard_install
	.ident	"GCC: (GNU) 7.2.0"
