[bits 16]

[org 0x00]

start:
    mov bx, string      ; Print a welcome string
mov ah, 0x0E
print_loop:
    mov al, byte [bx]
    int 0x10
    inc bx
    cmp byte [bx], 0
    jne print_loop
;end print_loop








string: db "This is the second stage program.", 0