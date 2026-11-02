org 7c00h
bits 16

start:
	xor ax, ax
	mov ds, ax

	xor cx, cx
	xor dx, dx
	xor bx, bx

	fninit

	mov ah, 0x00
	mov al, 13h
	int 10h

	mov ah, 0x00
    int 1Ah

    mov word [seed], dx

	call xorshift16

	xor dx, dx
	mov bx, 9
	div bx

	mov [formula], dx

	mov ax, 0xA000
	mov es, ax

main:
	call draw_frame

	inc word [frame]
	
	mov ax, [frame]
	mov bl, 24
	div bl
	mov [frame], ah
	
	mov ah, 0x86
	mov cx, 1
	mov dx, 0
	int 15h
	
	jmp main

draw_frame:
	push dx
	push cx

	xor dx, dx
	xor cx, cx
	xor di, di

	call enum_rows

	pop cx
	pop dx

	ret

enum_rows:
	cmp dx, 200
	jge done

	xor cx, cx
	call process_row

process_row:
	cmp cx, 320
	jge continue_processing

	mov bx, cx
	mov ax, dx

	cmp word [formula], 0
	je formula0

	cmp word [formula], 1
	je formula1

	cmp word [formula], 2
	je formula2

	cmp word [formula], 3
	je formula3

	cmp word [formula], 4
	je formula4

	cmp word [formula], 5
	je formula5

	cmp word [formula], 6
	je formula6

	cmp word [formula], 7
	je formula7

	cmp word [formula], 8
	je formula8

process_row_p2:
	call xorshift16
	and ax, 0xFF
	and al, 3
	
	add al, bl
	xor al, [formulaXor]
	add al, [frame]

	mov bl, 0x18
    div bl
    mov al, ah

	add al, 0x20

	mov [es:di], al
	inc di

	inc cx
	jmp process_row

continue_processing:
	inc dx
	jmp enum_rows

formula0:
	xor bx, ax
	shr bx, 3

	jmp process_row_p2

formula1:
	or bx, ax
	shr bx, 3

	jmp process_row_p2

formula2:
	and bx, ax
	shr bx, 3

	jmp process_row_p2

formula3:
	shr ax, 1
	add bx, ax
	shr bx, 2

	jmp process_row_p2

formula4:
	shr bx, 2

	jmp process_row_p2

formula5:
	mov bx, ax
	shr bx, 2

	jmp process_row_p2

formula6:
	mul bx, ax
	shr bx, 9

	jmp process_row_p2

formula7:
	shr bx, 1
	add bx, ax
	shr bx, 2
	
	mov ax, [frame]
	mov [formulaXor], al
	jmp process_row_p2

formula8:
    mov word [sineWaveAngle], cx

    fild word [sineWaveAngle] ; cx
    fmul dword [sineWaveFreq] ; cx * sineWaveFreq

    fsin ; sin(cx * sineWaveFreq)

    fimul dword [sineWaveSize] ; sin(cx * sineWaveFreq) * sineWaveSize
    fistp word [sineWaveResult] ; sineWaveResult = st(0)
	
    mov bx, dx
    shr bx, 2
    add bx, [sineWaveResult]

    jmp process_row_p2

done:
	ret

xorshift16:
    push bx
    mov ax, [seed]

    ; x ^= x << 7
    mov bx, ax
    shl bx, 7
    xor ax, bx

    ; x ^= x >> 9
    mov bx, ax
    shr bx, 9
    xor ax, bx

    ; x ^= x << 8
    mov bx, ax
    shl bx, 8
    xor ax, bx

    xor bx, bx
    mov [seed], ax

    pop bx
    ret

frame dw 0
seed dw 0x1010
formula dw 0

formulaXor dw 0

sineWaveAngle dw 0
sineWaveFreq dd 0.14
sineWaveSize dd 7
sineWaveResult dw 0

times 510-($-$$) db 0
dw 0xAA55