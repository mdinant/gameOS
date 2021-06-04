[BITS 32]

global memcpy_SSE2

memcpy_SSE2:
;	mov esi, src;    //src pointer
;   mov edi, dest;   //dest pointer
;    mov ebx, size;   //ebx is our counter
	push ebp
	mov ebp, esp

 	mov edi, [ebp+8]; dest
 	mov esi, [ebp+12]; src
 	mov ebx, [ebp+16]; size



    shr ebx, 7;      //divide by 128 (8 * 128bit registers)


    loop_copy:
 ;     prefetchnta 128[ESI] //SSE2 prefetch
 ;     prefetchnta 160[ESI]
 ;     prefetchnta 192[ESI]
 ;     prefetchnta 224[ESI]
 		prefetchnta [esi + 128]
  		prefetchnta [esi + 160]
  		 prefetchnta [esi + 192]
  		 prefetchnta [esi + 224]

      movdqa xmm0, [esi]; //move data from src to registers
      movdqa xmm1, [esi + 16]
      movdqa xmm2, [esi + 32]
      movdqa xmm3, [esi + 48]
      movdqa xmm4, [esi + 64]
      movdqa xmm5, [esi + 80]
      movdqa xmm6, [esi + 96]
      movdqa xmm7, [esi + 112]

      movntdq [edi], xmm0; //move data from registers to dest
      movntdq [edi + 16], xmm1
      movntdq [edi + 32], xmm2
      movntdq [edi + 48], xmm3
      movntdq [edi + 64], xmm4
      movntdq [edi + 80], xmm5
      movntdq [edi + 96], xmm6
      movntdq [edi + 112], xmm7

      add esi, 128
      add edi, 128
      dec ebx

      jnz loop_copy; //loop please

		mov esp, ebp
    	pop ebp
      	ret
    loop_copy_end:
