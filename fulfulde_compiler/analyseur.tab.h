/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison interface for Yacc-like parsers in C

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

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

#ifndef YY_YY_ANALYSEUR_TAB_H_INCLUDED
# define YY_YY_ANALYSEUR_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    NOMBRE_ENTIER = 258,           /* NOMBRE_ENTIER  */
    NOMBRE_REEL = 259,             /* NOMBRE_REEL  */
    IDENTIFICATEUR = 260,          /* IDENTIFICATEUR  */
    CHAINE_LITT = 261,             /* CHAINE_LITT  */
    ENTIER = 262,                  /* ENTIER  */
    REEL = 263,                    /* REEL  */
    CHAINE = 264,                  /* CHAINE  */
    BOOLEEN = 265,                 /* BOOLEEN  */
    LIRE = 266,                    /* LIRE  */
    ECRIRE = 267,                  /* ECRIRE  */
    AFFICHER = 268,                /* AFFICHER  */
    SI = 269,                      /* SI  */
    ALORS = 270,                   /* ALORS  */
    SINON = 271,                   /* SINON  */
    FINSI = 272,                   /* FINSI  */
    TANTQUE = 273,                 /* TANTQUE  */
    FAIRE = 274,                   /* FAIRE  */
    FINTANTQUE = 275,              /* FINTANTQUE  */
    POUR = 276,                    /* POUR  */
    DE = 277,                      /* DE  */
    A = 278,                       /* A  */
    PAS = 279,                     /* PAS  */
    FINPOUR = 280,                 /* FINPOUR  */
    FONCTION = 281,                /* FONCTION  */
    RETOURNER = 282,               /* RETOURNER  */
    FINFONCTION = 283,             /* FINFONCTION  */
    VRAI = 284,                    /* VRAI  */
    FAUX = 285,                    /* FAUX  */
    ET = 286,                      /* ET  */
    OU = 287,                      /* OU  */
    NON = 288,                     /* NON  */
    EGAL = 289,                    /* EGAL  */
    DIFFERENT = 290,               /* DIFFERENT  */
    INFEGAL = 291,                 /* INFEGAL  */
    SUPEGAL = 292,                 /* SUPEGAL  */
    INFERIEUR = 293,               /* INFERIEUR  */
    SUPERIEUR = 294,               /* SUPERIEUR  */
    AFFECTATION = 295,             /* AFFECTATION  */
    PLUS = 296,                    /* PLUS  */
    MOINS = 297,                   /* MOINS  */
    FOIS = 298,                    /* FOIS  */
    DIVISE = 299,                  /* DIVISE  */
    MODULO = 300,                  /* MODULO  */
    PAREN_OUV = 301,               /* PAREN_OUV  */
    PAREN_FERM = 302,              /* PAREN_FERM  */
    ACCOLADE_OUV = 303,            /* ACCOLADE_OUV  */
    ACCOLADE_FERM = 304,           /* ACCOLADE_FERM  */
    POINT_VIRGULE = 305,           /* POINT_VIRGULE  */
    VIRGULE = 306,                 /* VIRGULE  */
    MOINS_UNAIRE = 307             /* MOINS_UNAIRE  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 431 "analyseur.y"

    int entier;
    double reel;
    char *chaine;

#line 122 "analyseur.tab.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;


int yyparse (void);


#endif /* !YY_YY_ANALYSEUR_TAB_H_INCLUDED  */
