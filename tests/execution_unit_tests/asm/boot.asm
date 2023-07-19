mov bp, 0x7c00
mov sp, bp

mov bx, string
call prnt

jmp $

print:
  mov ah, 0x0e
  .loop:
  cmp [bx], byte 0
  je .exit
    mov al, [bx]
    int 0x10
    inc bx
    jmp .loop
  .exit:
    ret

string: db 'booting!', 0

times 510 - ($ - $$)db 0
dw 0xAA55
