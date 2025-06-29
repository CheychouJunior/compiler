%{
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
%}

%union {
    int entier;
    double reel;
    char *chaine;
}

%token <entier> NOMBRE_ENTIER
%token <reel> NOMBRE_REEL
%token <chaine> IDENTIFICATEUR CHAINE_LITT

%token ENTIER REEL CHAINE BOOLEEN TABLEAU
%token LIRE ECRIRE AFFICHER
%token SI ALORS SINON FINSI
%token TANTQUE FAIRE FINTANTQUE
%token POUR DE A PAS FINPOUR
%token FONCTION RETOURNER FINFONCTION
%token VRAI FAUX
%token ET OU NON

%token EGAL DIFFERENT INFEGAL SUPEGAL INFERIEUR SUPERIEUR
%token AFFECTATION PLUS MOINS FOIS DIVISE MODULO
%token PAREN_OUV PAREN_FERM ACCOLADE_OUV ACCOLADE_FERM
%token CROCHET_OUV CROCHET_FERM
%token POINT_VIRGULE VIRGULE

%type <entier> expression
%type <chaine> type

%left OU
%left ET
%left EGAL DIFFERENT
%left INFERIEUR SUPERIEUR INFEGAL SUPEGAL
%left PLUS MOINS
%left FOIS DIVISE MODULO
%right NON
%right MOINS_UNAIRE

%%

programme:
    {
        write_assembly_header();
        temp_code_file = fopen("temp_code.asm", "w");
    }
    liste_instructions
    {
        write_assembly_footer();
        write_variables_and_code();
    }
    ;

liste_instructions:
    /* vide */
    | liste_instructions instruction
    ;

instruction:
    declaration POINT_VIRGULE
    | affectation POINT_VIRGULE
    | instruction_si
    | instruction_tantque
    | instruction_pour
    | instruction_ecrire POINT_VIRGULE
    | instruction_lire POINT_VIRGULE
    | instruction_retourner POINT_VIRGULE
    | declaration_fonction
    | appel_fonction POINT_VIRGULE
    ;

declaration:
    type IDENTIFICATEUR {
        printf("Déclaration de variable %s de type %s\n", $2, $1);
        add_variable($2, $1);
        free($1);
        free($2);
    }
    | type IDENTIFICATEUR AFFECTATION expression {
        printf("Déclaration avec initialisation: %s = %d\n", $2, $4);
        add_variable($2, $1);
        
        fprintf(temp_code_file, "    ; Initialize %s\n", $2);
        fprintf(temp_code_file, "    pop eax\n");
        fprintf(temp_code_file, "    mov [%s], eax\n", $2);
        
        free($1);
        free($2);
    }
    | TABLEAU CROCHET_OUV NOMBRE_ENTIER CROCHET_FERM type IDENTIFICATEUR {
        printf("Déclaration de tableau %s[%d] de type %s\n", $6, $3, $5);
        add_array_variable($6, $5, $3);
        free($5);
        free($6);
    }
    | TABLEAU CROCHET_OUV NOMBRE_ENTIER CROCHET_FERM type IDENTIFICATEUR AFFECTATION ACCOLADE_OUV liste_valeurs_tableau ACCOLADE_FERM {
    printf("Déclaration de tableau %s[%d] avec initialisation\n", $6, $3);
    add_array_variable($6, $5, $3);
    
    fprintf(temp_code_file, "    ; Array %s initialization\n", $6);
    // The values are already on the stack from liste_valeurs_tableau
    // Pop them and store in reverse order
    for (int idx = $3 - 1; idx >= 0; idx--) {
        fprintf(temp_code_file, "    pop eax\n");
        fprintf(temp_code_file, "    mov [%s + %d], eax\n", $6, idx * 4);
    }
    
    free($5);
    free($6);
    }
    ;
    
liste_valeurs_tableau:
    expression
    | liste_valeurs_tableau VIRGULE expression
    ;

type:
    ENTIER      { 
        printf("Type: entier\n"); 
        $$ = strdup("entier");
    }
    | REEL      { 
        printf("Type: réel\n"); 
        $$ = strdup("reel");
    }
    | CHAINE    { 
        printf("Type: chaîne\n"); 
        $$ = strdup("chaine");
    }
    | BOOLEEN   { 
        printf("Type: booléen\n"); 
        $$ = strdup("booleen");
    }
    ;

affectation:
    IDENTIFICATEUR AFFECTATION expression {
        printf("Affectation à la variable: %s (valeur: %d)\n", $1, $3);
        
        if (find_variable($1) == -1) {
            fprintf(stderr, "Erreur: Variable '%s' non déclarée à la ligne %d\n", $1, yylineno);
        }
        
        fprintf(temp_code_file, "    ; Assignment to %s\n", $1);
        fprintf(temp_code_file, "    pop eax\n");
        fprintf(temp_code_file, "    mov [%s], eax\n", $1);
        
        free($1);
    }
    | IDENTIFICATEUR CROCHET_OUV expression CROCHET_FERM AFFECTATION expression {
        printf("Affectation à l'élément de tableau: %s[%d] = %d\n", $1, $3, $6);
        
        if (find_variable($1) == -1) {
            fprintf(stderr, "Erreur: Tableau '%s' non déclaré à la ligne %d\n", $1, yylineno);
        } else if (!is_array_variable($1)) {
            fprintf(stderr, "Erreur: '%s' n'est pas un tableau à la ligne %d\n", $1, yylineno);
        } else {
            fprintf(temp_code_file, "    ; Array assignment %s[index] = value\n", $1);
            fprintf(temp_code_file, "    pop eax          ; value\n");
            fprintf(temp_code_file, "    pop ebx          ; index\n");
            fprintf(temp_code_file, "    \n");
            fprintf(temp_code_file, "    ; Bounds check\n");
            fprintf(temp_code_file, "    push eax         ; save value\n");
            fprintf(temp_code_file, "    push ebx         ; save index\n");
            fprintf(temp_code_file, "    mov eax, ebx     ; index for bounds check\n");
            fprintf(temp_code_file, "    mov ebx, %d      ; array size\n", get_array_size($1));
            fprintf(temp_code_file, "    call check_array_bounds\n");
            fprintf(temp_code_file, "    pop ebx          ; restore index\n");
            fprintf(temp_code_file, "    pop eax          ; restore value\n");
            fprintf(temp_code_file, "    \n");
            fprintf(temp_code_file, "    ; Calculate address and store\n");
            fprintf(temp_code_file, "    mov edx, ebx     ; index\n");
            fprintf(temp_code_file, "    shl edx, 2       ; index * 4 (for 32-bit integers)\n");
            fprintf(temp_code_file, "    mov [%s + edx], eax ; store value at base + offset\n", $1);
            
            free($1);
        }
    }
    ;

instruction_si:
    SI expression ALORS {
        current_if_else_label = generate_label();
        current_if_end_label = generate_label();
        
        fprintf(temp_code_file, "    ; If condition check\n");
        fprintf(temp_code_file, "    pop eax\n");
        fprintf(temp_code_file, "    cmp eax, 0\n");
        fprintf(temp_code_file, "    je label_else_%d\n", current_if_else_label);
    }
    liste_instructions partie_sinon {
        printf("If statement complete\n");
    }
    ;

partie_sinon:
    FINSI {
        printf("Simple if (no else)\n");
        fprintf(temp_code_file, "label_else_%d:\n", current_if_else_label);
    }
    | SINON {
        fprintf(temp_code_file, "    jmp label_end_%d\n", current_if_end_label);
        fprintf(temp_code_file, "label_else_%d:\n", current_if_else_label);
    }
    liste_instructions FINSI {
        printf("If with else clause\n");
        fprintf(temp_code_file, "label_end_%d:\n", current_if_end_label);
    }
    ;


instruction_tantque:
    TANTQUE {
        int start_label = generate_label();
        fprintf(temp_code_file, "label_start_%d:\n", start_label);
        $<entier>$ = start_label;
    }
    expression FAIRE {
        int start_label = $<entier>2;
        int end_label = generate_label();
        fprintf(temp_code_file, "    ; While condition\n");
        fprintf(temp_code_file, "    pop eax\n");
        fprintf(temp_code_file, "    cmp eax, 0\n");
        fprintf(temp_code_file, "    je label_end_%d\n", end_label);
        $<entier>$ = end_label;
    }
    liste_instructions FINTANTQUE {
        int start_label = $<entier>2;
        int end_label = $<entier>5;
        printf("Boucle tant que\n");
        fprintf(temp_code_file, "    jmp label_start_%d\n", start_label);
        fprintf(temp_code_file, "label_end_%d:\n", end_label);
    }
    ;

instruction_pour:
    POUR IDENTIFICATEUR DE expression A expression FAIRE {
        printf("Début boucle pour %s\n", $2);
        
        // Store the loop variable name for later use
        strcpy(current_loop_var, $2);
        free($2);
        
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
    liste_instructions FINPOUR {
        printf("Fin boucle pour %s\n", current_loop_var);
        
        fprintf(temp_code_file, "    ; Increment and continue loop\n");
        fprintf(temp_code_file, "    inc dword [%s]\n", current_loop_var);
        fprintf(temp_code_file, "    jmp label_start_%d\n", current_loop_start_label);
        fprintf(temp_code_file, "label_end_%d:\n", current_loop_end_label);
    }
    ;

instruction_ecrire:
	ecrire_variable
    | ecrire_string_literal
    | ecrire_expression
    | ecrire_array_element
    ;
    
ecrire_variable:
    ECRIRE PAREN_OUV IDENTIFICATEUR PAREN_FERM {
        printf("Ecriture de variable: %s\n", $3);
        
        const char *var_type = get_variable_type($3);
        
        if (strcmp(var_type, "entier") == 0) {
            fprintf(temp_code_file, "    ; Write integer\n");
            fprintf(temp_code_file, "    push dword [%s]\n", $3);
            fprintf(temp_code_file, "    call print_integer\n");
            fprintf(temp_code_file, "    add esp, 4\n");
        } else if (strcmp(var_type, "reel") == 0) {
            fprintf(temp_code_file, "    ; Write real\n");
            fprintf(temp_code_file, "    push dword [%s]\n", $3);
            fprintf(temp_code_file, "    call print_real\n");
            fprintf(temp_code_file, "    add esp, 4\n");
        } else if (strcmp(var_type, "chaine") == 0) {
            fprintf(temp_code_file, "    ; Write string\n");
            fprintf(temp_code_file, "    push %s\n", $3);  // Push address of string
            fprintf(temp_code_file, "    call print_string\n");
            fprintf(temp_code_file, "    add esp, 4\n");
        } else {
        	fprintf(stderr, "Erreur: Variable '%s' de type inconnu\n", $3);
        }
        free($3);
    }
    ;

ecrire_array_element:
    ECRIRE PAREN_OUV IDENTIFICATEUR CROCHET_OUV expression CROCHET_FERM PAREN_FERM {
        printf("Ecriture d'élément de tableau: %s[%d]\n", $3, $5);
        
        if (find_variable($3) == -1) {
            fprintf(stderr, "Erreur: Tableau '%s' non déclaré à la ligne %d\n", $3, yylineno);
            free($3);
        } else if (!is_array_variable($3)) {
            fprintf(stderr, "Erreur: '%s' n'est pas un tableau à la ligne %d\n", $3, yylineno);
            free($3);
        } else {
            const char *element_type = get_array_element_type($3);
    
            fprintf(temp_code_file, "    ; Write array element %s[index]\n", $3);
            fprintf(temp_code_file, "    pop eax          ; index\n");
            fprintf(temp_code_file, "    \n");
            fprintf(temp_code_file, "    ; Bounds check\n");
            fprintf(temp_code_file, "    push eax         ; save index\n");
            fprintf(temp_code_file, "    mov ebx, %d      ; array size\n", get_array_size($3));
            fprintf(temp_code_file, "    call check_array_bounds\n");
            fprintf(temp_code_file, "    pop eax          ; restore index\n");
            fprintf(temp_code_file, "    \n");
            fprintf(temp_code_file, "    ; Calculate address and load value\n");
            fprintf(temp_code_file, "    mov edx, eax     ; index\n");
            fprintf(temp_code_file, "    shl edx, 2       ; index * 4\n");
            
            if (strcmp(element_type, "entier") == 0) {
                fprintf(temp_code_file, "    push dword [%s + edx]\n", $3);
                fprintf(temp_code_file, "    call print_integer\n");
                fprintf(temp_code_file, "    add esp, 4\n");
            } else if (strcmp(element_type, "reel") == 0) {
                fprintf(temp_code_file, "    push dword [%s + edx]\n", $3);
                fprintf(temp_code_file, "    call print_real\n");
                fprintf(temp_code_file, "    add esp, 4\n");
            } else if (strcmp(element_type, "chaine") == 0) {
                fprintf(temp_code_file, "    lea eax, [%s + edx]\n", $3);
                fprintf(temp_code_file, "    push eax\n");
                fprintf(temp_code_file, "    call print_string\n");
                fprintf(temp_code_file, "    add esp, 4\n");
            }
            
            free($3);
        }
    }
    ;


ecrire_string_literal:
    ECRIRE PAREN_OUV CHAINE_LITT PAREN_FERM {
        printf("Écriture de chaîne littérale: %s\n", $3);
        
        /* Generate unique label for this string */
        int string_label = generate_label();
        
        /* Add to string literals (we need to store these) */
        add_string_literal($3, string_label);
        
        fprintf(temp_code_file, "    ; Write string literal\n");
        fprintf(temp_code_file, "    push string_lit_%d\n", string_label);
        fprintf(temp_code_file, "    call print_string\n");
        fprintf(temp_code_file, "    add esp, 4\n");
        
        free($3);
    }
    ;

ecrire_expression:
    ECRIRE PAREN_OUV expression PAREN_FERM {
        printf("Écriture d'expression\n");
        
        /* For expressions, assume integer result */
        fprintf(temp_code_file, "    ; Write expression result\n");
        fprintf(temp_code_file, "    push dword [esp]\n");
        fprintf(temp_code_file, "    call print_integer\n");
        fprintf(temp_code_file, "    add esp, 4\n");
        fprintf(temp_code_file, "    pop eax\n");
    }
    ;

instruction_lire:
    LIRE PAREN_OUV IDENTIFICATEUR PAREN_FERM {
        printf("Instruction de lecture pour %s\n", $3);
        
        const char *var_type = get_variable_type($3);
        
        if (strcmp(var_type, "entier") == 0) {
            fprintf(temp_code_file, "    ; Read integer\n");
            fprintf(temp_code_file, "    call read_integer\n");
            fprintf(temp_code_file, "    mov [%s], eax\n", $3);
        } else if (strcmp(var_type, "reel") == 0) {
            fprintf(temp_code_file, "    ; Read real\n");
            fprintf(temp_code_file, "    call read_real\n");
            fprintf(temp_code_file, "    mov [%s], eax\n", $3);
        } else if (strcmp(var_type, "chaine") == 0) {
            fprintf(temp_code_file, "    ; Read string\n");
            fprintf(temp_code_file, "    push %s\n", $3);  // Push address of string buffer
            fprintf(temp_code_file, "    call read_string\n");
            fprintf(temp_code_file, "    add esp, 4\n");
        }
        
        free($3);
    }
    | LIRE PAREN_OUV IDENTIFICATEUR CROCHET_OUV expression CROCHET_FERM PAREN_FERM {
        printf("Instruction de lecture pour élément de tableau: %s[%d]\n", $3, $5);
        
        if (find_variable($3) == -1) {
            fprintf(stderr, "Erreur: Tableau '%s' non déclaré à la ligne %d\n", $3, yylineno);
            free($3);
        } else if (!is_array_variable($3)) {
            fprintf(stderr, "Erreur: '%s' n'est pas un tableau à la ligne %d\n", $3, yylineno);
            free($3);
        } else {
           const char *element_type = get_array_element_type($3);
    
            fprintf(temp_code_file, "    ; Read into array element %s[index]\n", $3);
            fprintf(temp_code_file, "    pop eax          ; index\n");
            fprintf(temp_code_file, "    \n");
            fprintf(temp_code_file, "    ; Bounds check\n");
            fprintf(temp_code_file, "    push eax         ; save index\n");
            fprintf(temp_code_file, "    mov ebx, %d      ; array size\n", get_array_size($3));
            fprintf(temp_code_file, "    call check_array_bounds\n");
            fprintf(temp_code_file, "    pop eax          ; restore index\n");
            fprintf(temp_code_file, "    mov [array_index_temp], eax  ; save index\n");
            fprintf(temp_code_file, "    \n");
            
            if (strcmp(element_type, "entier") == 0) {
                fprintf(temp_code_file, "    call read_integer\n");
                fprintf(temp_code_file, "    mov ebx, [array_index_temp]  ; restore index\n");
                fprintf(temp_code_file, "    shl ebx, 2       ; index * 4\n");
                fprintf(temp_code_file, "    mov [%s + ebx], eax   ; store value\n", $3);
            } else if (strcmp(element_type, "reel") == 0) {
                fprintf(temp_code_file, "    call read_real\n");
                fprintf(temp_code_file, "    mov ebx, [array_index_temp]  ; restore index\n");
                fprintf(temp_code_file, "    shl ebx, 2       ; index * 4\n");
                fprintf(temp_code_file, "    mov [%s + ebx], eax   ; store value\n", $3);
            } else if (strcmp(element_type, "chaine") == 0) {
                fprintf(temp_code_file, "    mov ebx, [array_index_temp]  ; restore index\n");
                fprintf(temp_code_file, "    mov eax, 256     ; string size\n");
                fprintf(temp_code_file, "    mul ebx          ; index * 256\n");
                fprintf(temp_code_file, "    lea ecx, [%s + eax] ; calculate string address\n", $3);
                fprintf(temp_code_file, "    push ecx\n");
                fprintf(temp_code_file, "    call read_string\n");
                fprintf(temp_code_file, "    add esp, 4\n");
            }
            
            free($3);
        }
    }
    ;

instruction_retourner:
    RETOURNER expression {
        if (in_function) {
            printf("Retour de fonction (valeur: %d)\n", $2);
            function_has_return = 1;
            
            fprintf(temp_code_file, "    ; Function return\n");
            fprintf(temp_code_file, "    pop eax\n");
            fprintf(temp_code_file, "    pop ebp\n");
            fprintf(temp_code_file, "    ret\n");
        } else {
            printf("Retour de programme (valeur: %d)\n", $2);
            
            fprintf(temp_code_file, "    ; Program exit\n");
            fprintf(temp_code_file, "    pop eax\n");
            fprintf(temp_code_file, "    mov ebx, eax\n");
            fprintf(temp_code_file, "    mov eax, 1\n");
            fprintf(temp_code_file, "    int 0x80\n");
        }
    }
    ;

declaration_fonction:
    FONCTION IDENTIFICATEUR PAREN_OUV {
        printf("Début fonction %s\n", $2);
        
        /* Add jump before first function */
        if (!first_function_found) {
            fprintf(temp_code_file, "\n    ; Jump over function definitions\n");
            fprintf(temp_code_file, "    jmp program_exit\n");
            first_function_found = 1;
        }
        
        strcpy(current_function_name, $2);
        add_function($2);
        in_function = 1;
        function_has_return = 0;
        param_count = 0; /* Reset for each function */
        
        fprintf(temp_code_file, "\n; === Function %s ===\n", $2);
        fprintf(temp_code_file, "%s:\n", $2);
        fprintf(temp_code_file, "    push ebp\n");
        fprintf(temp_code_file, "    mov ebp, esp\n");
        
        free($2);
    }
    parametres PAREN_FERM {
    }
    liste_instructions FINFONCTION {
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
    ;

parametres:
    /* vide */
    | liste_parametres
    ;

liste_parametres:
    parametre
    | liste_parametres VIRGULE parametre
    ;

parametre:
    type IDENTIFICATEUR {
        printf("Paramètre %d: %s de type %s\n", param_count, $2, $1);
        
        /* Store parameter name */
        if (param_count < 10) {
            strcpy(function_parameters[param_count], $2);
        }
        add_variable($2, $1);
        
        /* Calculate correct stack offset */
        int stack_offset = 8 + (param_count * 4);
        
        fprintf(temp_code_file, "    ; Setup parameter %s at [ebp+%d]\n", $2, stack_offset);
        fprintf(temp_code_file, "    mov eax, [ebp+%d]\n", stack_offset);
        fprintf(temp_code_file, "    mov [%s], eax\n", $2);
        
        param_count++;  /* Increment for next parameter */
        free($1);
        free($2);
    }
    ;

liste_expressions:
    expression
    | liste_expressions VIRGULE expression
    ;

expression:
    NOMBRE_ENTIER {
        printf("Nombre entier: %d\n", $1);
        fprintf(temp_code_file, "    ; Load constant %d\n", $1);
        fprintf(temp_code_file, "    push %d\n", $1);
        $$ = $1;
    }
    | NOMBRE_REEL {
        printf("Nombre réel: %f\n", $1);
        fprintf(temp_code_file, "    ; Load real constant (converted to int)\n");
        fprintf(temp_code_file, "    push %d\n", (int)$1);
        $$ = (int)$1;
    }
    | IDENTIFICATEUR {
        printf("Variable: %s\n", $1);
        
        if (find_variable($1) == -1) {
            fprintf(stderr, "Erreur: Variable '%s' non déclarée à la ligne %d\n", $1, yylineno);
        }
        
        fprintf(temp_code_file, "    ; Load variable %s\n", $1);
        fprintf(temp_code_file, "    push dword [%s]\n", $1);
        
        $$ = 0;
        free($1);
    }
    | CHAINE_LITT {
        printf("Chaîne littérale: %s\n", $1);
        fprintf(temp_code_file, "    ; String literal (not fully implemented)\n");
        fprintf(temp_code_file, "    push 0\n");
        $$ = 0;
        free($1);
    }
    | VRAI {
        printf("Valeur booléenne: vrai\n");
        fprintf(temp_code_file, "    ; Boolean true\n");
        fprintf(temp_code_file, "    push 1\n");
        $$ = 1;
    }
    | FAUX {
        printf("Valeur booléenne: faux\n");
        fprintf(temp_code_file, "    ; Boolean false\n");
        fprintf(temp_code_file, "    push 0\n");
        $$ = 0;
    }
    | expression PLUS expression {
        printf("Addition: %d + %d = %d\n", $1, $3, $1 + $3);
        
        fprintf(temp_code_file, "    ; Addition\n");
        fprintf(temp_code_file, "    pop ebx\n");
        fprintf(temp_code_file, "    pop eax\n");
        fprintf(temp_code_file, "    add eax, ebx\n");
        fprintf(temp_code_file, "    push eax\n");
        
        $$ = $1 + $3;
    }
    | expression MOINS expression {
        printf("Soustraction: %d - %d = %d\n", $1, $3, $1 - $3);
        
        fprintf(temp_code_file, "    ; Subtraction\n");
        fprintf(temp_code_file, "    pop ebx\n");
        fprintf(temp_code_file, "    pop eax\n");
        fprintf(temp_code_file, "    sub eax, ebx\n");
        fprintf(temp_code_file, "    push eax\n");
        
        $$ = $1 - $3;
    }
    | expression FOIS expression {
        printf("Multiplication: %d * %d = %d\n", $1, $3, $1 * $3);
        
        fprintf(temp_code_file, "    ; Multiplication\n");
        fprintf(temp_code_file, "    pop ebx\n");
        fprintf(temp_code_file, "    pop eax\n");
        fprintf(temp_code_file, "    imul eax, ebx\n");
        fprintf(temp_code_file, "    push eax\n");
        
        $$ = $1 * $3;
    }
    | expression DIVISE expression {
        printf("Division: %d / %d\n", $1, $3);
        
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
        
        if ($3 != 0) {
            $$ = $1 / $3;
        } else {
            $$ = 0;
        }
    }
    | expression MODULO expression {
        printf("Modulo: %d %% %d\n", $1, $3);
        
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
        
        if ($3 != 0) {
            $$ = $1 % $3;
        } else {
            $$ = 0;
        }
    }
    | expression EGAL expression {
        printf("Comparaison d'égalité: %d == %d = %s\n", $1, $3, ($1 == $3) ? "vrai" : "faux");
        
        fprintf(temp_code_file, "    ; Equality comparison\n");
        fprintf(temp_code_file, "    pop ebx\n");
        fprintf(temp_code_file, "    pop eax\n");
        fprintf(temp_code_file, "    cmp eax, ebx\n");
        fprintf(temp_code_file, "    sete al\n");
        fprintf(temp_code_file, "    movzx eax, al\n");
        fprintf(temp_code_file, "    push eax\n");
        
        $$ = ($1 == $3) ? 1 : 0;
    }
    | expression DIFFERENT expression {
        printf("Comparaison de différence: %d != %d = %s\n", $1, $3, ($1 != $3) ? "vrai" : "faux");
        
        fprintf(temp_code_file, "    ; Inequality comparison\n");
        fprintf(temp_code_file, "    pop ebx\n");
        fprintf(temp_code_file, "    pop eax\n");
        fprintf(temp_code_file, "    cmp eax, ebx\n");
        fprintf(temp_code_file, "    setne al\n");
        fprintf(temp_code_file, "    movzx eax, al\n");
        fprintf(temp_code_file, "    push eax\n");
        
        $$ = ($1 != $3) ? 1 : 0;
    }
    | expression INFERIEUR expression {
        printf("Comparaison inférieur: %d < %d = %s\n", $1, $3, ($1 < $3) ? "vrai" : "faux");
        
        fprintf(temp_code_file, "    ; Less than comparison\n");
        fprintf(temp_code_file, "    pop ebx\n");
        fprintf(temp_code_file, "    pop eax\n");
        fprintf(temp_code_file, "    cmp eax, ebx\n");
        fprintf(temp_code_file, "    setl al\n");
        fprintf(temp_code_file, "    movzx eax, al\n");
        fprintf(temp_code_file, "    push eax\n");
        
        $$ = ($1 < $3) ? 1 : 0;
    }
    | expression SUPERIEUR expression {
        printf("Comparaison supérieur: %d > %d = %s\n", $1, $3, ($1 > $3) ? "vrai" : "faux");
        
        fprintf(temp_code_file, "    ; Greater than comparison\n");
        fprintf(temp_code_file, "    pop ebx\n");
        fprintf(temp_code_file, "    pop eax\n");
        fprintf(temp_code_file, "    cmp eax, ebx\n");
        fprintf(temp_code_file, "    setg al\n");
        fprintf(temp_code_file, "    movzx eax, al\n");
        fprintf(temp_code_file, "    push eax\n");
        
        $$ = ($1 > $3) ? 1 : 0;
    }
    | expression INFEGAL expression {
        printf("Comparaison inférieur ou égal: %d <= %d = %s\n", $1, $3, ($1 <= $3) ? "vrai" : "faux");
        
        fprintf(temp_code_file, "    ; Less than or equal comparison\n");
        fprintf(temp_code_file, "    pop ebx\n");
        fprintf(temp_code_file, "    pop eax\n");
        fprintf(temp_code_file, "    cmp eax, ebx\n");
        fprintf(temp_code_file, "    setle al\n");
        fprintf(temp_code_file, "    movzx eax, al\n");
        fprintf(temp_code_file, "    push eax\n");
        
        $$ = ($1 <= $3) ? 1 : 0;
    }
    | expression SUPEGAL expression {
        printf("Comparaison supérieur ou égal: %d >= %d = %s\n", $1, $3, ($1 >= $3) ? "vrai" : "faux");
        
        fprintf(temp_code_file, "    ; Greater than or equal comparison\n");
        fprintf(temp_code_file, "    pop ebx\n");
        fprintf(temp_code_file, "    pop eax\n");
        fprintf(temp_code_file, "    cmp eax, ebx\n");
        fprintf(temp_code_file, "    setge al\n");
        fprintf(temp_code_file, "    movzx eax, al\n");
        fprintf(temp_code_file, "    push eax\n");
        
        $$ = ($1 >= $3) ? 1 : 0;
    }
    | expression ET expression {
        printf("ET logique: %d et %d = %s\n", $1, $3, ($1 && $3) ? "vrai" : "faux");
        
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
        
        $$ = ($1 && $3) ? 1 : 0;
    }
    | expression OU expression {
        printf("OU logique: %d ou %d = %s\n", $1, $3, ($1 || $3) ? "vrai" : "faux");
        
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
        
        $$ = ($1 || $3) ? 1 : 0;
    }
    | NON expression {
        printf("NON logique: non %d = %s\n", $2, (!$2) ? "vrai" : "faux");
        
        fprintf(temp_code_file, "    ; Logical NOT\n");
        fprintf(temp_code_file, "    pop eax\n");
        fprintf(temp_code_file, "    test eax, eax\n");
        fprintf(temp_code_file, "    setz al\n");
        fprintf(temp_code_file, "    movzx eax, al\n");
        fprintf(temp_code_file, "    push eax\n");
        
        $$ = (!$2) ? 1 : 0;
    }
    | MOINS expression %prec MOINS_UNAIRE {
        printf("Moins unaire: -%d = %d\n", $2, -$2);
        
        fprintf(temp_code_file, "    ; Unary minus\n");
        fprintf(temp_code_file, "    pop eax\n");
        fprintf(temp_code_file, "    neg eax\n");
        fprintf(temp_code_file, "    push eax\n");
        
        $$ = -$2;
    }
    | PAREN_OUV expression PAREN_FERM {
        printf("Expression entre parenthèses: (%d)\n", $2);
        // No additional assembly needed - expression already on stack
        $$ = $2;
    }
    | IDENTIFICATEUR PAREN_OUV liste_expressions PAREN_FERM {
        printf("Appel de fonction: %s\n", $1);
        
        if (find_function($1) == -1) {
            fprintf(stderr, "Erreur: Fonction '%s' non déclarée à la ligne %d\n", $1, yylineno);
        }
        
        fprintf(temp_code_file, "    ; Function call: %s\n", $1);
        fprintf(temp_code_file, "    call %s\n", $1);
        fprintf(temp_code_file, "    push eax\n");
        
        $$ = 0;
        free($1);
    }
    | IDENTIFICATEUR PAREN_OUV PAREN_FERM {
        printf("Appel de fonction sans paramètres: %s\n", $1);
        
        if (find_function($1) == -1) {
            fprintf(stderr, "Erreur: Fonction '%s' non déclarée à la ligne %d\n", $1, yylineno);
        }
        
        fprintf(temp_code_file, "    ; Function call (no params): %s\n", $1);
        fprintf(temp_code_file, "    call %s\n", $1);
        fprintf(temp_code_file, "    push eax\n");
        
        $$ = 0;
        free($1);
    }
    | IDENTIFICATEUR CROCHET_OUV expression CROCHET_FERM {
    printf("Accès à l'élément de tableau: %s[index]\n", $1);
    
    if (find_variable($1) == -1) {
        fprintf(stderr, "Erreur: Tableau '%s' non déclaré à la ligne %d\n", $1, yylineno);
        $$ = 0;
        free($1);
    } else if (!is_array_variable($1)) {
        fprintf(stderr, "Erreur: '%s' n'est pas un tableau à la ligne %d\n", $1, yylineno);
        $$ = 0;
        free($1);
    } else {
        fprintf(temp_code_file, "    ; Array access %s[index]\n", $1);
        fprintf(temp_code_file, "    pop eax          ; index\n");
        fprintf(temp_code_file, "    \n");
        fprintf(temp_code_file, "    ; Bounds check\n");
        fprintf(temp_code_file, "    push eax         ; save index\n");
        fprintf(temp_code_file, "    mov ebx, %d      ; array size\n", get_array_size($1));
        fprintf(temp_code_file, "    call check_array_bounds\n");
        fprintf(temp_code_file, "    pop eax          ; restore index\n");
        fprintf(temp_code_file, "    \n");
        fprintf(temp_code_file, "    ; Calculate address and load value\n");
        fprintf(temp_code_file, "    mov edx, eax     ; index\n");
        fprintf(temp_code_file, "    shl edx, 2       ; index * 4\n");
        fprintf(temp_code_file, "    mov eax, [%s + edx] ; load value\n", $1);
        fprintf(temp_code_file, "    push eax         ; push value for expression\n");
        
        $$ = 0;
        free($1);
    }
}
    ;

appel_fonction:
    IDENTIFICATEUR PAREN_OUV liste_expressions PAREN_FERM {
        printf("Appel de fonction: %s\n", $1);
        
        fprintf(temp_code_file, "    ; Function call statement: %s\n", $1);
        fprintf(temp_code_file, "    call %s\n", $1);
        
        free($1);
    }
    | IDENTIFICATEUR PAREN_OUV PAREN_FERM {
        printf("Appel de fonction sans paramètres: %s\n", $1);
        
        fprintf(temp_code_file, "    ; Function call statement (no params): %s\n", $1);
        fprintf(temp_code_file, "    call %s\n", $1);
        
        free($1);
    }
    ;

%%

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
