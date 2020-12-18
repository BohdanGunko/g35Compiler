.586
.model flat ,stdcall
option casemap:none

include \masm32\include\kernel32.inc
include \masm32\include\msvcrt.inc
includelib \masm32\lib\msvcrt.lib

.data
    tmpLeftOperand dw 0
	tmpRightOperand dw 0
	gPrintFormat db "%hd",13,10,0
	gScanFormat db "%hd",0
	pressAnyKeyMsg db "Press any key to exit...",0
	_v0 dw 0
	_v1 dw 87
	_re dw ?

.code
_tb:

push cx

mov bx, _v0

mov cx, bx

mov bx, _v1

or bx, cx
cmp bx, 0
setne bl
movzx bx, bl

mov cx, bx

mov bx, cx

mov cx, [esp]
add esp, 2

mov cx, bx

push cx

mov bx, 10

mov cx, bx

mov bx, 2

or bx, cx
cmp bx, 0
setne bl
movzx bx, bl

mov cx, bx

mov bx, cx

mov cx, [esp]
add esp, 2

cmp cx, 0
je _LAND_1
cmp bx, 0
je _LAND_1
mov bx, 1
jmp _LAND_RES_1
_LAND_1:
mov bx, 0
_LAND_RES_1:

mov cx, bx

mov _re, cx

invoke crt_printf, addr gPrintFormat, _re

push cx

mov bx, _v1

mov cx, bx

mov bx, 10

cmp cx, bx

setg bl
movzx bx, bl

mov cx, bx

mov bx, cx

mov cx, [esp]
add esp, 2

mov cx, bx

push cx

mov bx, _v1

mov cx, bx

mov bx, 50

cmp bx, cx
setne bl
movzx bx, bl

mov cx, bx

mov bx, cx

mov cx, [esp]
add esp, 2

or bx, cx
cmp bx, 0
setne bl
movzx bx, bl

mov cx, bx

mov _re, cx

invoke crt_printf, addr gPrintFormat, _re

invoke crt_printf, addr pressAnyKeyMsg
invoke  crt__getch
invoke ExitProcess, 0
end _tb