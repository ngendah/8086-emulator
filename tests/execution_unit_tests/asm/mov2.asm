org 100h
lea ax, buff
mov ds, ax
mov ax, bx
mov bl, al
mov al, [si]

section .data
buff: times 3 db 0
