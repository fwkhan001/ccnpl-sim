
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton interface for Bison's Yacc-like parsers in C
   
      Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

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


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     TOPOLOGY_LINE = 258,
     MODEL_LINE = 259,
     NODES_LINE = 260,
     EDGES_LINE = 261,
     TOK_AS_NODE = 262,
     TOK_RT_NODE = 263,
     TOK_E_AS = 264,
     TOK_E_RT = 265,
     TOK_U = 266,
     INT = 267,
     DOUBLE = 268,
     CHAR = 269
   };
#endif
/* Tokens.  */
#define TOPOLOGY_LINE 258
#define MODEL_LINE 259
#define NODES_LINE 260
#define EDGES_LINE 261
#define TOK_AS_NODE 262
#define TOK_RT_NODE 263
#define TOK_E_AS 264
#define TOK_E_RT 265
#define TOK_U 266
#define INT 267
#define DOUBLE 268
#define CHAR 269




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 1676 of yacc.c  */
#line 25 "brite_parser.yy"

  int ival;
  double dval;
  char cval;



/* Line 1676 of yacc.c  */
#line 88 "brite_parser.h"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE yylval;


