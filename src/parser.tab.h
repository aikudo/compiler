/* A Bison parser, made by GNU Bison 2.5.  */

/* Bison interface for Yacc-like parsers in C
   
      Copyright (C) 1984, 1989-1990, 2000-2011 Free Software Foundation, Inc.
   
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
     VOID = 258,
     BOOL = 259,
     CHAR = 260,
     INT = 261,
     STRING = 262,
     IF = 263,
     ELSE = 264,
     WHILE = 265,
     RETURN = 266,
     STRUCT = 267,
     FALSE = 268,
     TRUE = 269,
     NIL = 270,
     NEW = 271,
     ARR = 272,
     EQ = 273,
     NE = 274,
     LT = 275,
     LE = 276,
     GT = 277,
     GE = 278,
     IDENT = 279,
     INTCON = 280,
     CHARCON = 281,
     STRINGCON = 282,
     BLOCK = 283,
     CALL = 284,
     IFELSE = 285,
     INITDECL = 286,
     POS = 287,
     NEG = 288,
     NEWARRAY = 289,
     NEWSTRING = 290,
     TYPEID = 291,
     FIELD = 292,
     ORD = 293,
     CHR = 294,
     ROOT = 295,
     INDEX = 296,
     RETURNVOID = 297,
     VARDECL = 298,
     DECLID = 299,
     UPOS = 300,
     UNEG = 301
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef int YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE yylval;


