section .data
    newline db 10, 0
    digit_buffer db 16 dup(0)
    for_end_value dd 0
    input_buffer times 16 db 0
    array_index_temp dd 0
    array_bounds_msg db 'Array index out of bounds', 10, 0
    nombres times 5 dd 0
    scores times 3 dd 0
    i dd 0
    somme dd 0
    string_lit_5 db 'La somme des éléments du beyngu est: ', 0
    string_lit_6 db 'Entrez 3 nouvelles valeurs:', 0
    string_lit_9 db 'valeur ', 0
    string_lit_10 db ': ', 0
    string_lit_11 db 'Nouvelles valeurs:', 0
    string_lit_14 db 'scores ', 0
    string_lit_15 db ' = ', 0

section .text
    global _start

_start:
    ; Load constant 10
    push 10
    ; Load constant 20
    push 20
    ; Load constant 30
    push 30
    ; Array scores initialization
    pop eax
    mov [scores + 8], eax
    pop eax
    mov [scores + 4], eax
    pop eax
    mov [scores + 0], eax
    ; Load constant 0
    push 0
    ; Load constant 1
    push 1
    ; Array assignment nombres[index] = value
    pop eax          ; value
    pop ebx          ; index
    
    ; Bounds check
    push eax         ; save value
    push ebx         ; save index
    mov eax, ebx     ; index for bounds check
    mov ebx, 5      ; array size
    call check_array_bounds
    pop ebx          ; restore index
    pop eax          ; restore value
    
    ; Calculate address and store
    mov edx, ebx     ; index
    shl edx, 2       ; index * 4 (for 32-bit integers)
    mov [nombres + edx], eax ; store value at base + offset
    ; Load constant 1
    push 1
    ; Load constant 2
    push 2
    ; Array assignment nombres[index] = value
    pop eax          ; value
    pop ebx          ; index
    
    ; Bounds check
    push eax         ; save value
    push ebx         ; save index
    mov eax, ebx     ; index for bounds check
    mov ebx, 5      ; array size
    call check_array_bounds
    pop ebx          ; restore index
    pop eax          ; restore value
    
    ; Calculate address and store
    mov edx, ebx     ; index
    shl edx, 2       ; index * 4 (for 32-bit integers)
    mov [nombres + edx], eax ; store value at base + offset
    ; Load constant 2
    push 2
    ; Load constant 3
    push 3
    ; Array assignment nombres[index] = value
    pop eax          ; value
    pop ebx          ; index
    
    ; Bounds check
    push eax         ; save value
    push ebx         ; save index
    mov eax, ebx     ; index for bounds check
    mov ebx, 5      ; array size
    call check_array_bounds
    pop ebx          ; restore index
    pop eax          ; restore value
    
    ; Calculate address and store
    mov edx, ebx     ; index
    shl edx, 2       ; index * 4 (for 32-bit integers)
    mov [nombres + edx], eax ; store value at base + offset
    ; Load constant 3
    push 3
    ; Load constant 4
    push 4
    ; Array assignment nombres[index] = value
    pop eax          ; value
    pop ebx          ; index
    
    ; Bounds check
    push eax         ; save value
    push ebx         ; save index
    mov eax, ebx     ; index for bounds check
    mov ebx, 5      ; array size
    call check_array_bounds
    pop ebx          ; restore index
    pop eax          ; restore value
    
    ; Calculate address and store
    mov edx, ebx     ; index
    shl edx, 2       ; index * 4 (for 32-bit integers)
    mov [nombres + edx], eax ; store value at base + offset
    ; Load constant 4
    push 4
    ; Load constant 5
    push 5
    ; Array assignment nombres[index] = value
    pop eax          ; value
    pop ebx          ; index
    
    ; Bounds check
    push eax         ; save value
    push ebx         ; save index
    mov eax, ebx     ; index for bounds check
    mov ebx, 5      ; array size
    call check_array_bounds
    pop ebx          ; restore index
    pop eax          ; restore value
    
    ; Calculate address and store
    mov edx, ebx     ; index
    shl edx, 2       ; index * 4 (for 32-bit integers)
    mov [nombres + edx], eax ; store value at base + offset
    ; Load constant 0
    push 0
    ; Assignment to somme
    pop eax
    mov [somme], eax
    ; Load constant 0
    push 0
    ; Load constant 2
    push 2
    ; For loop initialization
    pop ebx          ; end value
    pop eax          ; start value
    mov [i], eax    ; initialize loop variable
    mov [for_end_value], ebx  ; store end value
label_start_1:
    ; Check loop condition
    mov eax, [i]
    mov ebx, [for_end_value]
    cmp eax, ebx
    jg label_end_2
    ; Load variable i
    push dword [i]
    ; Write array element scores[index]
    pop eax          ; index
    
    ; Bounds check
    push eax         ; save index
    mov ebx, 3      ; array size
    call check_array_bounds
    pop eax          ; restore index
    
    ; Calculate address and load value
    mov edx, eax     ; index
    shl edx, 2       ; index * 4
    push dword [scores + edx]
    call print_integer
    add esp, 4
    ; Increment and continue loop
    inc dword [i]
    jmp label_start_1
label_end_2:
    ; Load constant 0
    push 0
    ; Load constant 4
    push 4
    ; For loop initialization
    pop ebx          ; end value
    pop eax          ; start value
    mov [i], eax    ; initialize loop variable
    mov [for_end_value], ebx  ; store end value
label_start_3:
    ; Check loop condition
    mov eax, [i]
    mov ebx, [for_end_value]
    cmp eax, ebx
    jg label_end_4
    ; Load variable somme
    push dword [somme]
    ; Load variable i
    push dword [i]
    ; Array access nombres[index]
    pop eax          ; index
    
    ; Bounds check
    push eax         ; save index
    mov ebx, 5      ; array size
    call check_array_bounds
    pop eax          ; restore index
    
    ; Calculate address and load value
    mov edx, eax     ; index
    shl edx, 2       ; index * 4
    mov eax, [nombres + edx] ; load value
    push eax         ; push value for expression
    ; Addition
    pop ebx
    pop eax
    add eax, ebx
    push eax
    ; Assignment to somme
    pop eax
    mov [somme], eax
    ; Increment and continue loop
    inc dword [i]
    jmp label_start_3
label_end_4:
    ; Write string literal
    push string_lit_5
    call print_string
    add esp, 4
    ; Write integer
    push dword [somme]
    call print_integer
    add esp, 4
    ; Write string literal
    push string_lit_6
    call print_string
    add esp, 4
    ; Load constant 0
    push 0
    ; Load constant 2
    push 2
    ; For loop initialization
    pop ebx          ; end value
    pop eax          ; start value
    mov [i], eax    ; initialize loop variable
    mov [for_end_value], ebx  ; store end value
label_start_7:
    ; Check loop condition
    mov eax, [i]
    mov ebx, [for_end_value]
    cmp eax, ebx
    jg label_end_8
    ; Write string literal
    push string_lit_9
    call print_string
    add esp, 4
    ; Write integer
    push dword [i]
    call print_integer
    add esp, 4
    ; Write string literal
    push string_lit_10
    call print_string
    add esp, 4
    ; Load variable i
    push dword [i]
    ; Read into array element scores[index]
    pop eax          ; index
    
    ; Bounds check
    push eax         ; save index
    mov ebx, 3      ; array size
    call check_array_bounds
    pop eax          ; restore index
    mov [array_index_temp], eax  ; save index
    
    call read_integer
    mov ebx, [array_index_temp]  ; restore index
    shl ebx, 2       ; index * 4
    mov [scores + ebx], eax   ; store value
    ; Increment and continue loop
    inc dword [i]
    jmp label_start_7
label_end_8:
    ; Write string literal
    push string_lit_11
    call print_string
    add esp, 4
    ; Load constant 0
    push 0
    ; Load constant 2
    push 2
    ; For loop initialization
    pop ebx          ; end value
    pop eax          ; start value
    mov [i], eax    ; initialize loop variable
    mov [for_end_value], ebx  ; store end value
label_start_12:
    ; Check loop condition
    mov eax, [i]
    mov ebx, [for_end_value]
    cmp eax, ebx
    jg label_end_13
    ; Write string literal
    push string_lit_14
    call print_string
    add esp, 4
    ; Write integer
    push dword [i]
    call print_integer
    add esp, 4
    ; Write string literal
    push string_lit_15
    call print_string
    add esp, 4
    ; Load variable i
    push dword [i]
    ; Write array element scores[index]
    pop eax          ; index
    
    ; Bounds check
    push eax         ; save index
    mov ebx, 3      ; array size
    call check_array_bounds
    pop eax          ; restore index
    
    ; Calculate address and load value
    mov edx, eax     ; index
    shl edx, 2       ; index * 4
    push dword [scores + edx]
    call print_integer
    add esp, 4
    ; Increment and continue loop
    inc dword [i]
    jmp label_start_12
label_end_13:
    ; Load constant 0
    push 0
    ; Program exit
    pop eax
    mov ebx, eax
    mov eax, 1
    int 0x80

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
    
