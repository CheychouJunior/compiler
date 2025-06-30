section .data
    newline db 10, 0
    digit_buffer times 16 db 0
    for_end_value dd 0
    input_buffer times 16 db 0
    var_a dd 0
    var_b dd 0
    var_a dd 0
    var_b dd 0
    r dd 0
    string_lit_1 db 'le pgcd de est ', 0

section .text
    global _start

_start:
    ; Load constant 3
    push 3
    ; Assignment to var_a
    pop eax
    mov [var_a], eax
    ; Load constant 5
    push 5
    ; Assignment to var_b
    pop eax
    mov [var_b], eax
    ; Write string literal
    push string_lit_1
    call print_string
    add esp, 4
    ; Load variable var_a
    push dword [var_a]
    ; Load variable var_b
    push dword [var_b]
    ; Function call: calculer_pgcd
    call calculer_pgcd
    push eax
    ; Write expression result
    push dword [esp]
    call print_integer
    add esp, 4
    pop eax

    ; Jump over function definitions
    jmp program_exit

; === Function calculer_pgcd ===
calculer_pgcd:
    push ebp
    mov ebp, esp
    ; Setup parameter var_a at [ebp+8]
    mov eax, [ebp+8]
    mov [var_a], eax
    ; Setup parameter var_b at [ebp+12]
    mov eax, [ebp+12]
    mov [var_b], eax
label_start_2:
    ; Load variable var_b
    push dword [var_b]
    ; Load constant 0
    push 0
    ; Inequality comparison
    pop ebx
    pop eax
    cmp eax, ebx
    setne al
    movzx eax, al
    push eax
    ; While condition
    pop eax
    cmp eax, 0
    je label_end_3
    ; Load variable var_a
    push dword [var_a]
    ; Load variable var_b
    push dword [var_b]
    ; Modulo
    pop ebx
    pop eax
    ; Check for modulo by zero
    cmp ebx, 0
    je mod_by_zero_4
    cdq
    idiv ebx
    mov eax, edx        ; remainder
    jmp mod_end_5
mod_by_zero_4:
    mov eax, 0          ; result = 0 for modulo by zero
mod_end_5:
    push eax
    ; Assignment to r
    pop eax
    mov [r], eax
    ; Load variable var_b
    push dword [var_b]
    ; Assignment to var_a
    pop eax
    mov [var_a], eax
    ; Load variable r
    push dword [r]
    ; Assignment to var_b
    pop eax
    mov [var_b], eax
    jmp label_start_2
label_end_3:
    ; Load variable var_a
    push dword [var_a]
    ; Function return
    pop eax
    pop ebp
    ret
; === End calculer_pgcd ===


program_exit:
    ; Program exit point
    mov eax, 1          ; sys_exit
    mov ebx, 0          ; exit status
    int 0x80            ; system call

; Runtime support functions
read_integer:
    push ebp
    mov ebp, esp
    push ebx
    push ecx
    push edx
    push esi
    push edi
    
    ; Read from stdin
    mov eax, 3          ; sys_read
    mov ebx, 0          ; stdin
    mov ecx, input_buffer
    mov edx, 15         ; buffer size (leave space for null)
    int 0x80
    
    ; Null-terminate the input
    mov esi, input_buffer
    add esi, eax        ; esi points to end of input
    dec esi             ; back up over newline
    mov byte [esi], 0   ; null terminate
    
    ; Convert string to integer
    mov esi, input_buffer
    xor eax, eax        ; result = 0
    xor ebx, ebx        ; temp = 0
    mov edi, 1          ; sign = positive
    
    ; Check for negative sign
    cmp byte [esi], '-'
    jne .convert_start
    mov edi, -1         ; sign = negative
    inc esi             ; skip minus sign
    
.convert_start:
    mov bl, [esi]       ; get character
    cmp bl, 0           ; null terminator?
    je .convert_done
    cmp bl, 10          ; newline?
    je .convert_done
    cmp bl, '0'         ; check if digit
    jl .convert_done
    cmp bl, '9'
    jg .convert_done
    
    sub bl, '0'         ; convert ASCII to digit
    imul eax, 10        ; result *= 10
    add eax, ebx        ; result += digit
    inc esi             ; next character
    jmp .convert_start
    
.convert_done:
    imul eax, edi       ; apply sign
    
    pop edi
    pop esi
    pop edx
    pop ecx
    pop ebx
    pop ebp
    ret
    
print_integer:
    push ebp
    mov ebp, esp
    push ebx
    push ecx
    push edx
    push esi
    
    mov eax, [ebp+8]    ; get the number to print
    mov esi, digit_buffer
    add esi, 15         ; point to end of buffer
    mov byte [esi], 0   ; null terminate
    dec esi
    
    mov ebx, 10         ; divisor
    test eax, eax
    jns .positive
    neg eax             ; make positive
    
.positive:
.convert_loop:
    xor edx, edx
    div ebx             ; eax = eax/10, edx = remainder
    add dl, '0'         ; convert to ASCII
    mov [esi], dl
    dec esi
    test eax, eax
    jnz .convert_loop
    
    inc esi             ; point to first digit
    
    ; Calculate string length
    mov ecx, digit_buffer
    add ecx, 15
    sub ecx, esi        ; length = end - start
    
    ; System call to write
    mov eax, 4          ; sys_write
    mov ebx, 1          ; stdout
    mov ecx, esi        ; string to print
    mov edx, ecx        ; length (recompute)
    mov edx, digit_buffer
    add edx, 15
    sub edx, esi
    int 0x80
    
    ; Print newline
    mov eax, 4
    mov ebx, 1
    mov ecx, newline
    mov edx, 1
    int 0x80
    
    pop esi
    pop edx
    pop ecx
    pop ebx
    pop ebp
    ret
read_real:
    push ebp
    mov ebp, esp
    push ebx
    push ecx
    push edx
    push esi
    
    ; Read from stdin
    mov eax, 3          ; sys_read
    mov ebx, 0          ; stdin
    mov ecx, input_buffer
    mov edx, 15
    int 0x80
    
    ; Simple approach: treat as integer for now
    ; Real floating-point parsing is complex
    ; For educational purposes, we'll read as integer
    call read_integer
    
    pop esi
    pop edx
    pop ecx
    pop ebx
    pop ebp
    ret
    
print_real:
    push ebp
    mov ebp, esp
    
    ; Simple approach: print as integer
    ; Real floating-point formatting is complex
    call print_integer
    
    pop ebp
    ret
read_string:
    push ebp
    mov ebp, esp
    push ebx
    push ecx
    push edx
    
    ; Get string buffer address from parameter
    mov ecx, [ebp+8]    ; string buffer address
    
    ; Read from stdin
    mov eax, 3          ; sys_read
    mov ebx, 0          ; stdin
    mov edx, 255        ; max string length
    int 0x80
    
    ; Remove newline
    dec eax
    add ecx, eax
    mov byte [ecx], 0   ; null terminate
    
    pop edx
    pop ecx
    pop ebx
    pop ebp
    ret
    
print_string:
    push ebp
    mov ebp, esp
    push ebx
    push ecx
    push edx
    push esi
    
    ; Get string address from parameter
    mov esi, [ebp+8]    ; string address
    
    ; Calculate string length
    mov ecx, esi        ; start of string
.strlen_loop:
    cmp byte [ecx], 0   ; check for null terminator
    je .strlen_done
    inc ecx
    jmp .strlen_loop
.strlen_done:
    sub ecx, esi        ; length = end - start
    mov edx, ecx        ; length
    
    ; Write string
    mov eax, 4          ; sys_write
    mov ebx, 1          ; stdout
    mov ecx, esi        ; string address
    int 0x80
    
    ; Print newline
    mov eax, 4
    mov ebx, 1
    mov ecx, newline
    mov edx, 1
    int 0x80
    
    pop esi
    pop edx
    pop ecx
    pop ebx
    pop ebp
    ret
    
