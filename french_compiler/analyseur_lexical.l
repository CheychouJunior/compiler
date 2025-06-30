%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "analyseur.tab.h"

extern YYSTYPE yylval;
%}

%option noyywrap
%option yylineno

%%

 /* Mots-clés de types */
"entier"        { printf("[LEX] Token: ENTIER\n"); return ENTIER; }
"reel"          { printf("[LEX] Token: REEL\n"); return REEL; }
"chaine"        { printf("[LEX] Token: CHAINE\n"); return CHAINE; }
"booleen"       { printf("[LEX] Token: BOOLEEN\n"); return BOOLEEN; }
"tableau"       { printf("[LEX] Token: TABLEAU\n"); return TABLEAU; }

 /* Instructions d'E/S */
"lire"          { printf("[LEX] Token: LIRE\n"); return LIRE; }
"ecrire"        { printf("[LEX] Token: ECRIRE\n"); return ECRIRE; }
"afficher"      { printf("[LEX] Token: AFFICHER\n"); return AFFICHER; }

 /* Instructions conditionnelles */
"si"            { printf("[LEX] Token: SI\n"); return SI; }
"alors"         { printf("[LEX] Token: ALORS\n"); return ALORS; }
"sinon"         { printf("[LEX] Token: SINON\n"); return SINON; }
"finsi"         { printf("[LEX] Token: FINSI\n"); return FINSI; }

 /* Instructions de boucle */
"tantque"       { printf("[LEX] Token: TANTQUE\n"); return TANTQUE; }
"faire"         { printf("[LEX] Token: FAIRE\n"); return FAIRE; }
"fintantque"    { printf("[LEX] Token: FINTANTQUE\n"); return FINTANTQUE; }

"pour"          { printf("[LEX] Token: POUR\n"); return POUR; }
"de"            { printf("[LEX] Token: DE\n"); return DE; }
"a"             { printf("[LEX] Token: A\n"); return A; }
"pas"           { printf("[LEX] Token: PAS\n"); return PAS; }
"finpour"       { printf("[LEX] Token: FINPOUR\n"); return FINPOUR; }

 /* Instructions de fonction */
"fonction"      { printf("[LEX] Token: FONCTION\n"); return FONCTION; }
"retourner"     { printf("[LEX] Token: RETOURNER\n"); return RETOURNER; }
"finfonction"   { printf("[LEX] Token: FINFONCTION\n"); return FINFONCTION; }


 /* Valeurs booléennes */
"vrai"          { printf("[LEX] Token: VRAI\n"); return VRAI; }
"faux"          { printf("[LEX] Token: FAUX\n"); return FAUX; }

 /* Opérateurs logiques */
"et"            { printf("[LEX] Token: ET\n"); return ET; }
"ou"            { printf("[LEX] Token: OU\n"); return OU; }
"non"           { printf("[LEX] Token: NON\n"); return NON; }

 /* Opérateurs de comparaison */
"=="            { printf("[LEX] Token: EGAL\n"); return EGAL; }
"!="            { printf("[LEX] Token: DIFFERENT\n"); return DIFFERENT; }
"<="            { printf("[LEX] Token: INFEGAL\n"); return INFEGAL; }
">="            { printf("[LEX] Token: SUPEGAL\n"); return SUPEGAL; }
"<"             { printf("[LEX] Token: INFERIEUR\n"); return INFERIEUR; }
">"             { printf("[LEX] Token: SUPERIEUR\n"); return SUPERIEUR; }

 /* Opérateur d'affectation */
"="             { printf("[LEX] Token: AFFECTATION\n"); return AFFECTATION; }

 /* Opérateurs arithmétiques */
"+"             { printf("[LEX] Token: PLUS\n"); return PLUS; }
"-"             { printf("[LEX] Token: MOINS\n"); return MOINS; }
"*"             { printf("[LEX] Token: FOIS\n"); return FOIS; }
"/"             { printf("[LEX] Token: DIVISE\n"); return DIVISE; }
"%"             { printf("[LEX] Token: MODULO\n"); return MODULO; }

 /* Délimiteurs */
"("             { printf("[LEX] Token: PAREN_OUV\n"); return PAREN_OUV; }
")"             { printf("[LEX] Token: PAREN_FERM\n"); return PAREN_FERM; }
"{"             { printf("[LEX] Token: ACCOLADE_OUV\n"); return ACCOLADE_OUV; }
"}"             { printf("[LEX] Token: ACCOLADE_FERM\n"); return ACCOLADE_FERM; }
"["             { printf("[LEX] Token: CROCHET_OUV\n"); return CROCHET_OUV; }
"]"             { printf("[LEX] Token: CROCHET_FERM\n"); return CROCHET_FERM; }
";"             { printf("[LEX] Token: POINT_VIRGULE\n"); return POINT_VIRGULE; }
","             { printf("[LEX] Token: VIRGULE\n"); return VIRGULE; }

 /* Nombres - IMPORTANT: AVANT les identificateurs */
[0-9]+                  { 
    printf("[LEX] Nombre entier: %s\n", yytext); 
    yylval.entier = atoi(yytext); 
    return NOMBRE_ENTIER; 
}

[0-9]+\.[0-9]+         { 
    printf("[LEX] Nombre réel: %s\n", yytext); 
    yylval.reel = atof(yytext); 
    return NOMBRE_REEL; 
}

 /* Chaînes de caractères */
\"([^"\\]|\\.)*\"      { 
    printf("[LEX] Chaîne: %s\n", yytext); 
    yylval.chaine = strdup(yytext); 
    return CHAINE_LITT; 
}

 /* Identificateurs - APRÈS les mots-clés et nombres */
[a-zA-Z_][a-zA-Z0-9_]*  { 
    printf("[LEX] Identificateur: %s\n", yytext); 
    yylval.chaine = strdup(yytext); 
    return IDENTIFICATEUR; 
}

 /* Commentaires */
"//".*$         { printf("[LEX] Commentaire ignoré\n"); }
"/*"([^*]|\*+[^*/])*\*+"/"  { printf("[LEX] Commentaire bloc ignoré\n"); }

 /* Espaces et retours à la ligne */
[ \t]+          { /* ignorer les espaces et tabulations */ }
\n              { printf("[LEX] Nouvelle ligne %d\n", yylineno); }

 /* Caractères non reconnus */
.               { 
    printf("[LEX] ERREUR: Caractère non reconnu '%s' à la ligne %d\n", yytext, yylineno); 
    return yytext[0]; 
}

%%
