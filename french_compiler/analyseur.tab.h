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
    TABLEAU = 266,                 /* TABLEAU  */
    LIRE = 267,                    /* LIRE  */
    ECRIRE = 268,                  /* ECRIRE  */
    AFFICHER = 269,                /* AFFICHER  */
    SI = 270,                      /* SI  */
    ALORS = 271,                   /* ALORS  */
    SINON = 272,                   /* SINON  */
    FINSI = 273,                   /* FINSI  */
    TANTQUE = 274,                 /* TANTQUE  */
    FAIRE = 275,                   /* FAIRE  */
    FINTANTQUE = 276,              /* FINTANTQUE  */
    POUR = 277,                    /* POUR  */
    DE = 278,                      /* DE  */
    A = 279,                       /* A  */
    PAS = 280,                     /* PAS  */
    FINPOUR = 281,                 /* FINPOUR  */
    FONCTION = 282,                /* FONCTION  */
    RETOURNER = 283,               /* RETOURNER  */
    FINFONCTION = 284,             /* FINFONCTION  */
    VRAI = 285,                    /* VRAI  */
    FAUX = 286,                    /* FAUX  */
    ET = 287,                      /* ET  */
    OU = 288,                      /* OU  */
    NON = 289,                     /* NON  */
    EGAL = 290,                    /* EGAL  */
    DIFFERENT = 291,               /* DIFFERENT  */
    INFEGAL = 292,                 /* INFEGAL  */
    SUPEGAL = 293,                 /* SUPEGAL  */
    INFERIEUR = 294,               /* INFERIEUR  */
    SUPERIEUR = 295,               /* SUPERIEUR  */
    AFFECTATION = 296,             /* AFFECTATION  */
    PLUS = 297,                    /* PLUS  */
    MOINS = 298,                   /* MOINS  */
    FOIS = 299,                    /* FOIS  */
    DIVISE = 300,                  /* DIVISE  */
    MODULO = 301,                  /* MODULO  */
    PAREN_OUV = 302,               /* PAREN_OUV  */
    PAREN_FERM = 303,              /* PAREN_FERM  */
    ACCOLADE_OUV = 304,            /* ACCOLADE_OUV  */
    ACCOLADE_FERM = 305,           /* ACCOLADE_FERM  */
    CROCHET_OUV = 306,             /* CROCHET_OUV  */
    CROCHET_FERM = 307,            /* CROCHET_FERM  */
    POINT_VIRGULE = 308,           /* POINT_VIRGULE  */
    VIRGULE = 309,                 /* VIRGULE  */
    QUESTION = 310,                /* QUESTION  */
    DEUX_POINTS = 311,             /* DEUX_POINTS  */
    INCREMENT = 312,               /* INCREMENT  */
    DECREMENT = 313,               /* DECREMENT  */
    MOINS_UNAIRE = 314             /* MOINS_UNAIRE  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 519 "analyseur.y"

    int entier;
    double reel;
    char *chaine;

#line 129 "analyseur.tab.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;


int yyparse (void);


#endif /* !YY_YY_ANALYSEUR_TAB_H_INCLUDED  */
