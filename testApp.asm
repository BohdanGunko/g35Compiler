.586
.model flat ,stdcall
option casemap:none

include \masm32\include\kernel32.inc
include \masm32\include\msvcrt.inc
includelib \masm32\lib\msvcrt.lib

.data
    tmpLeftOperand dw 0
	tmpRightOperand dw 0
	gPrintFormat db "%s = %hd",13,10,0
	gPrintBeforeScan db "Enter %s = ",0
	gScanFormat db "%hd",0
	pressAnyKeyMsg db "Press any key to exit...",0
	_i1 dw 0
	_i1_gName db "i1",0
	_p1 dw 1
	_p1_gName db "p1",0
	_p2 dw 0
	_p2_gName db "p2",0
	_tm dw 0
	_tm_gName db "tm",0

.code
_tb:
invoke crt_printf, addr gPrintBeforeScan, addr _i1_gName, _i1
invoke crt_scanf, addr gScanFormat, addr _i1
_while_begin_1:
mov bx, _i1
mov cx, bx
mov bx, 23
cmp cx, bx
setl bl
movzx bx, bl
mov cx, bx
cmp cx, 0
je _while_end_1
invoke crt_printf, addr gPrintFormat, addr _p1_gName, _p1
mov bx, _p1
mov cx, bx
mov _tm, cx
mov bx, _p1
mov cx, bx
mov bx, _p2
add bx, cx
mov cx, bx
mov _p1, cx
mov bx, _tm
mov cx, bx
mov _p2, cx
mov bx, _i1
mov cx, bx
mov bx, 1
add bx, cx
mov cx, bx
mov _i1, cx
jmp _while_begin_1
_while_end_1:
invoke crt_printf, addr pressAnyKeyMsg
invoke  crt__getch
ret
end _tb