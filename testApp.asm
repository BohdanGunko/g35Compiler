.586
.model flat ,stdcall
option casemap:none

include \masm32\include\kernel32.inc
include \masm32\include\msvcrt.inc
includelib \masm32\lib\msvcrt.lib

.data
    tmpLeftOperand dw 0
	tmpRightOperand dw 0
	gPrintFormat db "%d",13,10,0
	gScanFormat db "%d",0
	pressAnyKeyMsg db "Press any key to exit...",0
	_v0 dw 0
	_v1 dw 1
	_re dw ?

.code
_tb:
mov bx, 2
mov cx, bx
mov bx, 1
cmp cx, bx

setg bl
movzx bx, bl

mov cx, bx
mov _re, cx
invoke crt_printf, addr gPrintFormat, _re
mov bx, 1
mov cx, bx
mov bx, 2
cmp cx, bx

setg bl
movzx bx, bl

mov cx, bx
mov _re, cx
invoke crt_printf, addr gPrintFormat, _re
mov bx, 5
mov cx, bx
mov bx, 5
cmp cx, bx

setg bl
movzx bx, bl

mov cx, bx
mov _re, cx
invoke crt_printf, addr gPrintFormat, _re
mov bx, 2
mov cx, bx
mov bx, 1
cmp bx, cx
sete bl
movzx bx, bl

mov cx, bx
mov _re, cx
invoke crt_printf, addr gPrintFormat, _re
mov bx, 1
mov cx, bx
mov bx, 2
cmp bx, cx
sete bl
movzx bx, bl

mov cx, bx
mov _re, cx
invoke crt_printf, addr gPrintFormat, _re
mov bx, 5
mov cx, bx
mov bx, 5
cmp bx, cx
sete bl
movzx bx, bl

mov cx, bx
mov _re, cx
invoke crt_printf, addr gPrintFormat, _re
mov bx, 2
mov cx, bx
mov bx, 1
cmp bx, cx
setne bl
movzx bx, bl

mov cx, bx
mov _re, cx
invoke crt_printf, addr gPrintFormat, _re
mov bx, 1
mov cx, bx
mov bx, 2
cmp bx, cx
setne bl
movzx bx, bl

mov cx, bx
mov _re, cx
invoke crt_printf, addr gPrintFormat, _re
mov bx, 5
mov cx, bx
mov bx, 5
cmp bx, cx
setne bl
movzx bx, bl

mov cx, bx
mov _re, cx
invoke crt_printf, addr gPrintFormat, _re
mov bx, 2
mov cx, bx
mov bx, 1
cmp cx, bx

setl bl
movzx bx, bl

mov cx, bx
mov _re, cx
invoke crt_printf, addr gPrintFormat, _re
mov bx, 1
mov cx, bx
mov bx, 2
cmp cx, bx

setl bl
movzx bx, bl

mov cx, bx
mov _re, cx
invoke crt_printf, addr gPrintFormat, _re
mov bx, 5
mov cx, bx
mov bx, 5
cmp cx, bx

setl bl
movzx bx, bl

mov cx, bx
mov _re, cx
invoke crt_printf, addr gPrintFormat, _re
invoke crt_printf, addr pressAnyKeyMsg
invoke  crt__getch
invoke ExitProcess, 0
end _tb