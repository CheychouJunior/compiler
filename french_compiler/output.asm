section .data
    newline db 10, 0
    digit_buffer db 16 dup(0)
    for_end_value dd 0
    input_buffer times 16 db 0
    array_index_temp dd 0
    array_bounds_msg db 'Array index out of bounds', 10, 0
    x dd 0
    y dd 0
    max dd 0

section .text
    global _start

_start:
    ; Load constant 10
    push 10
    ; Assignment to x
    pop eax
    mov [x], eax
    ; Load constant 20
    push 20
    ; Assignment to y
    pop eax
    mov [y], eax
    ; Load variable x
    push dword [x]
    ; Load variable y
    push dword [y]
    ; Greater than comparison
    pop ebx
    pop eax
    cmp eax, ebx
    setg al
    movzx eax, al
    push eax
    ; Load variable x
    push dword [x]
    ; Load variable y
    push dword [y]
    ; Ternary operation: condition ? true_value : false_value
    pop ecx          ; false value
    pop ebx          ; true value
    pop eax          ; condition
    cmp eax, 0
    je ternary_false_1
    push ebx         ; push true value
    jmp ternary_end_2
ternary_false_1:
    push ecx         ; push false value
ternary_end_2:
    ; Assignment to max
    pop eax
    mov [max], eax
    ; Write integer
    push dword [max]
    call print_integer
    add esp, 4

program_exit:
    ; Program exit point
    mov eax, 1          ; sys_exit
    mov ebx, 0          ; exit status
    int 0x80            ; system call

; Runtime support functions
check_array_bounds:
    ; Parameters: eax = index, ebx = array_size
    push ecx
    push edx
    
    ; Check if index < 0
    cmp eax, 0
    jl .bounds_error
    
    ; Check if index >= size
    cmp eax, ebx
    jge .bounds_error
    
    ; Index is valid
    pop edx
    pop ecx
    ret
    
.bounds_error:
    ; Print error message
    mov eax, 4
    mov ebx, 1
    mov ecx, array_bounds_msg
    mov edx, 26
    int 0x80
    
    ; Exit program
    mov eax, 1
    mov ebx, 1
    int 0x80
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
    
