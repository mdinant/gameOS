global disable_pic

SECTION .text

disable_pic:
	;cli
	mov al, 0xff
	out 0xa1, al
	out 0x21, al
	ret
