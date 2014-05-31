%{
// $Id: parser.y,v 1.4 2014-05-30 23:24:09-07 - - $

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "lyutils.h"
#include "astree.h"
#include "astree.rep.h"

#define YYDEBUG 1
#define YYERROR_VERBOSE 1
#define YYPRINT yyprint
#define YYMALLOC yycalloc

static void *yycalloc (size_t size);



%}

%debug
%defines
%error-verbose
%token-table

%token VOID BOOL CHAR INT STRING
%token IF ELSE WHILE RETURN STRUCT
%token FALSE TRUE NIL NEW ARR
%token EQ NE LT LE GT GE
%token ID INTCON CHARCON STRINGCON

%token BLOCK CALL IFELSE INITDECL
%token POS NEG NEWARR TYPEID FIELD
%token ORD CHR ROOT

%start start

%%
start    : program               { yyparse_astree = $1;  }

program  : program structdef     { $$ = adopt1 ($1, $2); }
         | program func          { $$ = adopt1 ($1, $2); }
         | program stmt          { $$ = adopt1 ($1, $2); }
         | program error '}'     { $$ = $1; }
         | program error ';'     { $$ = $1; }
         |                       { $$ = new_parseroot(); }

/*'struct' ID '{' [field ';' ]* '}' */
structdef | STRUCT ID '{' '}'



fielddecl : basetype ID { /*ID -> FIELD */ }
          | basetype ARR ID {}
          ;


fdlist   : /* empty */
         | fdlist
         | fddlec ';'
         ;

fddecl   : basetype ID { /*switch ID -> FIELD */ }
         | basetype ARR ID { /*switch ID -> FIELD */ }
         ;

basetype : VOID { $$ = $1; }
         | BOOL { $$ = $1; }
         | CHAR { $$ = $1; }
         | INT  { $$ = $1; }
         | ID   { /*user-defined type */ }
         ;

identdecl : basetype ID {}
          | basetype ARR ID {}
          ;

constant  : INTCON             { $$ = $1; }
          | CHARCON            { $$ = $1; }
          | STRINGCON          { $$ = $1; }
          | FALSE              { $$ = $1; }
          | TRUE               { $$ = $1; }
          | NIL                { $$ = $1; }
          ;

%%

const char *get_yytname (int symbol) {
   return yytname [YYTRANSLATE (symbol)];
}


static void *yycalloc (size_t size) {
   void *result = calloc (1, size);
   assert (result != NULL);
   return result;
}




