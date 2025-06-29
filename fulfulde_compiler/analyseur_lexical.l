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

 /* Mots-clés de types en fulfulde */
"limre"         { printf("[LEX] Token: ENTIER\n"); return ENTIER; }
"jaango"        { printf("[LEX] Token: REEL\n"); return REEL; }
"deftere"       { printf("[LEX] Token: CHAINE\n"); return CHAINE; }
"goonga"        { printf("[LEX] Token: BOOLEEN\n"); return BOOLEEN; }

 /* Instructions d'E/S en fulfulde */
"tar"           { printf("[LEX] Token: LIRE\n"); return LIRE; }
"winndude"      { printf("[LEX] Token: ECRIRE\n"); return ECRIRE; }
"hollude"       { printf("[LEX] Token: AFFICHER\n"); return AFFICHER; }

 /* Instructions conditionnelles en fulfulde */
"so"            { printf("[LEX] Token: SI\n"); return SI; }
"no"            { printf("[LEX] Token: ALORS\n"); return ALORS; }
"kono"          { printf("[LEX] Token: SINON\n"); return SINON; }
"gasii_so"      { printf("[LEX] Token: FINSI\n"); return FINSI; }

 /* Instructions de boucle en fulfulde - VERSION CORRIGÉE */
"haa_nga"       { printf("[LEX] Token: TANTQUE\n"); return TANTQUE; }
"wayde"         { printf("[LEX] Token: FAIRE\n"); return FAIRE; }
"gasii_haa"     { printf("[LEX] Token: FINTANTQUE\n"); return FINTANTQUE; }

"e_kala"        { printf("[LEX] Token: POUR\n"); return POUR; }
"iwde"          { printf("[LEX] Token: DE\n"); return DE; }
"haa"           { printf("[LEX] Token: A\n"); return A; }
"takko"         { printf("[LEX] Token: PAS\n"); return PAS; }
"gasii_e"       { printf("[LEX] Token: FINPOUR\n"); return FINPOUR; }

 /* Instructions de fonction en fulfulde */
"golle"         { printf("[LEX] Token: FONCTION\n"); return FONCTION; }
"ruttude"       { printf("[LEX] Token: RETOURNER\n"); return RETOURNER; }
"gasii_golle"   { printf("[LEX] Token: FINFONCTION\n"); return FINFONCTION; }

 /* Valeurs booléennes en fulfulde */
"gooto"         { printf("[LEX] Token: VRAI\n"); return VRAI; }
"wonaa"         { printf("[LEX] Token: FAUX\n"); return FAUX; }

 /* Opérateurs logiques en fulfulde - VERSION CORRIGÉE */
"e_kadi"        { printf("[LEX] Token: ET\n"); return ET; }
"walla"         { printf("[LEX] Token: OU\n"); return OU; }
"alaa"          { printf("[LEX] Token: NON\n"); return NON; }

 /* Opérateurs de comparaison - GARDÉS IDENTIQUES */
"=="            { printf("[LEX] Token: EGAL\n"); return EGAL; }
"!="            { printf("[LEX] Token: DIFFERENT\n"); return DIFFERENT; }
"<="            { printf("[LEX] Token: INFEGAL\n"); return INFEGAL; }
">="            { printf("[LEX] Token: SUPEGAL\n"); return SUPEGAL; }
"<"             { printf("[LEX] Token: INFERIEUR\n"); return INFERIEUR; }
">"             { printf("[LEX] Token: SUPERIEUR\n"); return SUPERIEUR; }

 /* Opérateur d'affectation - GARDÉ IDENTIQUE */
"="             { printf("[LEX] Token: AFFECTATION\n"); return AFFECTATION; }

 /* Opérateurs arithmétiques - GARDÉS IDENTIQUES */
"+"             { printf("[LEX] Token: PLUS\n"); return PLUS; }
"-"             { printf("[LEX] Token: MOINS\n"); return MOINS; }
"*"             { printf("[LEX] Token: FOIS\n"); return FOIS; }
"/"             { printf("[LEX] Token: DIVISE\n"); return DIVISE; }
"%"             { printf("[LEX] Token: MODULO\n"); return MODULO; }

 /* Délimiteurs - GARDÉS IDENTIQUES */
"("             { printf("[LEX] Token: PAREN_OUV\n"); return PAREN_OUV; }
")"             { printf("[LEX] Token: PAREN_FERM\n"); return PAREN_FERM; }
"{"             { printf("[LEX] Token: ACCOLADE_OUV\n"); return ACCOLADE_OUV; }
"}"             { printf("[LEX] Token: ACCOLADE_FERM\n"); return ACCOLADE_FERM; }
";"             { printf("[LEX] Token: POINT_VIRGULE\n"); return POINT_VIRGULE; }
","             { printf("[LEX] Token: VIRGULE\n"); return VIRGULE; }

 /* Nombres - GARDÉS IDENTIQUES */
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

 /* Chaînes de caractères - GARDÉES IDENTIQUES */
\"([^"\\]|\\.)*\"      { 
    printf("[LEX] Chaîne: %s\n", yytext); 
    yylval.chaine = strdup(yytext); 
    return CHAINE_LITT; 
}

 /* Identificateurs - GARDÉS IDENTIQUES */
[a-zA-Z_][a-zA-Z0-9_]*  { 
    printf("[LEX] Identificateur: %s\n", yytext); 
    yylval.chaine = strdup(yytext); 
    return IDENTIFICATEUR; 
}

 /* Commentaires - GARDÉS IDENTIQUES */
"//".*$         { printf("[LEX] Commentaire ignoré\n"); }
"/*"([^*]|\*+[^*/])*\*+"/"  { printf("[LEX] Commentaire bloc ignoré\n"); }

 /* Espaces et retours à la ligne - GARDÉS IDENTIQUES */
[ \t]+          { /* ignorer les espaces et tabulations */ }
\n              { printf("[LEX] Nouvelle ligne %d\n", yylineno); }

 /* Caractères non reconnus - GARDÉS IDENTIQUES */
.               { 
    printf("[LEX] ERREUR: Caractère non reconnu '%s' à la ligne %d\n", yytext, yylineno); 
    return yytext[0]; 
}

%%
