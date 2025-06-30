/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30802

/* Bison version string.  */
#define YYBISON_VERSION "3.8.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 1 "analyseur.y"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern int yylex();
void yyerror(const char *s);
extern int yylineno;

// Code generation globals
typedef struct {
    char name[50];
    char type[20];  // "entier", "reel", "chaine", "tableau_entier", "tableau_reel", "tableau_chaine"
    int array_size; // Size for arrays, 0 for non-arrays
    char element_type[20]; // Type of array elements
} variable_info;

typedef struct {
    char content[256];
    int label;
} string_literal;

FILE *output_file = NULL;
FILE *temp_code_file = NULL;
int label_counter = 0;
variable_info variables[100];
int var_count = 0;

int current_if_else_label = 0;
int current_if_end_label = 0;

char current_loop_var[50];
int current_loop_start_label = 0;
int current_loop_end_label = 0;

char current_function_name[50];
int in_function = 0;
char function_names[100][50];
int function_count = 0;
int function_has_return = 0;
int first_function_found = 0;

string_literal string_literals[100];
int string_count = 0;

char function_parameters[10][50];
int param_count = 0;

// Helper functions
int generate_label() {
    return ++label_counter;
}

void add_variable(const char *name, const char *type) {
    if (var_count < 100) {
        strcpy(variables[var_count].name, name);
        strcpy(variables[var_count].type, type);
        variables[var_count].array_size = 0;
        variables[var_count].element_type[0] = '\0';
        var_count++;
    }
}

void add_array_variable(const char *name, const char *element_type, int size) {
    if (var_count < 100) {
        strcpy(variables[var_count].name, name);
        snprintf(variables[var_count].type, sizeof(variables[var_count].type), "tableau_%s", element_type);
        strcpy(variables[var_count].element_type, element_type);
        variables[var_count].array_size = size;
        var_count++;
    }
}

int find_variable(const char *name) {
    for (int i = 0; i < var_count; i++) {
        if (strcmp(variables[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

const char* get_variable_type(const char *name) {
    for (int i = 0; i < var_count; i++) {
        if (strcmp(variables[i].name, name) == 0) {
            return variables[i].type;
        }
    }
    return "unknown";
}

int is_array_variable(const char *name) {
    int var_index = find_variable(name);
    if (var_index != -1) {
        return variables[var_index].array_size > 0;
    }
    return 0;
}

const char* get_array_element_type(const char *name) {
    int var_index = find_variable(name);
    if (var_index != -1 && variables[var_index].array_size > 0) {
        return variables[var_index].element_type;
    }
    return "unknown";
}

int get_array_size(const char *name) {
    int var_index = find_variable(name);
    if (var_index != -1) {
        return variables[var_index].array_size;
    }
    return 0;
}

void add_string_literal(const char *content, int label) {
    if (string_count < 100) {
        strcpy(string_literals[string_count].content, content);
        string_literals[string_count].label = label;
        string_count++;
    }
}

void write_assembly_header() {
    fprintf(output_file, "section .data\n");
    fprintf(output_file, "    newline db 10, 0\n");
    fprintf(output_file, "    digit_buffer db 16 dup(0)\n");
    fprintf(output_file, "    for_end_value dd 0\n");
    fprintf(output_file, "    input_buffer times 16 db 0\n");  
    fprintf(output_file, "    array_index_temp dd 0\n");
    fprintf(output_file, "    array_bounds_msg db 'Array index out of bounds', 10, 0\n");
}

void write_variables_and_code() {
    // Write all variables first
    for (int i = 0; i < var_count; i++) {
        if (variables[i].array_size > 0) {
            // Array variable
            if (strcmp(variables[i].element_type, "entier") == 0) {
                fprintf(output_file, "    %s times %d dd 0\n", variables[i].name, variables[i].array_size);
            } else if (strcmp(variables[i].element_type, "reel") == 0) {
                fprintf(output_file, "    %s times %d dd 0\n", variables[i].name, variables[i].array_size);
            } else if (strcmp(variables[i].element_type, "chaine") == 0) {
                fprintf(output_file, "    %s times %d times 256 db 0\n", variables[i].name, variables[i].array_size);
            }
        } else {
            // Regular variable
            if (strcmp(variables[i].type, "entier") == 0) {
                fprintf(output_file, "    %s dd 0\n", variables[i].name);
            } else if (strcmp(variables[i].type, "reel") == 0) {
                fprintf(output_file, "    %s dd 0\n", variables[i].name);
            } else if (strcmp(variables[i].type, "chaine") == 0) {
                fprintf(output_file, "    %s times 256 db 0\n", variables[i].name);
            }
        }
    }
    
    /* Add string literals */
    for (int i = 0; i < string_count; i++) {
        /* Remove quotes from string content */
        char clean_string[256];
        int len = strlen(string_literals[i].content);
        
        /* Copy string without quotes */
        strncpy(clean_string, string_literals[i].content + 1, len - 2);
        clean_string[len - 2] = '\0';
        
        fprintf(output_file, "    string_lit_%d db '%s', 0\n", 
                string_literals[i].label, clean_string);
    }
    
    // Write section headers
    fprintf(output_file, "\nsection .text\n");
    fprintf(output_file, "    global _start\n\n");
    fprintf(output_file, "_start:\n");
    
    // Copy code from temporary file
    fclose(temp_code_file);
    temp_code_file = fopen("temp_code.asm", "r");
    if (temp_code_file) {
        char line[1000];
        while (fgets(line, sizeof(line), temp_code_file)) {
            fprintf(output_file, "%s", line);
        }
        fclose(temp_code_file);
        remove("temp_code.asm");
    }
}

/* Array bounds checking function */
void write_array_bounds_check() {
    fprintf(output_file, "check_array_bounds:\n");
    fprintf(output_file, "    ; Parameters: eax = index, ebx = array_size\n");
    fprintf(output_file, "    push ecx\n");
    fprintf(output_file, "    push edx\n");
    fprintf(output_file, "    \n");
    fprintf(output_file, "    ; Check if index < 0\n");
    fprintf(output_file, "    cmp eax, 0\n");
    fprintf(output_file, "    jl .bounds_error\n");
    fprintf(output_file, "    \n");
    fprintf(output_file, "    ; Check if index >= size\n");
    fprintf(output_file, "    cmp eax, ebx\n");
    fprintf(output_file, "    jge .bounds_error\n");
    fprintf(output_file, "    \n");
    fprintf(output_file, "    ; Index is valid\n");
    fprintf(output_file, "    pop edx\n");
    fprintf(output_file, "    pop ecx\n");
    fprintf(output_file, "    ret\n");
    fprintf(output_file, "    \n");
    fprintf(output_file, ".bounds_error:\n");
    fprintf(output_file, "    ; Print error message\n");
    fprintf(output_file, "    mov eax, 4\n");
    fprintf(output_file, "    mov ebx, 1\n");
    fprintf(output_file, "    mov ecx, array_bounds_msg\n");
    fprintf(output_file, "    mov edx, 26\n");
    fprintf(output_file, "    int 0x80\n");
    fprintf(output_file, "    \n");
    fprintf(output_file, "    ; Exit program\n");
    fprintf(output_file, "    mov eax, 1\n");
    fprintf(output_file, "    mov ebx, 1\n");
    fprintf(output_file, "    int 0x80\n");
}

/* INTEGER FUNCTIONS */
void write_read_integer() {
    fprintf(output_file, "read_integer:\n");
    fprintf(output_file, "    push ebp\n");
    fprintf(output_file, "    mov ebp, esp\n");
    fprintf(output_file, "    push ebx\n");
    fprintf(output_file, "    push ecx\n");
    fprintf(output_file, "    push edx\n");
    fprintf(output_file, "    push esi\n");
    fprintf(output_file, "    push edi\n");
    fprintf(output_file, "    \n");
    fprintf(output_file, "    ; Read from stdin\n");
    fprintf(output_file, "    mov eax, 3          ; sys_read\n");
    fprintf(output_file, "    mov ebx, 0          ; stdin\n");
    fprintf(output_file, "    mov ecx, input_buffer\n");
    fprintf(output_file, "    mov edx, 15         ; buffer size (leave space for null)\n");
    fprintf(output_file, "    int 0x80\n");
    fprintf(output_file, "    \n");
    fprintf(output_file, "    ; Null-terminate the input\n");
    fprintf(output_file, "    mov esi, input_buffer\n");
    fprintf(output_file, "    add esi, eax        ; esi points to end of input\n");
    fprintf(output_file, "    dec esi             ; back up over newline\n");
    fprintf(output_file, "    mov byte [esi], 0   ; null terminate\n");
    fprintf(output_file, "    \n");
    fprintf(output_file, "    ; Convert string to integer\n");
    fprintf(output_file, "    mov esi, input_buffer\n");
    fprintf(output_file, "    xor eax, eax        ; result = 0\n");
    fprintf(output_file, "    xor ebx, ebx        ; temp = 0\n");
    fprintf(output_file, "    mov edi, 1          ; sign = positive\n");
    fprintf(output_file, "    \n");
    fprintf(output_file, "    ; Check for negative sign\n");
    fprintf(output_file, "    cmp byte [esi], '-'\n");
    fprintf(output_file, "    jne .convert_start\n");
    fprintf(output_file, "    mov edi, -1         ; sign = negative\n");
    fprintf(output_file, "    inc esi             ; skip minus sign\n");
    fprintf(output_file, "    \n");
    fprintf(output_file, ".convert_start:\n");
    fprintf(output_file, "    mov bl, [esi]       ; get character\n");
    fprintf(output_file, "    cmp bl, 0           ; null terminator?\n");
    fprintf(output_file, "    je .convert_done\n");
    fprintf(output_file, "    cmp bl, 10          ; newline?\n");
    fprintf(output_file, "    je .convert_done\n");
    fprintf(output_file, "    cmp bl, '0'         ; check if digit\n");
    fprintf(output_file, "    jl .convert_done\n");
    fprintf(output_file, "    cmp bl, '9'\n");
    fprintf(output_file, "    jg .convert_done\n");
    fprintf(output_file, "    \n");
    fprintf(output_file, "    sub bl, '0'         ; convert ASCII to digit\n");
    fprintf(output_file, "    imul eax, 10        ; result *= 10\n");
    fprintf(output_file, "    add eax, ebx        ; result += digit\n");
    fprintf(output_file, "    inc esi             ; next character\n");
    fprintf(output_file, "    jmp .convert_start\n");
    fprintf(output_file, "    \n");
    fprintf(output_file, ".convert_done:\n");
    fprintf(output_file, "    imul eax, edi       ; apply sign\n");
    fprintf(output_file, "    \n");
    fprintf(output_file, "    pop edi\n");
    fprintf(output_file, "    pop esi\n");
    fprintf(output_file, "    pop edx\n");
    fprintf(output_file, "    pop ecx\n");
    fprintf(output_file, "    pop ebx\n");
    fprintf(output_file, "    pop ebp\n");
    fprintf(output_file, "    ret\n");
    fprintf(output_file, "    \n");
}

void write_print_integer() {
    fprintf(output_file, "print_integer:\n");
    fprintf(output_file, "    push ebp\n");
    fprintf(output_file, "    mov ebp, esp\n");
    fprintf(output_file, "    push ebx\n");
    fprintf(output_file, "    push ecx\n");
    fprintf(output_file, "    push edx\n");
    fprintf(output_file, "    push esi\n");
    fprintf(output_file, "    \n");
    fprintf(output_file, "    mov eax, [ebp+8]    ; get the number to print\n");
    fprintf(output_file, "    mov esi, digit_buffer\n");
    fprintf(output_file, "    add esi, 15         ; point to end of buffer\n");
    fprintf(output_file, "    mov byte [esi], 0   ; null terminate\n");
    fprintf(output_file, "    dec esi\n");
    fprintf(output_file, "    \n");
    fprintf(output_file, "    mov ebx, 10         ; divisor\n");
    fprintf(output_file, "    test eax, eax\n");
    fprintf(output_file, "    jns .positive\n");
    fprintf(output_file, "    neg eax             ; make positive\n");
    fprintf(output_file, "    \n");
    fprintf(output_file, ".positive:\n");
    fprintf(output_file, ".convert_loop:\n");
    fprintf(output_file, "    xor edx, edx\n");
    fprintf(output_file, "    div ebx             ; eax = eax/10, edx = remainder\n");
    fprintf(output_file, "    add dl, '0'         ; convert to ASCII\n");
    fprintf(output_file, "    mov [esi], dl\n");
    fprintf(output_file, "    dec esi\n");
    fprintf(output_file, "    test eax, eax\n");
    fprintf(output_file, "    jnz .convert_loop\n");
    fprintf(output_file, "    \n");
    fprintf(output_file, "    inc esi             ; point to first digit\n");
    fprintf(output_file, "    \n");
    fprintf(output_file, "    ; Calculate string length\n");
    fprintf(output_file, "    mov ecx, digit_buffer\n");
    fprintf(output_file, "    add ecx, 15\n");
    fprintf(output_file, "    sub ecx, esi        ; length = end - start\n");
    fprintf(output_file, "    \n");
    fprintf(output_file, "    ; System call to write\n");
    fprintf(output_file, "    mov eax, 4          ; sys_write\n");
    fprintf(output_file, "    mov ebx, 1          ; stdout\n");
    fprintf(output_file, "    mov ecx, esi        ; string to print\n");
    fprintf(output_file, "    mov edx, ecx        ; length (recompute)\n");
    fprintf(output_file, "    mov edx, digit_buffer\n");
    fprintf(output_file, "    add edx, 15\n");
    fprintf(output_file, "    sub edx, esi\n");
    fprintf(output_file, "    int 0x80\n");
    fprintf(output_file, "    \n");
    fprintf(output_file, "    ; Print newline\n");
    fprintf(output_file, "    mov eax, 4\n");
    fprintf(output_file, "    mov ebx, 1\n");
    fprintf(output_file, "    mov ecx, newline\n");
    fprintf(output_file, "    mov edx, 1\n");
    fprintf(output_file, "    int 0x80\n");
    fprintf(output_file, "    \n");
    fprintf(output_file, "    pop esi\n");
    fprintf(output_file, "    pop edx\n");
    fprintf(output_file, "    pop ecx\n");
    fprintf(output_file, "    pop ebx\n");
    fprintf(output_file, "    pop ebp\n");
    fprintf(output_file, "    ret\n");
}

/* REAL/FLOAT FUNCTIONS */
void write_read_real() {
    fprintf(output_file, "read_real:\n");
    fprintf(output_file, "    push ebp\n");
    fprintf(output_file, "    mov ebp, esp\n");
    fprintf(output_file, "    push ebx\n");
    fprintf(output_file, "    push ecx\n");
    fprintf(output_file, "    push edx\n");
    fprintf(output_file, "    push esi\n");
    fprintf(output_file, "    \n");
    fprintf(output_file, "    ; Read from stdin\n");
    fprintf(output_file, "    mov eax, 3          ; sys_read\n");
    fprintf(output_file, "    mov ebx, 0          ; stdin\n");
    fprintf(output_file, "    mov ecx, input_buffer\n");
    fprintf(output_file, "    mov edx, 15\n");
    fprintf(output_file, "    int 0x80\n");
    fprintf(output_file, "    \n");
    fprintf(output_file, "    ; Simple approach: treat as integer for now\n");
    fprintf(output_file, "    ; Real floating-point parsing is complex\n");
    fprintf(output_file, "    ; For educational purposes, we'll read as integer\n");
    fprintf(output_file, "    call read_integer\n");
    fprintf(output_file, "    \n");
    fprintf(output_file, "    pop esi\n");
    fprintf(output_file, "    pop edx\n");
    fprintf(output_file, "    pop ecx\n");
    fprintf(output_file, "    pop ebx\n");
    fprintf(output_file, "    pop ebp\n");
    fprintf(output_file, "    ret\n");
    fprintf(output_file, "    \n");
}

void write_print_real() {
    fprintf(output_file, "print_real:\n");
    fprintf(output_file, "    push ebp\n");
    fprintf(output_file, "    mov ebp, esp\n");
    fprintf(output_file, "    \n");
    fprintf(output_file, "    ; Simple approach: print as integer\n");
    fprintf(output_file, "    ; Real floating-point formatting is complex\n");
    fprintf(output_file, "    call print_integer\n");
    fprintf(output_file, "    \n");
    fprintf(output_file, "    pop ebp\n");
    fprintf(output_file, "    ret\n");
}

/* STRING FUNCTIONS */
void write_read_string() {
    fprintf(output_file, "read_string:\n");
    fprintf(output_file, "    push ebp\n");
    fprintf(output_file, "    mov ebp, esp\n");
    fprintf(output_file, "    push ebx\n");
    fprintf(output_file, "    push ecx\n");
    fprintf(output_file, "    push edx\n");
    fprintf(output_file, "    \n");
    fprintf(output_file, "    ; Get string buffer address from parameter\n");
    fprintf(output_file, "    mov ecx, [ebp+8]    ; string buffer address\n");
    fprintf(output_file, "    \n");
    fprintf(output_file, "    ; Read from stdin\n");
    fprintf(output_file, "    mov eax, 3          ; sys_read\n");
    fprintf(output_file, "    mov ebx, 0          ; stdin\n");
    fprintf(output_file, "    mov edx, 255        ; max string length\n");
    fprintf(output_file, "    int 0x80\n");
    fprintf(output_file, "    \n");
    fprintf(output_file, "    ; Remove newline\n");
    fprintf(output_file, "    dec eax\n");
    fprintf(output_file, "    add ecx, eax\n");
    fprintf(output_file, "    mov byte [ecx], 0   ; null terminate\n");
    fprintf(output_file, "    \n");
    fprintf(output_file, "    pop edx\n");
    fprintf(output_file, "    pop ecx\n");
    fprintf(output_file, "    pop ebx\n");
    fprintf(output_file, "    pop ebp\n");
    fprintf(output_file, "    ret\n");
    fprintf(output_file, "    \n");
}

void write_print_string() {
    fprintf(output_file, "print_string:\n");
    fprintf(output_file, "    push ebp\n");
    fprintf(output_file, "    mov ebp, esp\n");
    fprintf(output_file, "    push ebx\n");
    fprintf(output_file, "    push ecx\n");
    fprintf(output_file, "    push edx\n");
    fprintf(output_file, "    push esi\n");
    fprintf(output_file, "    \n");
    fprintf(output_file, "    ; Get string address from parameter\n");
    fprintf(output_file, "    mov esi, [ebp+8]    ; string address\n");
    fprintf(output_file, "    \n");
    fprintf(output_file, "    ; Calculate string length\n");
    fprintf(output_file, "    mov ecx, esi        ; start of string\n");
    fprintf(output_file, ".strlen_loop:\n");
    fprintf(output_file, "    cmp byte [ecx], 0   ; check for null terminator\n");
    fprintf(output_file, "    je .strlen_done\n");
    fprintf(output_file, "    inc ecx\n");
    fprintf(output_file, "    jmp .strlen_loop\n");
    fprintf(output_file, ".strlen_done:\n");
    fprintf(output_file, "    sub ecx, esi        ; length = end - start\n");
    fprintf(output_file, "    mov edx, ecx        ; length\n");
    fprintf(output_file, "    \n");
    fprintf(output_file, "    ; Write string\n");
    fprintf(output_file, "    mov eax, 4          ; sys_write\n");
    fprintf(output_file, "    mov ebx, 1          ; stdout\n");
    fprintf(output_file, "    mov ecx, esi        ; string address\n");
    fprintf(output_file, "    int 0x80\n");
    fprintf(output_file, "    \n");
    fprintf(output_file, "    ; Print newline\n");
    fprintf(output_file, "    mov eax, 4\n");
    fprintf(output_file, "    mov ebx, 1\n");
    fprintf(output_file, "    mov ecx, newline\n");
    fprintf(output_file, "    mov edx, 1\n");
    fprintf(output_file, "    int 0x80\n");
    fprintf(output_file, "    \n");
    fprintf(output_file, "    pop esi\n");
    fprintf(output_file, "    pop edx\n");
    fprintf(output_file, "    pop ecx\n");
    fprintf(output_file, "    pop ebx\n");
    fprintf(output_file, "    pop ebp\n");
    fprintf(output_file, "    ret\n");
    fprintf(output_file, "    \n");
}


void write_runtime_functions() {
	fprintf(output_file, "\nprogram_exit:\n");
    fprintf(output_file, "    ; Program exit point\n");
    fprintf(output_file, "    mov eax, 1          ; sys_exit\n");
    fprintf(output_file, "    mov ebx, 0          ; exit status\n");
    fprintf(output_file, "    int 0x80            ; system call\n");

    fprintf(output_file, "\n; Runtime support functions\n");
    
    /* ARRAY BOUNDS CHECK */
    write_array_bounds_check();

    /* INTEGER FUNCTIONS */
    write_read_integer();
    write_print_integer();
    
    /* REAL/FLOAT FUNCTIONS */
    write_read_real();
    write_print_real();
    
    /* STRING FUNCTIONS */
    write_read_string();
    write_print_string();
    
}

void write_assembly_footer() {

}

void add_function(const char *name) {
    if (function_count < 100) {
        strcpy(function_names[function_count++], name);
    }
}

int find_function(const char *name) {
    for (int i = 0; i < function_count; i++) {
        if (strcmp(function_names[i], name) == 0) {
            return i;
        }
    }
    return -1;
}

#line 589 "analyseur.tab.c"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

#include "analyseur.tab.h"
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_NOMBRE_ENTIER = 3,              /* NOMBRE_ENTIER  */
  YYSYMBOL_NOMBRE_REEL = 4,                /* NOMBRE_REEL  */
  YYSYMBOL_IDENTIFICATEUR = 5,             /* IDENTIFICATEUR  */
  YYSYMBOL_CHAINE_LITT = 6,                /* CHAINE_LITT  */
  YYSYMBOL_ENTIER = 7,                     /* ENTIER  */
  YYSYMBOL_REEL = 8,                       /* REEL  */
  YYSYMBOL_CHAINE = 9,                     /* CHAINE  */
  YYSYMBOL_BOOLEEN = 10,                   /* BOOLEEN  */
  YYSYMBOL_TABLEAU = 11,                   /* TABLEAU  */
  YYSYMBOL_LIRE = 12,                      /* LIRE  */
  YYSYMBOL_ECRIRE = 13,                    /* ECRIRE  */
  YYSYMBOL_AFFICHER = 14,                  /* AFFICHER  */
  YYSYMBOL_SI = 15,                        /* SI  */
  YYSYMBOL_ALORS = 16,                     /* ALORS  */
  YYSYMBOL_SINON = 17,                     /* SINON  */
  YYSYMBOL_FINSI = 18,                     /* FINSI  */
  YYSYMBOL_TANTQUE = 19,                   /* TANTQUE  */
  YYSYMBOL_FAIRE = 20,                     /* FAIRE  */
  YYSYMBOL_FINTANTQUE = 21,                /* FINTANTQUE  */
  YYSYMBOL_POUR = 22,                      /* POUR  */
  YYSYMBOL_DE = 23,                        /* DE  */
  YYSYMBOL_A = 24,                         /* A  */
  YYSYMBOL_PAS = 25,                       /* PAS  */
  YYSYMBOL_FINPOUR = 26,                   /* FINPOUR  */
  YYSYMBOL_FONCTION = 27,                  /* FONCTION  */
  YYSYMBOL_RETOURNER = 28,                 /* RETOURNER  */
  YYSYMBOL_FINFONCTION = 29,               /* FINFONCTION  */
  YYSYMBOL_VRAI = 30,                      /* VRAI  */
  YYSYMBOL_FAUX = 31,                      /* FAUX  */
  YYSYMBOL_ET = 32,                        /* ET  */
  YYSYMBOL_OU = 33,                        /* OU  */
  YYSYMBOL_NON = 34,                       /* NON  */
  YYSYMBOL_EGAL = 35,                      /* EGAL  */
  YYSYMBOL_DIFFERENT = 36,                 /* DIFFERENT  */
  YYSYMBOL_INFEGAL = 37,                   /* INFEGAL  */
  YYSYMBOL_SUPEGAL = 38,                   /* SUPEGAL  */
  YYSYMBOL_INFERIEUR = 39,                 /* INFERIEUR  */
  YYSYMBOL_SUPERIEUR = 40,                 /* SUPERIEUR  */
  YYSYMBOL_AFFECTATION = 41,               /* AFFECTATION  */
  YYSYMBOL_PLUS = 42,                      /* PLUS  */
  YYSYMBOL_MOINS = 43,                     /* MOINS  */
  YYSYMBOL_FOIS = 44,                      /* FOIS  */
  YYSYMBOL_DIVISE = 45,                    /* DIVISE  */
  YYSYMBOL_MODULO = 46,                    /* MODULO  */
  YYSYMBOL_PAREN_OUV = 47,                 /* PAREN_OUV  */
  YYSYMBOL_PAREN_FERM = 48,                /* PAREN_FERM  */
  YYSYMBOL_ACCOLADE_OUV = 49,              /* ACCOLADE_OUV  */
  YYSYMBOL_ACCOLADE_FERM = 50,             /* ACCOLADE_FERM  */
  YYSYMBOL_CROCHET_OUV = 51,               /* CROCHET_OUV  */
  YYSYMBOL_CROCHET_FERM = 52,              /* CROCHET_FERM  */
  YYSYMBOL_POINT_VIRGULE = 53,             /* POINT_VIRGULE  */
  YYSYMBOL_VIRGULE = 54,                   /* VIRGULE  */
  YYSYMBOL_QUESTION = 55,                  /* QUESTION  */
  YYSYMBOL_DEUX_POINTS = 56,               /* DEUX_POINTS  */
  YYSYMBOL_INCREMENT = 57,                 /* INCREMENT  */
  YYSYMBOL_DECREMENT = 58,                 /* DECREMENT  */
  YYSYMBOL_MOINS_UNAIRE = 59,              /* MOINS_UNAIRE  */
  YYSYMBOL_YYACCEPT = 60,                  /* $accept  */
  YYSYMBOL_programme = 61,                 /* programme  */
  YYSYMBOL_62_1 = 62,                      /* $@1  */
  YYSYMBOL_liste_instructions = 63,        /* liste_instructions  */
  YYSYMBOL_instruction = 64,               /* instruction  */
  YYSYMBOL_declaration = 65,               /* declaration  */
  YYSYMBOL_liste_valeurs_tableau = 66,     /* liste_valeurs_tableau  */
  YYSYMBOL_type = 67,                      /* type  */
  YYSYMBOL_affectation = 68,               /* affectation  */
  YYSYMBOL_instruction_si = 69,            /* instruction_si  */
  YYSYMBOL_70_2 = 70,                      /* $@2  */
  YYSYMBOL_partie_sinon = 71,              /* partie_sinon  */
  YYSYMBOL_72_3 = 72,                      /* $@3  */
  YYSYMBOL_instruction_tantque = 73,       /* instruction_tantque  */
  YYSYMBOL_74_4 = 74,                      /* @4  */
  YYSYMBOL_75_5 = 75,                      /* @5  */
  YYSYMBOL_instruction_pour = 76,          /* instruction_pour  */
  YYSYMBOL_77_6 = 77,                      /* $@6  */
  YYSYMBOL_instruction_ecrire = 78,        /* instruction_ecrire  */
  YYSYMBOL_ecrire_variable = 79,           /* ecrire_variable  */
  YYSYMBOL_ecrire_array_element = 80,      /* ecrire_array_element  */
  YYSYMBOL_ecrire_string_literal = 81,     /* ecrire_string_literal  */
  YYSYMBOL_ecrire_expression = 82,         /* ecrire_expression  */
  YYSYMBOL_instruction_lire = 83,          /* instruction_lire  */
  YYSYMBOL_instruction_retourner = 84,     /* instruction_retourner  */
  YYSYMBOL_declaration_fonction = 85,      /* declaration_fonction  */
  YYSYMBOL_86_7 = 86,                      /* $@7  */
  YYSYMBOL_87_8 = 87,                      /* $@8  */
  YYSYMBOL_parametres = 88,                /* parametres  */
  YYSYMBOL_liste_parametres = 89,          /* liste_parametres  */
  YYSYMBOL_parametre = 90,                 /* parametre  */
  YYSYMBOL_liste_expressions = 91,         /* liste_expressions  */
  YYSYMBOL_expression = 92,                /* expression  */
  YYSYMBOL_appel_fonction = 93             /* appel_fonction  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
# undef UINT_LEAST8_MAX
# undef UINT_LEAST16_MAX
# define UINT_LEAST8_MAX 255
# define UINT_LEAST16_MAX 65535
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_uint8 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if !defined yyoverflow

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* !defined yyoverflow */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  3
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   696

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  60
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  34
/* YYNRULES -- Number of rules.  */
#define YYNRULES  88
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  179

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   314


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   565,   565,   565,   576,   578,   582,   583,   584,   585,
     586,   587,   588,   589,   590,   591,   595,   601,   612,   618,
     636,   637,   641,   645,   649,   653,   660,   673,   705,   705,
     720,   724,   724,   736,   741,   736,   760,   760,   795,   796,
     797,   798,   802,   830,   877,   896,   909,   931,   980,  1002,
    1025,  1002,  1043,  1045,  1049,  1050,  1054,  1077,  1078,  1082,
    1088,  1094,  1107,  1114,  1120,  1126,  1137,  1148,  1159,  1182,
    1206,  1219,  1232,  1245,  1258,  1271,  1284,  1303,  1322,  1334,
    1344,  1349,  1363,  1377,  1408,  1440,  1450,  1463,  1471
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if YYDEBUG || 0
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "NOMBRE_ENTIER",
  "NOMBRE_REEL", "IDENTIFICATEUR", "CHAINE_LITT", "ENTIER", "REEL",
  "CHAINE", "BOOLEEN", "TABLEAU", "LIRE", "ECRIRE", "AFFICHER", "SI",
  "ALORS", "SINON", "FINSI", "TANTQUE", "FAIRE", "FINTANTQUE", "POUR",
  "DE", "A", "PAS", "FINPOUR", "FONCTION", "RETOURNER", "FINFONCTION",
  "VRAI", "FAUX", "ET", "OU", "NON", "EGAL", "DIFFERENT", "INFEGAL",
  "SUPEGAL", "INFERIEUR", "SUPERIEUR", "AFFECTATION", "PLUS", "MOINS",
  "FOIS", "DIVISE", "MODULO", "PAREN_OUV", "PAREN_FERM", "ACCOLADE_OUV",
  "ACCOLADE_FERM", "CROCHET_OUV", "CROCHET_FERM", "POINT_VIRGULE",
  "VIRGULE", "QUESTION", "DEUX_POINTS", "INCREMENT", "DECREMENT",
  "MOINS_UNAIRE", "$accept", "programme", "$@1", "liste_instructions",
  "instruction", "declaration", "liste_valeurs_tableau", "type",
  "affectation", "instruction_si", "$@2", "partie_sinon", "$@3",
  "instruction_tantque", "@4", "@5", "instruction_pour", "$@6",
  "instruction_ecrire", "ecrire_variable", "ecrire_array_element",
  "ecrire_string_literal", "ecrire_expression", "instruction_lire",
  "instruction_retourner", "declaration_fonction", "$@7", "$@8",
  "parametres", "liste_parametres", "parametre", "liste_expressions",
  "expression", "appel_fonction", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-111)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-1)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
    -111,    11,  -111,  -111,   649,   -31,  -111,  -111,  -111,  -111,
     -49,   -30,   -17,    99,  -111,    36,    42,    99,  -111,    -5,
      56,    10,  -111,  -111,  -111,    25,  -111,  -111,  -111,  -111,
      26,    27,  -111,    29,    99,     9,    99,    59,    79,   104,
    -111,  -111,   -15,  -111,  -111,  -111,    99,    99,    99,   129,
      99,    63,    40,   426,  -111,    47,  -111,  -111,  -111,  -111,
    -111,   426,  -111,   -29,   426,   237,    38,   -24,     2,    48,
     264,    89,    99,    46,    46,   291,  -111,    99,    99,    99,
      99,    99,    99,    99,    99,    99,    99,    99,    99,    99,
      99,  -111,  -111,   156,    99,  -111,    99,  -111,    99,    65,
      90,  -111,    99,  -111,    99,  -111,  -111,  -111,   -26,   318,
    -111,  -111,   523,   450,   641,   641,   -37,   -37,   -37,   -37,
       0,     0,    46,    46,    46,   345,  -111,   210,    90,   426,
     426,    99,   106,   372,   399,  -111,  -111,   529,    99,  -111,
      99,   107,    66,    61,  -111,   426,    72,    68,    69,  -111,
    -111,  -111,    46,   577,   183,  -111,  -111,    90,    73,  -111,
    -111,  -111,  -111,  -111,  -111,  -111,    99,   601,  -111,   504,
     -12,   426,  -111,   625,  -111,  -111,    99,  -111,   426
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int8 yydefact[] =
{
       2,     0,     4,     1,     3,     0,    22,    23,    24,    25,
       0,     0,     0,     0,    33,     0,     0,     0,     5,     0,
       0,     0,     8,     9,    10,     0,    38,    41,    39,    40,
       0,     0,    14,     0,     0,     0,     0,     0,     0,     0,
      59,    60,    61,    62,    63,    64,     0,     0,     0,     0,
       0,     0,     0,    48,     6,    16,     7,    11,    12,    13,
      15,    26,    88,     0,    57,     0,     0,     0,    61,    62,
       0,     0,     0,    78,    79,     0,    28,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    85,    86,     0,     0,    49,     0,    87,     0,     0,
       0,    46,     0,    42,     0,    44,    45,    82,     0,     0,
      80,     4,    76,    77,    70,    71,    74,    75,    72,    73,
      65,    66,    67,    68,    69,     0,    34,     0,    52,    17,
      58,     0,     0,     0,     0,    81,    83,     0,     0,     4,
       0,     0,     0,    53,    54,    27,    18,     0,    83,    31,
      30,    29,    84,     0,     0,    56,    50,     0,     0,    47,
      43,     4,    35,    36,     4,    55,     0,     0,     4,     0,
       0,    20,    32,     0,    51,    19,     0,    37,    21
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
    -111,  -111,  -111,  -110,  -111,  -111,  -111,   -97,  -111,  -111,
    -111,  -111,  -111,  -111,  -111,  -111,  -111,  -111,  -111,  -111,
    -111,  -111,  -111,  -111,  -111,  -111,  -111,  -111,  -111,  -111,
     -36,    53,   -13,  -111
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_uint8 yydefgoto[] =
{
       0,     1,     2,     4,    18,    19,   170,    20,    21,    22,
     111,   151,   161,    23,    50,   139,    24,   168,    25,    26,
      27,    28,    29,    30,    31,    32,   128,   164,   142,   143,
     144,    63,    64,    33
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_uint8 yytable[] =
{
      49,   137,    37,   132,    53,    85,    86,    87,    88,    89,
      34,     3,    40,    41,    42,    43,    35,    38,    90,    97,
      36,    61,   135,    65,   101,    98,    70,   102,    98,   153,
      39,   141,    71,    73,    74,    75,    72,    93,   175,    44,
      45,    51,   176,    46,    87,    88,    89,    52,    54,    71,
     103,   167,    47,   104,   169,    90,    48,    62,   173,   109,
     141,    55,    66,    56,   112,   113,   114,   115,   116,   117,
     118,   119,   120,   121,   122,   123,   124,   125,    57,    58,
      59,   127,    60,   129,    67,   130,    94,    95,    96,   133,
     100,   134,    40,    41,    42,    43,   105,     6,     7,     8,
       9,    90,    40,    41,    42,    43,   131,    40,    41,    68,
      69,   146,   155,   158,   156,   157,   159,   160,   145,    44,
      45,   165,   166,    46,   108,   152,     0,   154,     0,    44,
      45,     0,    47,    46,    44,    45,    48,   107,    46,     0,
       0,     0,    47,     0,     0,    76,    48,    47,     0,     0,
       0,    48,     0,   171,     0,     0,     0,     0,     0,     0,
       0,    77,    78,   178,    79,    80,    81,    82,    83,    84,
       0,    85,    86,    87,    88,    89,   126,     0,     0,     0,
       0,     0,     0,     0,    90,     0,    91,    92,    77,    78,
       0,    79,    80,    81,    82,    83,    84,     0,    85,    86,
      87,    88,    89,   163,     0,     0,     0,     0,     0,     0,
       0,    90,     0,    91,    92,    77,    78,     0,    79,    80,
      81,    82,    83,    84,     0,    85,    86,    87,    88,    89,
       0,     0,     0,     0,   140,     0,     0,     0,    90,     0,
      91,    92,    77,    78,     0,    79,    80,    81,    82,    83,
      84,     0,    85,    86,    87,    88,    89,     0,     0,     0,
       0,     0,     0,     0,     0,    90,     0,    91,    92,    77,
      78,     0,    79,    80,    81,    82,    83,    84,     0,    85,
      86,    87,    88,    89,     0,     0,     0,     0,     0,    99,
       0,     0,    90,     0,    91,    92,    77,    78,     0,    79,
      80,    81,    82,    83,    84,     0,    85,    86,    87,    88,
      89,     0,   106,     0,     0,     0,     0,     0,     0,    90,
       0,    91,    92,    77,    78,     0,    79,    80,    81,    82,
      83,    84,     0,    85,    86,    87,    88,    89,     0,   110,
       0,     0,     0,     0,     0,     0,    90,     0,    91,    92,
      77,    78,     0,    79,    80,    81,    82,    83,    84,     0,
      85,    86,    87,    88,    89,     0,     0,     0,     0,     0,
     136,     0,     0,    90,     0,    91,    92,    77,    78,     0,
      79,    80,    81,    82,    83,    84,     0,    85,    86,    87,
      88,    89,     0,     0,     0,     0,     0,     0,     0,     0,
      90,   138,    91,    92,    77,    78,     0,    79,    80,    81,
      82,    83,    84,     0,    85,    86,    87,    88,    89,     0,
       0,     0,     0,     0,   147,     0,     0,    90,     0,    91,
      92,    77,    78,     0,    79,    80,    81,    82,    83,    84,
       0,    85,    86,    87,    88,    89,     0,     0,     0,     0,
       0,   148,     0,     0,    90,     0,    91,    92,    77,    78,
       0,    79,    80,    81,    82,    83,    84,     0,    85,    86,
      87,    88,    89,     0,     0,     0,     0,     0,     0,     0,
       0,    90,    77,    91,    92,    79,    80,    81,    82,    83,
      84,     0,    85,    86,    87,    88,    89,     0,     0,     0,
       0,     0,     0,     0,     0,    90,     0,    91,    92,     5,
       0,     6,     7,     8,     9,    10,    11,    12,     0,    13,
       0,     0,     0,    14,     0,     0,    15,     0,     0,     0,
       0,    16,    17,   174,     5,     0,     6,     7,     8,     9,
      10,    11,    12,     0,    13,     0,   149,   150,    14,     0,
       0,    15,     0,     0,     0,     0,    16,    17,    79,    80,
      81,    82,    83,    84,     0,    85,    86,    87,    88,    89,
       0,     0,     0,     0,     0,     0,     0,     0,    90,     0,
      91,    92,     5,     0,     6,     7,     8,     9,    10,    11,
      12,     0,    13,     0,     0,     0,    14,     0,   162,    15,
       0,     0,     0,     0,    16,    17,     5,     0,     6,     7,
       8,     9,    10,    11,    12,     0,    13,     0,     0,   172,
      14,     0,     0,    15,     0,     0,     0,     0,    16,    17,
       5,     0,     6,     7,     8,     9,    10,    11,    12,     0,
      13,     0,     0,     0,    14,     0,     0,    15,     0,     0,
       0,   177,    16,    17,     5,     0,     6,     7,     8,     9,
      10,    11,    12,     0,    13,     0,     0,     0,    14,     0,
       0,    15,     0,     0,     0,     0,    16,    17,    81,    82,
      83,    84,     0,    85,    86,    87,    88,    89,     0,     0,
       0,     0,     0,     0,     0,     0,    90
};

static const yytype_int16 yycheck[] =
{
      13,   111,    51,   100,    17,    42,    43,    44,    45,    46,
      41,     0,     3,     4,     5,     6,    47,    47,    55,    48,
      51,    34,    48,    36,    48,    54,    39,    51,    54,   139,
      47,   128,    47,    46,    47,    48,    51,    50,    50,    30,
      31,     5,    54,    34,    44,    45,    46,     5,    53,    47,
      48,   161,    43,    51,   164,    55,    47,    48,   168,    72,
     157,     5,     3,    53,    77,    78,    79,    80,    81,    82,
      83,    84,    85,    86,    87,    88,    89,    90,    53,    53,
      53,    94,    53,    96,     5,    98,    23,    47,    41,   102,
      52,   104,     3,     4,     5,     6,    48,     7,     8,     9,
      10,    55,     3,     4,     5,     6,    41,     3,     4,     5,
       6,     5,     5,    41,    48,    54,    48,    48,   131,    30,
      31,   157,    49,    34,    71,   138,    -1,   140,    -1,    30,
      31,    -1,    43,    34,    30,    31,    47,    48,    34,    -1,
      -1,    -1,    43,    -1,    -1,    16,    47,    43,    -1,    -1,
      -1,    47,    -1,   166,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    32,    33,   176,    35,    36,    37,    38,    39,    40,
      -1,    42,    43,    44,    45,    46,    20,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    55,    -1,    57,    58,    32,    33,
      -1,    35,    36,    37,    38,    39,    40,    -1,    42,    43,
      44,    45,    46,    20,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    55,    -1,    57,    58,    32,    33,    -1,    35,    36,
      37,    38,    39,    40,    -1,    42,    43,    44,    45,    46,
      -1,    -1,    -1,    -1,    24,    -1,    -1,    -1,    55,    -1,
      57,    58,    32,    33,    -1,    35,    36,    37,    38,    39,
      40,    -1,    42,    43,    44,    45,    46,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    55,    -1,    57,    58,    32,
      33,    -1,    35,    36,    37,    38,    39,    40,    -1,    42,
      43,    44,    45,    46,    -1,    -1,    -1,    -1,    -1,    52,
      -1,    -1,    55,    -1,    57,    58,    32,    33,    -1,    35,
      36,    37,    38,    39,    40,    -1,    42,    43,    44,    45,
      46,    -1,    48,    -1,    -1,    -1,    -1,    -1,    -1,    55,
      -1,    57,    58,    32,    33,    -1,    35,    36,    37,    38,
      39,    40,    -1,    42,    43,    44,    45,    46,    -1,    48,
      -1,    -1,    -1,    -1,    -1,    -1,    55,    -1,    57,    58,
      32,    33,    -1,    35,    36,    37,    38,    39,    40,    -1,
      42,    43,    44,    45,    46,    -1,    -1,    -1,    -1,    -1,
      52,    -1,    -1,    55,    -1,    57,    58,    32,    33,    -1,
      35,    36,    37,    38,    39,    40,    -1,    42,    43,    44,
      45,    46,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      55,    56,    57,    58,    32,    33,    -1,    35,    36,    37,
      38,    39,    40,    -1,    42,    43,    44,    45,    46,    -1,
      -1,    -1,    -1,    -1,    52,    -1,    -1,    55,    -1,    57,
      58,    32,    33,    -1,    35,    36,    37,    38,    39,    40,
      -1,    42,    43,    44,    45,    46,    -1,    -1,    -1,    -1,
      -1,    52,    -1,    -1,    55,    -1,    57,    58,    32,    33,
      -1,    35,    36,    37,    38,    39,    40,    -1,    42,    43,
      44,    45,    46,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    55,    32,    57,    58,    35,    36,    37,    38,    39,
      40,    -1,    42,    43,    44,    45,    46,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    55,    -1,    57,    58,     5,
      -1,     7,     8,     9,    10,    11,    12,    13,    -1,    15,
      -1,    -1,    -1,    19,    -1,    -1,    22,    -1,    -1,    -1,
      -1,    27,    28,    29,     5,    -1,     7,     8,     9,    10,
      11,    12,    13,    -1,    15,    -1,    17,    18,    19,    -1,
      -1,    22,    -1,    -1,    -1,    -1,    27,    28,    35,    36,
      37,    38,    39,    40,    -1,    42,    43,    44,    45,    46,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    55,    -1,
      57,    58,     5,    -1,     7,     8,     9,    10,    11,    12,
      13,    -1,    15,    -1,    -1,    -1,    19,    -1,    21,    22,
      -1,    -1,    -1,    -1,    27,    28,     5,    -1,     7,     8,
       9,    10,    11,    12,    13,    -1,    15,    -1,    -1,    18,
      19,    -1,    -1,    22,    -1,    -1,    -1,    -1,    27,    28,
       5,    -1,     7,     8,     9,    10,    11,    12,    13,    -1,
      15,    -1,    -1,    -1,    19,    -1,    -1,    22,    -1,    -1,
      -1,    26,    27,    28,     5,    -1,     7,     8,     9,    10,
      11,    12,    13,    -1,    15,    -1,    -1,    -1,    19,    -1,
      -1,    22,    -1,    -1,    -1,    -1,    27,    28,    37,    38,
      39,    40,    -1,    42,    43,    44,    45,    46,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    55
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,    61,    62,     0,    63,     5,     7,     8,     9,    10,
      11,    12,    13,    15,    19,    22,    27,    28,    64,    65,
      67,    68,    69,    73,    76,    78,    79,    80,    81,    82,
      83,    84,    85,    93,    41,    47,    51,    51,    47,    47,
       3,     4,     5,     6,    30,    31,    34,    43,    47,    92,
      74,     5,     5,    92,    53,     5,    53,    53,    53,    53,
      53,    92,    48,    91,    92,    92,     3,     5,     5,     6,
      92,    47,    51,    92,    92,    92,    16,    32,    33,    35,
      36,    37,    38,    39,    40,    42,    43,    44,    45,    46,
      55,    57,    58,    92,    23,    47,    41,    48,    54,    52,
      52,    48,    51,    48,    51,    48,    48,    48,    91,    92,
      48,    70,    92,    92,    92,    92,    92,    92,    92,    92,
      92,    92,    92,    92,    92,    92,    20,    92,    86,    92,
      92,    41,    67,    92,    92,    48,    52,    63,    56,    75,
      24,    67,    88,    89,    90,    92,     5,    52,    52,    17,
      18,    71,    92,    63,    92,     5,    48,    54,    41,    48,
      48,    72,    21,    20,    87,    90,    49,    63,    77,    63,
      66,    92,    18,    63,    29,    50,    54,    26,    92
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr1[] =
{
       0,    60,    62,    61,    63,    63,    64,    64,    64,    64,
      64,    64,    64,    64,    64,    64,    65,    65,    65,    65,
      66,    66,    67,    67,    67,    67,    68,    68,    70,    69,
      71,    72,    71,    74,    75,    73,    77,    76,    78,    78,
      78,    78,    79,    80,    81,    82,    83,    83,    84,    86,
      87,    85,    88,    88,    89,    89,    90,    91,    91,    92,
      92,    92,    92,    92,    92,    92,    92,    92,    92,    92,
      92,    92,    92,    92,    92,    92,    92,    92,    92,    92,
      92,    92,    92,    92,    92,    92,    92,    93,    93
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     0,     2,     0,     2,     2,     2,     1,     1,
       1,     2,     2,     2,     1,     2,     2,     4,     6,    10,
       1,     3,     1,     1,     1,     1,     3,     6,     0,     6,
       1,     0,     4,     0,     0,     7,     0,    10,     1,     1,
       1,     1,     4,     7,     4,     4,     4,     7,     2,     0,
       0,     9,     0,     1,     1,     3,     2,     1,     3,     1,
       1,     1,     1,     1,     1,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     2,     2,
       3,     4,     3,     4,     5,     2,     2,     4,     3
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYNOMEM         goto yyexhaustedlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)




# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  if (!yyvaluep)
    return;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  yy_symbol_value_print (yyo, yykind, yyvaluep);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp,
                 int yyrule)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)]);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif






/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep)
{
  YY_USE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/* Lookahead token kind.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;




/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */

  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    YYNOMEM;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        YYNOMEM;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          YYNOMEM;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */


  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      goto yyerrlab1;
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 2: /* $@1: %empty  */
#line 565 "analyseur.y"
    {
        write_assembly_header();
        temp_code_file = fopen("temp_code.asm", "w");
    }
#line 1900 "analyseur.tab.c"
    break;

  case 3: /* programme: $@1 liste_instructions  */
#line 570 "analyseur.y"
    {
        write_assembly_footer();
        write_variables_and_code();
    }
#line 1909 "analyseur.tab.c"
    break;

  case 16: /* declaration: type IDENTIFICATEUR  */
#line 595 "analyseur.y"
                        {
        printf("Déclaration de variable %s de type %s\n", (yyvsp[0].chaine), (yyvsp[-1].chaine));
        add_variable((yyvsp[0].chaine), (yyvsp[-1].chaine));
        free((yyvsp[-1].chaine));
        free((yyvsp[0].chaine));
    }
#line 1920 "analyseur.tab.c"
    break;

  case 17: /* declaration: type IDENTIFICATEUR AFFECTATION expression  */
#line 601 "analyseur.y"
                                                 {
        printf("Déclaration avec initialisation: %s = %d\n", (yyvsp[-2].chaine), (yyvsp[0].entier));
        add_variable((yyvsp[-2].chaine), (yyvsp[-3].chaine));
        
        fprintf(temp_code_file, "    ; Initialize %s\n", (yyvsp[-2].chaine));
        fprintf(temp_code_file, "    pop eax\n");
        fprintf(temp_code_file, "    mov [%s], eax\n", (yyvsp[-2].chaine));
        
        free((yyvsp[-3].chaine));
        free((yyvsp[-2].chaine));
    }
#line 1936 "analyseur.tab.c"
    break;

  case 18: /* declaration: TABLEAU CROCHET_OUV NOMBRE_ENTIER CROCHET_FERM type IDENTIFICATEUR  */
#line 612 "analyseur.y"
                                                                         {
        printf("Déclaration de tableau %s[%d] de type %s\n", (yyvsp[0].chaine), (yyvsp[-3].entier), (yyvsp[-1].chaine));
        add_array_variable((yyvsp[0].chaine), (yyvsp[-1].chaine), (yyvsp[-3].entier));
        free((yyvsp[-1].chaine));
        free((yyvsp[0].chaine));
    }
#line 1947 "analyseur.tab.c"
    break;

  case 19: /* declaration: TABLEAU CROCHET_OUV NOMBRE_ENTIER CROCHET_FERM type IDENTIFICATEUR AFFECTATION ACCOLADE_OUV liste_valeurs_tableau ACCOLADE_FERM  */
#line 618 "analyseur.y"
                                                                                                                                      {
    printf("Déclaration de tableau %s[%d] avec initialisation\n", (yyvsp[-4].chaine), (yyvsp[-7].entier));
    add_array_variable((yyvsp[-4].chaine), (yyvsp[-5].chaine), (yyvsp[-7].entier));
    
    fprintf(temp_code_file, "    ; Array %s initialization\n", (yyvsp[-4].chaine));
    // The values are already on the stack from liste_valeurs_tableau
    // Pop them and store in reverse order
    for (int idx = (yyvsp[-7].entier) - 1; idx >= 0; idx--) {
        fprintf(temp_code_file, "    pop eax\n");
        fprintf(temp_code_file, "    mov [%s + %d], eax\n", (yyvsp[-4].chaine), idx * 4);
    }
    
    free((yyvsp[-5].chaine));
    free((yyvsp[-4].chaine));
    }
#line 1967 "analyseur.tab.c"
    break;

  case 22: /* type: ENTIER  */
#line 641 "analyseur.y"
                { 
        printf("Type: entier\n"); 
        (yyval.chaine) = strdup("entier");
    }
#line 1976 "analyseur.tab.c"
    break;

  case 23: /* type: REEL  */
#line 645 "analyseur.y"
                { 
        printf("Type: réel\n"); 
        (yyval.chaine) = strdup("reel");
    }
#line 1985 "analyseur.tab.c"
    break;

  case 24: /* type: CHAINE  */
#line 649 "analyseur.y"
                { 
        printf("Type: chaîne\n"); 
        (yyval.chaine) = strdup("chaine");
    }
#line 1994 "analyseur.tab.c"
    break;

  case 25: /* type: BOOLEEN  */
#line 653 "analyseur.y"
                { 
        printf("Type: booléen\n"); 
        (yyval.chaine) = strdup("booleen");
    }
#line 2003 "analyseur.tab.c"
    break;

  case 26: /* affectation: IDENTIFICATEUR AFFECTATION expression  */
#line 660 "analyseur.y"
                                          {
        printf("Affectation à la variable: %s (valeur: %d)\n", (yyvsp[-2].chaine), (yyvsp[0].entier));
        
        if (find_variable((yyvsp[-2].chaine)) == -1) {
            fprintf(stderr, "Erreur: Variable '%s' non déclarée à la ligne %d\n", (yyvsp[-2].chaine), yylineno);
        }
        
        fprintf(temp_code_file, "    ; Assignment to %s\n", (yyvsp[-2].chaine));
        fprintf(temp_code_file, "    pop eax\n");
        fprintf(temp_code_file, "    mov [%s], eax\n", (yyvsp[-2].chaine));
        
        free((yyvsp[-2].chaine));
    }
#line 2021 "analyseur.tab.c"
    break;

  case 27: /* affectation: IDENTIFICATEUR CROCHET_OUV expression CROCHET_FERM AFFECTATION expression  */
#line 673 "analyseur.y"
                                                                                {
        printf("Affectation à l'élément de tableau: %s[%d] = %d\n", (yyvsp[-5].chaine), (yyvsp[-3].entier), (yyvsp[0].entier));
        
        if (find_variable((yyvsp[-5].chaine)) == -1) {
            fprintf(stderr, "Erreur: Tableau '%s' non déclaré à la ligne %d\n", (yyvsp[-5].chaine), yylineno);
        } else if (!is_array_variable((yyvsp[-5].chaine))) {
            fprintf(stderr, "Erreur: '%s' n'est pas un tableau à la ligne %d\n", (yyvsp[-5].chaine), yylineno);
        } else {
            fprintf(temp_code_file, "    ; Array assignment %s[index] = value\n", (yyvsp[-5].chaine));
            fprintf(temp_code_file, "    pop eax          ; value\n");
            fprintf(temp_code_file, "    pop ebx          ; index\n");
            fprintf(temp_code_file, "    \n");
            fprintf(temp_code_file, "    ; Bounds check\n");
            fprintf(temp_code_file, "    push eax         ; save value\n");
            fprintf(temp_code_file, "    push ebx         ; save index\n");
            fprintf(temp_code_file, "    mov eax, ebx     ; index for bounds check\n");
            fprintf(temp_code_file, "    mov ebx, %d      ; array size\n", get_array_size((yyvsp[-5].chaine)));
            fprintf(temp_code_file, "    call check_array_bounds\n");
            fprintf(temp_code_file, "    pop ebx          ; restore index\n");
            fprintf(temp_code_file, "    pop eax          ; restore value\n");
            fprintf(temp_code_file, "    \n");
            fprintf(temp_code_file, "    ; Calculate address and store\n");
            fprintf(temp_code_file, "    mov edx, ebx     ; index\n");
            fprintf(temp_code_file, "    shl edx, 2       ; index * 4 (for 32-bit integers)\n");
            fprintf(temp_code_file, "    mov [%s + edx], eax ; store value at base + offset\n", (yyvsp[-5].chaine));
            
            free((yyvsp[-5].chaine));
        }
    }
#line 2055 "analyseur.tab.c"
    break;

  case 28: /* $@2: %empty  */
#line 705 "analyseur.y"
                        {
        current_if_else_label = generate_label();
        current_if_end_label = generate_label();
        
        fprintf(temp_code_file, "    ; If condition check\n");
        fprintf(temp_code_file, "    pop eax\n");
        fprintf(temp_code_file, "    cmp eax, 0\n");
        fprintf(temp_code_file, "    je label_else_%d\n", current_if_else_label);
    }
#line 2069 "analyseur.tab.c"
    break;

  case 29: /* instruction_si: SI expression ALORS $@2 liste_instructions partie_sinon  */
#line 714 "analyseur.y"
                                    {
        printf("If statement complete\n");
    }
#line 2077 "analyseur.tab.c"
    break;

  case 30: /* partie_sinon: FINSI  */
#line 720 "analyseur.y"
          {
        printf("Simple if (no else)\n");
        fprintf(temp_code_file, "label_else_%d:\n", current_if_else_label);
    }
#line 2086 "analyseur.tab.c"
    break;

  case 31: /* $@3: %empty  */
#line 724 "analyseur.y"
            {
        fprintf(temp_code_file, "    jmp label_end_%d\n", current_if_end_label);
        fprintf(temp_code_file, "label_else_%d:\n", current_if_else_label);
    }
#line 2095 "analyseur.tab.c"
    break;

  case 32: /* partie_sinon: SINON $@3 liste_instructions FINSI  */
#line 728 "analyseur.y"
                             {
        printf("If with else clause\n");
        fprintf(temp_code_file, "label_end_%d:\n", current_if_end_label);
    }
#line 2104 "analyseur.tab.c"
    break;

  case 33: /* @4: %empty  */
#line 736 "analyseur.y"
            {
        int start_label = generate_label();
        fprintf(temp_code_file, "label_start_%d:\n", start_label);
        (yyval.entier) = start_label;
    }
#line 2114 "analyseur.tab.c"
    break;

  case 34: /* @5: %empty  */
#line 741 "analyseur.y"
                     {
        int start_label = (yyvsp[-2].entier);
        int end_label = generate_label();
        fprintf(temp_code_file, "    ; While condition\n");
        fprintf(temp_code_file, "    pop eax\n");
        fprintf(temp_code_file, "    cmp eax, 0\n");
        fprintf(temp_code_file, "    je label_end_%d\n", end_label);
        (yyval.entier) = end_label;
    }
#line 2128 "analyseur.tab.c"
    break;

  case 35: /* instruction_tantque: TANTQUE @4 expression FAIRE @5 liste_instructions FINTANTQUE  */
#line 750 "analyseur.y"
                                  {
        int start_label = (yyvsp[-5].entier);
        int end_label = (yyvsp[-2].entier);
        printf("Boucle tant que\n");
        fprintf(temp_code_file, "    jmp label_start_%d\n", start_label);
        fprintf(temp_code_file, "label_end_%d:\n", end_label);
    }
#line 2140 "analyseur.tab.c"
    break;

  case 36: /* $@6: %empty  */
#line 760 "analyseur.y"
                                                         {
        printf("Début boucle pour %s\n", (yyvsp[-5].chaine));
        
        // Store the loop variable name for later use
        strcpy(current_loop_var, (yyvsp[-5].chaine));
        free((yyvsp[-5].chaine));
        
        // Generate labels
        current_loop_start_label = generate_label();
        current_loop_end_label = generate_label();
        
        fprintf(temp_code_file, "    ; For loop initialization\n");
        fprintf(temp_code_file, "    pop ebx          ; end value\n");
        fprintf(temp_code_file, "    pop eax          ; start value\n");
        fprintf(temp_code_file, "    mov [%s], eax    ; initialize loop variable\n", current_loop_var);
        fprintf(temp_code_file, "    mov [for_end_value], ebx  ; store end value\n");
        
        fprintf(temp_code_file, "label_start_%d:\n", current_loop_start_label);
        fprintf(temp_code_file, "    ; Check loop condition\n");
        fprintf(temp_code_file, "    mov eax, [%s]\n", current_loop_var);
        fprintf(temp_code_file, "    mov ebx, [for_end_value]\n");
        fprintf(temp_code_file, "    cmp eax, ebx\n");
        fprintf(temp_code_file, "    jg label_end_%d\n", current_loop_end_label);
    }
#line 2169 "analyseur.tab.c"
    break;

  case 37: /* instruction_pour: POUR IDENTIFICATEUR DE expression A expression FAIRE $@6 liste_instructions FINPOUR  */
#line 784 "analyseur.y"
                               {
        printf("Fin boucle pour %s\n", current_loop_var);
        
        fprintf(temp_code_file, "    ; Increment and continue loop\n");
        fprintf(temp_code_file, "    inc dword [%s]\n", current_loop_var);
        fprintf(temp_code_file, "    jmp label_start_%d\n", current_loop_start_label);
        fprintf(temp_code_file, "label_end_%d:\n", current_loop_end_label);
    }
#line 2182 "analyseur.tab.c"
    break;

  case 42: /* ecrire_variable: ECRIRE PAREN_OUV IDENTIFICATEUR PAREN_FERM  */
#line 802 "analyseur.y"
                                               {
        printf("Ecriture de variable: %s\n", (yyvsp[-1].chaine));
        
        const char *var_type = get_variable_type((yyvsp[-1].chaine));
        
        if (strcmp(var_type, "entier") == 0) {
            fprintf(temp_code_file, "    ; Write integer\n");
            fprintf(temp_code_file, "    push dword [%s]\n", (yyvsp[-1].chaine));
            fprintf(temp_code_file, "    call print_integer\n");
            fprintf(temp_code_file, "    add esp, 4\n");
        } else if (strcmp(var_type, "reel") == 0) {
            fprintf(temp_code_file, "    ; Write real\n");
            fprintf(temp_code_file, "    push dword [%s]\n", (yyvsp[-1].chaine));
            fprintf(temp_code_file, "    call print_real\n");
            fprintf(temp_code_file, "    add esp, 4\n");
        } else if (strcmp(var_type, "chaine") == 0) {
            fprintf(temp_code_file, "    ; Write string\n");
            fprintf(temp_code_file, "    push %s\n", (yyvsp[-1].chaine));  // Push address of string
            fprintf(temp_code_file, "    call print_string\n");
            fprintf(temp_code_file, "    add esp, 4\n");
        } else {
        	fprintf(stderr, "Erreur: Variable '%s' de type inconnu\n", (yyvsp[-1].chaine));
        }
        free((yyvsp[-1].chaine));
    }
#line 2212 "analyseur.tab.c"
    break;

  case 43: /* ecrire_array_element: ECRIRE PAREN_OUV IDENTIFICATEUR CROCHET_OUV expression CROCHET_FERM PAREN_FERM  */
#line 830 "analyseur.y"
                                                                                   {
        printf("Ecriture d'élément de tableau: %s[%d]\n", (yyvsp[-4].chaine), (yyvsp[-2].entier));
        
        if (find_variable((yyvsp[-4].chaine)) == -1) {
            fprintf(stderr, "Erreur: Tableau '%s' non déclaré à la ligne %d\n", (yyvsp[-4].chaine), yylineno);
            free((yyvsp[-4].chaine));
        } else if (!is_array_variable((yyvsp[-4].chaine))) {
            fprintf(stderr, "Erreur: '%s' n'est pas un tableau à la ligne %d\n", (yyvsp[-4].chaine), yylineno);
            free((yyvsp[-4].chaine));
        } else {
            const char *element_type = get_array_element_type((yyvsp[-4].chaine));
    
            fprintf(temp_code_file, "    ; Write array element %s[index]\n", (yyvsp[-4].chaine));
            fprintf(temp_code_file, "    pop eax          ; index\n");
            fprintf(temp_code_file, "    \n");
            fprintf(temp_code_file, "    ; Bounds check\n");
            fprintf(temp_code_file, "    push eax         ; save index\n");
            fprintf(temp_code_file, "    mov ebx, %d      ; array size\n", get_array_size((yyvsp[-4].chaine)));
            fprintf(temp_code_file, "    call check_array_bounds\n");
            fprintf(temp_code_file, "    pop eax          ; restore index\n");
            fprintf(temp_code_file, "    \n");
            fprintf(temp_code_file, "    ; Calculate address and load value\n");
            fprintf(temp_code_file, "    mov edx, eax     ; index\n");
            fprintf(temp_code_file, "    shl edx, 2       ; index * 4\n");
            
            if (strcmp(element_type, "entier") == 0) {
                fprintf(temp_code_file, "    push dword [%s + edx]\n", (yyvsp[-4].chaine));
                fprintf(temp_code_file, "    call print_integer\n");
                fprintf(temp_code_file, "    add esp, 4\n");
            } else if (strcmp(element_type, "reel") == 0) {
                fprintf(temp_code_file, "    push dword [%s + edx]\n", (yyvsp[-4].chaine));
                fprintf(temp_code_file, "    call print_real\n");
                fprintf(temp_code_file, "    add esp, 4\n");
            } else if (strcmp(element_type, "chaine") == 0) {
                fprintf(temp_code_file, "    lea eax, [%s + edx]\n", (yyvsp[-4].chaine));
                fprintf(temp_code_file, "    push eax\n");
                fprintf(temp_code_file, "    call print_string\n");
                fprintf(temp_code_file, "    add esp, 4\n");
            }
            
            free((yyvsp[-4].chaine));
        }
    }
#line 2260 "analyseur.tab.c"
    break;

  case 44: /* ecrire_string_literal: ECRIRE PAREN_OUV CHAINE_LITT PAREN_FERM  */
#line 877 "analyseur.y"
                                            {
        printf("Écriture de chaîne littérale: %s\n", (yyvsp[-1].chaine));
        
        /* Generate unique label for this string */
        int string_label = generate_label();
        
        /* Add to string literals (we need to store these) */
        add_string_literal((yyvsp[-1].chaine), string_label);
        
        fprintf(temp_code_file, "    ; Write string literal\n");
        fprintf(temp_code_file, "    push string_lit_%d\n", string_label);
        fprintf(temp_code_file, "    call print_string\n");
        fprintf(temp_code_file, "    add esp, 4\n");
        
        free((yyvsp[-1].chaine));
    }
#line 2281 "analyseur.tab.c"
    break;

  case 45: /* ecrire_expression: ECRIRE PAREN_OUV expression PAREN_FERM  */
#line 896 "analyseur.y"
                                           {
        printf("Écriture d'expression\n");
        
        /* For expressions, assume integer result */
        fprintf(temp_code_file, "    ; Write expression result\n");
        fprintf(temp_code_file, "    push dword [esp]\n");
        fprintf(temp_code_file, "    call print_integer\n");
        fprintf(temp_code_file, "    add esp, 4\n");
        fprintf(temp_code_file, "    pop eax\n");
    }
#line 2296 "analyseur.tab.c"
    break;

  case 46: /* instruction_lire: LIRE PAREN_OUV IDENTIFICATEUR PAREN_FERM  */
#line 909 "analyseur.y"
                                             {
        printf("Instruction de lecture pour %s\n", (yyvsp[-1].chaine));
        
        const char *var_type = get_variable_type((yyvsp[-1].chaine));
        
        if (strcmp(var_type, "entier") == 0) {
            fprintf(temp_code_file, "    ; Read integer\n");
            fprintf(temp_code_file, "    call read_integer\n");
            fprintf(temp_code_file, "    mov [%s], eax\n", (yyvsp[-1].chaine));
        } else if (strcmp(var_type, "reel") == 0) {
            fprintf(temp_code_file, "    ; Read real\n");
            fprintf(temp_code_file, "    call read_real\n");
            fprintf(temp_code_file, "    mov [%s], eax\n", (yyvsp[-1].chaine));
        } else if (strcmp(var_type, "chaine") == 0) {
            fprintf(temp_code_file, "    ; Read string\n");
            fprintf(temp_code_file, "    push %s\n", (yyvsp[-1].chaine));  // Push address of string buffer
            fprintf(temp_code_file, "    call read_string\n");
            fprintf(temp_code_file, "    add esp, 4\n");
        }
        
        free((yyvsp[-1].chaine));
    }
#line 2323 "analyseur.tab.c"
    break;

  case 47: /* instruction_lire: LIRE PAREN_OUV IDENTIFICATEUR CROCHET_OUV expression CROCHET_FERM PAREN_FERM  */
#line 931 "analyseur.y"
                                                                                   {
        printf("Instruction de lecture pour élément de tableau: %s[%d]\n", (yyvsp[-4].chaine), (yyvsp[-2].entier));
        
        if (find_variable((yyvsp[-4].chaine)) == -1) {
            fprintf(stderr, "Erreur: Tableau '%s' non déclaré à la ligne %d\n", (yyvsp[-4].chaine), yylineno);
            free((yyvsp[-4].chaine));
        } else if (!is_array_variable((yyvsp[-4].chaine))) {
            fprintf(stderr, "Erreur: '%s' n'est pas un tableau à la ligne %d\n", (yyvsp[-4].chaine), yylineno);
            free((yyvsp[-4].chaine));
        } else {
           const char *element_type = get_array_element_type((yyvsp[-4].chaine));
    
            fprintf(temp_code_file, "    ; Read into array element %s[index]\n", (yyvsp[-4].chaine));
            fprintf(temp_code_file, "    pop eax          ; index\n");
            fprintf(temp_code_file, "    \n");
            fprintf(temp_code_file, "    ; Bounds check\n");
            fprintf(temp_code_file, "    push eax         ; save index\n");
            fprintf(temp_code_file, "    mov ebx, %d      ; array size\n", get_array_size((yyvsp[-4].chaine)));
            fprintf(temp_code_file, "    call check_array_bounds\n");
            fprintf(temp_code_file, "    pop eax          ; restore index\n");
            fprintf(temp_code_file, "    mov [array_index_temp], eax  ; save index\n");
            fprintf(temp_code_file, "    \n");
            
            if (strcmp(element_type, "entier") == 0) {
                fprintf(temp_code_file, "    call read_integer\n");
                fprintf(temp_code_file, "    mov ebx, [array_index_temp]  ; restore index\n");
                fprintf(temp_code_file, "    shl ebx, 2       ; index * 4\n");
                fprintf(temp_code_file, "    mov [%s + ebx], eax   ; store value\n", (yyvsp[-4].chaine));
            } else if (strcmp(element_type, "reel") == 0) {
                fprintf(temp_code_file, "    call read_real\n");
                fprintf(temp_code_file, "    mov ebx, [array_index_temp]  ; restore index\n");
                fprintf(temp_code_file, "    shl ebx, 2       ; index * 4\n");
                fprintf(temp_code_file, "    mov [%s + ebx], eax   ; store value\n", (yyvsp[-4].chaine));
            } else if (strcmp(element_type, "chaine") == 0) {
                fprintf(temp_code_file, "    mov ebx, [array_index_temp]  ; restore index\n");
                fprintf(temp_code_file, "    mov eax, 256     ; string size\n");
                fprintf(temp_code_file, "    mul ebx          ; index * 256\n");
                fprintf(temp_code_file, "    lea ecx, [%s + eax] ; calculate string address\n", (yyvsp[-4].chaine));
                fprintf(temp_code_file, "    push ecx\n");
                fprintf(temp_code_file, "    call read_string\n");
                fprintf(temp_code_file, "    add esp, 4\n");
            }
            
            free((yyvsp[-4].chaine));
        }
    }
#line 2374 "analyseur.tab.c"
    break;

  case 48: /* instruction_retourner: RETOURNER expression  */
#line 980 "analyseur.y"
                         {
        if (in_function) {
            printf("Retour de fonction (valeur: %d)\n", (yyvsp[0].entier));
            function_has_return = 1;
            
            fprintf(temp_code_file, "    ; Function return\n");
            fprintf(temp_code_file, "    pop eax\n");
            fprintf(temp_code_file, "    pop ebp\n");
            fprintf(temp_code_file, "    ret\n");
        } else {
            printf("Retour de programme (valeur: %d)\n", (yyvsp[0].entier));
            
            fprintf(temp_code_file, "    ; Program exit\n");
            fprintf(temp_code_file, "    pop eax\n");
            fprintf(temp_code_file, "    mov ebx, eax\n");
            fprintf(temp_code_file, "    mov eax, 1\n");
            fprintf(temp_code_file, "    int 0x80\n");
        }
    }
#line 2398 "analyseur.tab.c"
    break;

  case 49: /* $@7: %empty  */
#line 1002 "analyseur.y"
                                      {
        printf("Début fonction %s\n", (yyvsp[-1].chaine));
        
        /* Add jump before first function */
        if (!first_function_found) {
            fprintf(temp_code_file, "\n    ; Jump over function definitions\n");
            fprintf(temp_code_file, "    jmp program_exit\n");
            first_function_found = 1;
        }
        
        strcpy(current_function_name, (yyvsp[-1].chaine));
        add_function((yyvsp[-1].chaine));
        in_function = 1;
        function_has_return = 0;
        param_count = 0; /* Reset for each function */
        
        fprintf(temp_code_file, "\n; === Function %s ===\n", (yyvsp[-1].chaine));
        fprintf(temp_code_file, "%s:\n", (yyvsp[-1].chaine));
        fprintf(temp_code_file, "    push ebp\n");
        fprintf(temp_code_file, "    mov ebp, esp\n");
        
        free((yyvsp[-1].chaine));
    }
#line 2426 "analyseur.tab.c"
    break;

  case 50: /* $@8: %empty  */
#line 1025 "analyseur.y"
                          {
    }
#line 2433 "analyseur.tab.c"
    break;

  case 51: /* declaration_fonction: FONCTION IDENTIFICATEUR PAREN_OUV $@7 parametres PAREN_FERM $@8 liste_instructions FINFONCTION  */
#line 1027 "analyseur.y"
                                   {
        printf("Fin fonction %s\n", current_function_name);
        
        if (!function_has_return) {
            fprintf(temp_code_file, "    ; Automatic return\n");
            fprintf(temp_code_file, "    mov eax, 0\n");
            fprintf(temp_code_file, "    pop ebp\n");
            fprintf(temp_code_file, "    ret\n");
        }
        
        fprintf(temp_code_file, "; === End %s ===\n\n", current_function_name);
        in_function = 0;
        param_count = 0;
    }
#line 2452 "analyseur.tab.c"
    break;

  case 56: /* parametre: type IDENTIFICATEUR  */
#line 1054 "analyseur.y"
                        {
        printf("Paramètre %d: %s de type %s\n", param_count, (yyvsp[0].chaine), (yyvsp[-1].chaine));
        
        /* Store parameter name */
        if (param_count < 10) {
            strcpy(function_parameters[param_count], (yyvsp[0].chaine));
        }
        add_variable((yyvsp[0].chaine), (yyvsp[-1].chaine));
        
        /* Calculate correct stack offset */
        int stack_offset = 8 + (param_count * 4);
        
        fprintf(temp_code_file, "    ; Setup parameter %s at [ebp+%d]\n", (yyvsp[0].chaine), stack_offset);
        fprintf(temp_code_file, "    mov eax, [ebp+%d]\n", stack_offset);
        fprintf(temp_code_file, "    mov [%s], eax\n", (yyvsp[0].chaine));
        
        param_count++;  /* Increment for next parameter */
        free((yyvsp[-1].chaine));
        free((yyvsp[0].chaine));
    }
#line 2477 "analyseur.tab.c"
    break;

  case 59: /* expression: NOMBRE_ENTIER  */
#line 1082 "analyseur.y"
                  {
        printf("Nombre entier: %d\n", (yyvsp[0].entier));
        fprintf(temp_code_file, "    ; Load constant %d\n", (yyvsp[0].entier));
        fprintf(temp_code_file, "    push %d\n", (yyvsp[0].entier));
        (yyval.entier) = (yyvsp[0].entier);
    }
#line 2488 "analyseur.tab.c"
    break;

  case 60: /* expression: NOMBRE_REEL  */
#line 1088 "analyseur.y"
                  {
        printf("Nombre réel: %f\n", (yyvsp[0].reel));
        fprintf(temp_code_file, "    ; Load real constant (converted to int)\n");
        fprintf(temp_code_file, "    push %d\n", (int)(yyvsp[0].reel));
        (yyval.entier) = (int)(yyvsp[0].reel);
    }
#line 2499 "analyseur.tab.c"
    break;

  case 61: /* expression: IDENTIFICATEUR  */
#line 1094 "analyseur.y"
                     {
        printf("Variable: %s\n", (yyvsp[0].chaine));
        
        if (find_variable((yyvsp[0].chaine)) == -1) {
            fprintf(stderr, "Erreur: Variable '%s' non déclarée à la ligne %d\n", (yyvsp[0].chaine), yylineno);
        }
        
        fprintf(temp_code_file, "    ; Load variable %s\n", (yyvsp[0].chaine));
        fprintf(temp_code_file, "    push dword [%s]\n", (yyvsp[0].chaine));
        
        (yyval.entier) = 0;
        free((yyvsp[0].chaine));
    }
#line 2517 "analyseur.tab.c"
    break;

  case 62: /* expression: CHAINE_LITT  */
#line 1107 "analyseur.y"
                  {
        printf("Chaîne littérale: %s\n", (yyvsp[0].chaine));
        fprintf(temp_code_file, "    ; String literal (not fully implemented)\n");
        fprintf(temp_code_file, "    push 0\n");
        (yyval.entier) = 0;
        free((yyvsp[0].chaine));
    }
#line 2529 "analyseur.tab.c"
    break;

  case 63: /* expression: VRAI  */
#line 1114 "analyseur.y"
           {
        printf("Valeur booléenne: vrai\n");
        fprintf(temp_code_file, "    ; Boolean true\n");
        fprintf(temp_code_file, "    push 1\n");
        (yyval.entier) = 1;
    }
#line 2540 "analyseur.tab.c"
    break;

  case 64: /* expression: FAUX  */
#line 1120 "analyseur.y"
           {
        printf("Valeur booléenne: faux\n");
        fprintf(temp_code_file, "    ; Boolean false\n");
        fprintf(temp_code_file, "    push 0\n");
        (yyval.entier) = 0;
    }
#line 2551 "analyseur.tab.c"
    break;

  case 65: /* expression: expression PLUS expression  */
#line 1126 "analyseur.y"
                                 {
        printf("Addition: %d + %d = %d\n", (yyvsp[-2].entier), (yyvsp[0].entier), (yyvsp[-2].entier) + (yyvsp[0].entier));
        
        fprintf(temp_code_file, "    ; Addition\n");
        fprintf(temp_code_file, "    pop ebx\n");
        fprintf(temp_code_file, "    pop eax\n");
        fprintf(temp_code_file, "    add eax, ebx\n");
        fprintf(temp_code_file, "    push eax\n");
        
        (yyval.entier) = (yyvsp[-2].entier) + (yyvsp[0].entier);
    }
#line 2567 "analyseur.tab.c"
    break;

  case 66: /* expression: expression MOINS expression  */
#line 1137 "analyseur.y"
                                  {
        printf("Soustraction: %d - %d = %d\n", (yyvsp[-2].entier), (yyvsp[0].entier), (yyvsp[-2].entier) - (yyvsp[0].entier));
        
        fprintf(temp_code_file, "    ; Subtraction\n");
        fprintf(temp_code_file, "    pop ebx\n");
        fprintf(temp_code_file, "    pop eax\n");
        fprintf(temp_code_file, "    sub eax, ebx\n");
        fprintf(temp_code_file, "    push eax\n");
        
        (yyval.entier) = (yyvsp[-2].entier) - (yyvsp[0].entier);
    }
#line 2583 "analyseur.tab.c"
    break;

  case 67: /* expression: expression FOIS expression  */
#line 1148 "analyseur.y"
                                 {
        printf("Multiplication: %d * %d = %d\n", (yyvsp[-2].entier), (yyvsp[0].entier), (yyvsp[-2].entier) * (yyvsp[0].entier));
        
        fprintf(temp_code_file, "    ; Multiplication\n");
        fprintf(temp_code_file, "    pop ebx\n");
        fprintf(temp_code_file, "    pop eax\n");
        fprintf(temp_code_file, "    imul eax, ebx\n");
        fprintf(temp_code_file, "    push eax\n");
        
        (yyval.entier) = (yyvsp[-2].entier) * (yyvsp[0].entier);
    }
#line 2599 "analyseur.tab.c"
    break;

  case 68: /* expression: expression DIVISE expression  */
#line 1159 "analyseur.y"
                                   {
        printf("Division: %d / %d\n", (yyvsp[-2].entier), (yyvsp[0].entier));
        
        fprintf(temp_code_file, "    ; Division\n");
        fprintf(temp_code_file, "    pop ebx\n");
        fprintf(temp_code_file, "    pop eax\n");
        fprintf(temp_code_file, "    ; Check for division by zero\n");
        fprintf(temp_code_file, "    cmp ebx, 0\n");
        fprintf(temp_code_file, "    je div_by_zero_%d\n", generate_label());
        fprintf(temp_code_file, "    cdq\n");
        fprintf(temp_code_file, "    idiv ebx\n");
        fprintf(temp_code_file, "    jmp div_end_%d\n", generate_label());
        fprintf(temp_code_file, "div_by_zero_%d:\n", label_counter - 1);
        fprintf(temp_code_file, "    mov eax, 0          ; result = 0 for division by zero\n");
        fprintf(temp_code_file, "div_end_%d:\n", label_counter);
        fprintf(temp_code_file, "    push eax\n");
        
        if ((yyvsp[0].entier) != 0) {
            (yyval.entier) = (yyvsp[-2].entier) / (yyvsp[0].entier);
        } else {
            (yyval.entier) = 0;
        }
    }
#line 2627 "analyseur.tab.c"
    break;

  case 69: /* expression: expression MODULO expression  */
#line 1182 "analyseur.y"
                                   {
        printf("Modulo: %d %% %d\n", (yyvsp[-2].entier), (yyvsp[0].entier));
        
        fprintf(temp_code_file, "    ; Modulo\n");
        fprintf(temp_code_file, "    pop ebx\n");
        fprintf(temp_code_file, "    pop eax\n");
        fprintf(temp_code_file, "    ; Check for modulo by zero\n");
        fprintf(temp_code_file, "    cmp ebx, 0\n");
        fprintf(temp_code_file, "    je mod_by_zero_%d\n", generate_label());
        fprintf(temp_code_file, "    cdq\n");
        fprintf(temp_code_file, "    idiv ebx\n");
        fprintf(temp_code_file, "    mov eax, edx        ; remainder\n");
        fprintf(temp_code_file, "    jmp mod_end_%d\n", generate_label());
        fprintf(temp_code_file, "mod_by_zero_%d:\n", label_counter - 1);
        fprintf(temp_code_file, "    mov eax, 0          ; result = 0 for modulo by zero\n");
        fprintf(temp_code_file, "mod_end_%d:\n", label_counter);
        fprintf(temp_code_file, "    push eax\n");
        
        if ((yyvsp[0].entier) != 0) {
            (yyval.entier) = (yyvsp[-2].entier) % (yyvsp[0].entier);
        } else {
            (yyval.entier) = 0;
        }
    }
#line 2656 "analyseur.tab.c"
    break;

  case 70: /* expression: expression EGAL expression  */
#line 1206 "analyseur.y"
                                 {
        printf("Comparaison d'égalité: %d == %d = %s\n", (yyvsp[-2].entier), (yyvsp[0].entier), ((yyvsp[-2].entier) == (yyvsp[0].entier)) ? "vrai" : "faux");
        
        fprintf(temp_code_file, "    ; Equality comparison\n");
        fprintf(temp_code_file, "    pop ebx\n");
        fprintf(temp_code_file, "    pop eax\n");
        fprintf(temp_code_file, "    cmp eax, ebx\n");
        fprintf(temp_code_file, "    sete al\n");
        fprintf(temp_code_file, "    movzx eax, al\n");
        fprintf(temp_code_file, "    push eax\n");
        
        (yyval.entier) = ((yyvsp[-2].entier) == (yyvsp[0].entier)) ? 1 : 0;
    }
#line 2674 "analyseur.tab.c"
    break;

  case 71: /* expression: expression DIFFERENT expression  */
#line 1219 "analyseur.y"
                                      {
        printf("Comparaison de différence: %d != %d = %s\n", (yyvsp[-2].entier), (yyvsp[0].entier), ((yyvsp[-2].entier) != (yyvsp[0].entier)) ? "vrai" : "faux");
        
        fprintf(temp_code_file, "    ; Inequality comparison\n");
        fprintf(temp_code_file, "    pop ebx\n");
        fprintf(temp_code_file, "    pop eax\n");
        fprintf(temp_code_file, "    cmp eax, ebx\n");
        fprintf(temp_code_file, "    setne al\n");
        fprintf(temp_code_file, "    movzx eax, al\n");
        fprintf(temp_code_file, "    push eax\n");
        
        (yyval.entier) = ((yyvsp[-2].entier) != (yyvsp[0].entier)) ? 1 : 0;
    }
#line 2692 "analyseur.tab.c"
    break;

  case 72: /* expression: expression INFERIEUR expression  */
#line 1232 "analyseur.y"
                                      {
        printf("Comparaison inférieur: %d < %d = %s\n", (yyvsp[-2].entier), (yyvsp[0].entier), ((yyvsp[-2].entier) < (yyvsp[0].entier)) ? "vrai" : "faux");
        
        fprintf(temp_code_file, "    ; Less than comparison\n");
        fprintf(temp_code_file, "    pop ebx\n");
        fprintf(temp_code_file, "    pop eax\n");
        fprintf(temp_code_file, "    cmp eax, ebx\n");
        fprintf(temp_code_file, "    setl al\n");
        fprintf(temp_code_file, "    movzx eax, al\n");
        fprintf(temp_code_file, "    push eax\n");
        
        (yyval.entier) = ((yyvsp[-2].entier) < (yyvsp[0].entier)) ? 1 : 0;
    }
#line 2710 "analyseur.tab.c"
    break;

  case 73: /* expression: expression SUPERIEUR expression  */
#line 1245 "analyseur.y"
                                      {
        printf("Comparaison supérieur: %d > %d = %s\n", (yyvsp[-2].entier), (yyvsp[0].entier), ((yyvsp[-2].entier) > (yyvsp[0].entier)) ? "vrai" : "faux");
        
        fprintf(temp_code_file, "    ; Greater than comparison\n");
        fprintf(temp_code_file, "    pop ebx\n");
        fprintf(temp_code_file, "    pop eax\n");
        fprintf(temp_code_file, "    cmp eax, ebx\n");
        fprintf(temp_code_file, "    setg al\n");
        fprintf(temp_code_file, "    movzx eax, al\n");
        fprintf(temp_code_file, "    push eax\n");
        
        (yyval.entier) = ((yyvsp[-2].entier) > (yyvsp[0].entier)) ? 1 : 0;
    }
#line 2728 "analyseur.tab.c"
    break;

  case 74: /* expression: expression INFEGAL expression  */
#line 1258 "analyseur.y"
                                    {
        printf("Comparaison inférieur ou égal: %d <= %d = %s\n", (yyvsp[-2].entier), (yyvsp[0].entier), ((yyvsp[-2].entier) <= (yyvsp[0].entier)) ? "vrai" : "faux");
        
        fprintf(temp_code_file, "    ; Less than or equal comparison\n");
        fprintf(temp_code_file, "    pop ebx\n");
        fprintf(temp_code_file, "    pop eax\n");
        fprintf(temp_code_file, "    cmp eax, ebx\n");
        fprintf(temp_code_file, "    setle al\n");
        fprintf(temp_code_file, "    movzx eax, al\n");
        fprintf(temp_code_file, "    push eax\n");
        
        (yyval.entier) = ((yyvsp[-2].entier) <= (yyvsp[0].entier)) ? 1 : 0;
    }
#line 2746 "analyseur.tab.c"
    break;

  case 75: /* expression: expression SUPEGAL expression  */
#line 1271 "analyseur.y"
                                    {
        printf("Comparaison supérieur ou égal: %d >= %d = %s\n", (yyvsp[-2].entier), (yyvsp[0].entier), ((yyvsp[-2].entier) >= (yyvsp[0].entier)) ? "vrai" : "faux");
        
        fprintf(temp_code_file, "    ; Greater than or equal comparison\n");
        fprintf(temp_code_file, "    pop ebx\n");
        fprintf(temp_code_file, "    pop eax\n");
        fprintf(temp_code_file, "    cmp eax, ebx\n");
        fprintf(temp_code_file, "    setge al\n");
        fprintf(temp_code_file, "    movzx eax, al\n");
        fprintf(temp_code_file, "    push eax\n");
        
        (yyval.entier) = ((yyvsp[-2].entier) >= (yyvsp[0].entier)) ? 1 : 0;
    }
#line 2764 "analyseur.tab.c"
    break;

  case 76: /* expression: expression ET expression  */
#line 1284 "analyseur.y"
                               {
        printf("ET logique: %d et %d = %s\n", (yyvsp[-2].entier), (yyvsp[0].entier), ((yyvsp[-2].entier) && (yyvsp[0].entier)) ? "vrai" : "faux");
        
        fprintf(temp_code_file, "    ; Logical AND\n");
        fprintf(temp_code_file, "    pop ebx\n");
        fprintf(temp_code_file, "    pop eax\n");
        fprintf(temp_code_file, "    test eax, eax\n");
        fprintf(temp_code_file, "    jz and_false_%d\n", generate_label());
        fprintf(temp_code_file, "    test ebx, ebx\n");
        fprintf(temp_code_file, "    jz and_false_%d\n", label_counter);
        fprintf(temp_code_file, "    mov eax, 1\n");
        fprintf(temp_code_file, "    jmp and_end_%d\n", generate_label());
        fprintf(temp_code_file, "and_false_%d:\n", label_counter - 1);
        fprintf(temp_code_file, "    mov eax, 0\n");
        fprintf(temp_code_file, "and_end_%d:\n", label_counter);
        fprintf(temp_code_file, "    push eax\n");
        
        (yyval.entier) = ((yyvsp[-2].entier) && (yyvsp[0].entier)) ? 1 : 0;
    }
#line 2788 "analyseur.tab.c"
    break;

  case 77: /* expression: expression OU expression  */
#line 1303 "analyseur.y"
                               {
        printf("OU logique: %d ou %d = %s\n", (yyvsp[-2].entier), (yyvsp[0].entier), ((yyvsp[-2].entier) || (yyvsp[0].entier)) ? "vrai" : "faux");
        
        fprintf(temp_code_file, "    ; Logical OR\n");
        fprintf(temp_code_file, "    pop ebx\n");
        fprintf(temp_code_file, "    pop eax\n");
        fprintf(temp_code_file, "    test eax, eax\n");
        fprintf(temp_code_file, "    jnz or_true_%d\n", generate_label());
        fprintf(temp_code_file, "    test ebx, ebx\n");
        fprintf(temp_code_file, "    jnz or_true_%d\n", label_counter);
        fprintf(temp_code_file, "    mov eax, 0\n");
        fprintf(temp_code_file, "    jmp or_end_%d\n", generate_label());
        fprintf(temp_code_file, "or_true_%d:\n", label_counter - 1);
        fprintf(temp_code_file, "    mov eax, 1\n");
        fprintf(temp_code_file, "or_end_%d:\n", label_counter);
        fprintf(temp_code_file, "    push eax\n");
        
        (yyval.entier) = ((yyvsp[-2].entier) || (yyvsp[0].entier)) ? 1 : 0;
    }
#line 2812 "analyseur.tab.c"
    break;

  case 78: /* expression: NON expression  */
#line 1322 "analyseur.y"
                     {
        printf("NON logique: non %d = %s\n", (yyvsp[0].entier), (!(yyvsp[0].entier)) ? "vrai" : "faux");
        
        fprintf(temp_code_file, "    ; Logical NOT\n");
        fprintf(temp_code_file, "    pop eax\n");
        fprintf(temp_code_file, "    test eax, eax\n");
        fprintf(temp_code_file, "    setz al\n");
        fprintf(temp_code_file, "    movzx eax, al\n");
        fprintf(temp_code_file, "    push eax\n");
        
        (yyval.entier) = (!(yyvsp[0].entier)) ? 1 : 0;
    }
#line 2829 "analyseur.tab.c"
    break;

  case 79: /* expression: MOINS expression  */
#line 1334 "analyseur.y"
                                          {
        printf("Moins unaire: -%d = %d\n", (yyvsp[0].entier), -(yyvsp[0].entier));
        
        fprintf(temp_code_file, "    ; Unary minus\n");
        fprintf(temp_code_file, "    pop eax\n");
        fprintf(temp_code_file, "    neg eax\n");
        fprintf(temp_code_file, "    push eax\n");
        
        (yyval.entier) = -(yyvsp[0].entier);
    }
#line 2844 "analyseur.tab.c"
    break;

  case 80: /* expression: PAREN_OUV expression PAREN_FERM  */
#line 1344 "analyseur.y"
                                      {
        printf("Expression entre parenthèses: (%d)\n", (yyvsp[-1].entier));
        // No additional assembly needed - expression already on stack
        (yyval.entier) = (yyvsp[-1].entier);
    }
#line 2854 "analyseur.tab.c"
    break;

  case 81: /* expression: IDENTIFICATEUR PAREN_OUV liste_expressions PAREN_FERM  */
#line 1349 "analyseur.y"
                                                            {
        printf("Appel de fonction: %s\n", (yyvsp[-3].chaine));
        
        if (find_function((yyvsp[-3].chaine)) == -1) {
            fprintf(stderr, "Erreur: Fonction '%s' non déclarée à la ligne %d\n", (yyvsp[-3].chaine), yylineno);
        }
        
        fprintf(temp_code_file, "    ; Function call: %s\n", (yyvsp[-3].chaine));
        fprintf(temp_code_file, "    call %s\n", (yyvsp[-3].chaine));
        fprintf(temp_code_file, "    push eax\n");
        
        (yyval.entier) = 0;
        free((yyvsp[-3].chaine));
    }
#line 2873 "analyseur.tab.c"
    break;

  case 82: /* expression: IDENTIFICATEUR PAREN_OUV PAREN_FERM  */
#line 1363 "analyseur.y"
                                          {
        printf("Appel de fonction sans paramètres: %s\n", (yyvsp[-2].chaine));
        
        if (find_function((yyvsp[-2].chaine)) == -1) {
            fprintf(stderr, "Erreur: Fonction '%s' non déclarée à la ligne %d\n", (yyvsp[-2].chaine), yylineno);
        }
        
        fprintf(temp_code_file, "    ; Function call (no params): %s\n", (yyvsp[-2].chaine));
        fprintf(temp_code_file, "    call %s\n", (yyvsp[-2].chaine));
        fprintf(temp_code_file, "    push eax\n");
        
        (yyval.entier) = 0;
        free((yyvsp[-2].chaine));
    }
#line 2892 "analyseur.tab.c"
    break;

  case 83: /* expression: IDENTIFICATEUR CROCHET_OUV expression CROCHET_FERM  */
#line 1377 "analyseur.y"
                                                         {
    printf("Accès à l'élément de tableau: %s[index]\n", (yyvsp[-3].chaine));
    
    if (find_variable((yyvsp[-3].chaine)) == -1) {
        fprintf(stderr, "Erreur: Tableau '%s' non déclaré à la ligne %d\n", (yyvsp[-3].chaine), yylineno);
        (yyval.entier) = 0;
        free((yyvsp[-3].chaine));
    } else if (!is_array_variable((yyvsp[-3].chaine))) {
        fprintf(stderr, "Erreur: '%s' n'est pas un tableau à la ligne %d\n", (yyvsp[-3].chaine), yylineno);
        (yyval.entier) = 0;
        free((yyvsp[-3].chaine));
    } else {
            fprintf(temp_code_file, "    ; Array access %s[index]\n", (yyvsp[-3].chaine));
            fprintf(temp_code_file, "    pop eax          ; index\n");
            fprintf(temp_code_file, "    \n");
            fprintf(temp_code_file, "    ; Bounds check\n");
            fprintf(temp_code_file, "    push eax         ; save index\n");
            fprintf(temp_code_file, "    mov ebx, %d      ; array size\n", get_array_size((yyvsp[-3].chaine)));
            fprintf(temp_code_file, "    call check_array_bounds\n");
            fprintf(temp_code_file, "    pop eax          ; restore index\n");
            fprintf(temp_code_file, "    \n");
            fprintf(temp_code_file, "    ; Calculate address and load value\n");
            fprintf(temp_code_file, "    mov edx, eax     ; index\n");
            fprintf(temp_code_file, "    shl edx, 2       ; index * 4\n");
            fprintf(temp_code_file, "    mov eax, [%s + edx] ; load value\n", (yyvsp[-3].chaine));
            fprintf(temp_code_file, "    push eax         ; push value for expression\n");
            
            (yyval.entier) = 0;
            free((yyvsp[-3].chaine));
        }
    }
#line 2928 "analyseur.tab.c"
    break;

  case 84: /* expression: expression QUESTION expression DEUX_POINTS expression  */
#line 1408 "analyseur.y"
                                                            {
        printf("Expression ternaire: condition ? valeur_si_vrai : valeur_si_faux\n");
        
        // Generate unique labels for ternary operation
        int false_label = generate_label();
        int end_label = generate_label();
        
        fprintf(temp_code_file, "    ; Ternary operation: condition ? true_value : false_value\n");
        
        // Pop the false value (third expression)
        fprintf(temp_code_file, "    pop ecx          ; false value\n");
        // Pop the true value (second expression) 
        fprintf(temp_code_file, "    pop ebx          ; true value\n");
        // Pop the condition (first expression)
        fprintf(temp_code_file, "    pop eax          ; condition\n");
        
        // Test condition
        fprintf(temp_code_file, "    cmp eax, 0\n");
        fprintf(temp_code_file, "    je ternary_false_%d\n", false_label);
        
        // Condition is true - use true value
        fprintf(temp_code_file, "    push ebx         ; push true value\n");
        fprintf(temp_code_file, "    jmp ternary_end_%d\n", end_label);
        
        // Condition is false - use false value
        fprintf(temp_code_file, "ternary_false_%d:\n", false_label);
        fprintf(temp_code_file, "    push ecx         ; push false value\n");
        
        fprintf(temp_code_file, "ternary_end_%d:\n", end_label);
        
        (yyval.entier) = 0; // The result will be on the stack
    }
#line 2965 "analyseur.tab.c"
    break;

  case 85: /* expression: expression INCREMENT  */
#line 1440 "analyseur.y"
                           {
        printf("Incrémentation: %d++\n", (yyvsp[-1].entier));
        
        fprintf(temp_code_file, "    ; Increment\n");
        fprintf(temp_code_file, "    pop eax          ; value\n");
        fprintf(temp_code_file, "    inc eax          ; increment\n");
        fprintf(temp_code_file, "    push eax         ; push result\n");
        
        (yyval.entier) = (yyvsp[-1].entier) + 1;
    }
#line 2980 "analyseur.tab.c"
    break;

  case 86: /* expression: expression DECREMENT  */
#line 1450 "analyseur.y"
                           {
        printf("Décrémentation: %d--\n", (yyvsp[-1].entier));
        
        fprintf(temp_code_file, "    ; Decrement\n");
        fprintf(temp_code_file, "    pop eax          ; value\n");
        fprintf(temp_code_file, "    dec eax          ; decrement\n");
        fprintf(temp_code_file, "    push eax         ; push result\n");
        
        (yyval.entier) = (yyvsp[-1].entier) - 1;
    }
#line 2995 "analyseur.tab.c"
    break;

  case 87: /* appel_fonction: IDENTIFICATEUR PAREN_OUV liste_expressions PAREN_FERM  */
#line 1463 "analyseur.y"
                                                          {
        printf("Appel de fonction: %s\n", (yyvsp[-3].chaine));
        
        fprintf(temp_code_file, "    ; Function call statement: %s\n", (yyvsp[-3].chaine));
        fprintf(temp_code_file, "    call %s\n", (yyvsp[-3].chaine));
        
        free((yyvsp[-3].chaine));
    }
#line 3008 "analyseur.tab.c"
    break;

  case 88: /* appel_fonction: IDENTIFICATEUR PAREN_OUV PAREN_FERM  */
#line 1471 "analyseur.y"
                                          {
        printf("Appel de fonction sans paramètres: %s\n", (yyvsp[-2].chaine));
        
        fprintf(temp_code_file, "    ; Function call statement (no params): %s\n", (yyvsp[-2].chaine));
        fprintf(temp_code_file, "    call %s\n", (yyvsp[-2].chaine));
        
        free((yyvsp[-2].chaine));
    }
#line 3021 "analyseur.tab.c"
    break;


#line 3025 "analyseur.tab.c"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      yyerror (YY_("syntax error"));
    }

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;
  ++yynerrs;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;


      yydestruct ("Error: popping",
                  YY_ACCESSING_SYMBOL (yystate), yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturnlab;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturnlab;


/*-----------------------------------------------------------.
| yyexhaustedlab -- YYNOMEM (memory exhaustion) comes here.  |
`-----------------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturnlab;


/*----------------------------------------------------------.
| yyreturnlab -- parsing is finished, clean up and return.  |
`----------------------------------------------------------*/
yyreturnlab:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif

  return yyresult;
}

#line 1481 "analyseur.y"


void yyerror(const char *s) {
    fprintf(stderr, "Erreur de syntaxe: %s à la ligne %d\n", s, yylineno);
}

int main() {
    printf("=== Compilateur pour langage français - Génération de code ===\n");
    printf("Génération de code assembleur x86\n");
    
    // Open output file for assembly code
    output_file = fopen("output.asm", "w");
    if (!output_file) {
        fprintf(stderr, "Erreur: Impossible d'ouvrir le fichier de sortie output.asm\n");
        return 1;
    }
    
    printf("Entrez votre programme (Ctrl+D pour terminer):\n");
    
    // Parse the input and generate assembly
    int result = yyparse();
    
    // Write runtime functions
    write_runtime_functions();
    
    fclose(output_file);
    
    if (result == 0) {
        printf("\n✓ Compilation réussie!\n");
        printf("Code assembleur généré dans 'output.asm'\n");
        printf("Variables déclarées: %d\n", var_count);
        for (int i = 0; i < var_count; i++) {
            printf("  - %s\n", variables[i].name);
        }
        printf("\nPour assembler et exécuter:\n");
        printf("  nasm -f elf32 -o output.o output.asm\n");
        printf("  ld -m elf_i386 -o program output.o\n");
        printf("  ./program\n");
        printf("\nOu utilisez: make test\n");
    } else {
        printf("❌ Erreurs de compilation détectées.\n");
        printf("Vérifiez la syntaxe de votre programme.\n");
    }
    
    return result;
}
