%{
// $Id: parser.y,v 1.1 2014-05-28 19:42:39-07 - - $

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
%token IDENT INTCON CHARCON STRINGCON

%token BLOCK CALL IFELSE INITDECL
%token POS NEG NEWARRAY TYPEID FIELD
%token ORD CHR ROOT

%start start

%%

start : program { yyparse_astree = $1; }
      ;

program : program struct { $$ = adopt1 ($1, $2); }
        /*
        | program function { $$ = adopt1 ($1, $2); }
        | program statement { $$ = adopt1 ($1, $2); }
        | program error '}' { $$ = adopt1 ($1, $2); }
        | program error ';' { $$ = adopt1 ($1, $2); }
        */
        |   /*empty */     { $$ = new_parseroot (); }
        ;

struct    : STRUCT IDENT '{' fieldlist '}'  { adopt2 ($1, $2, $4); }
        //  | STRUCT IDENT '{' '}'            { adopt1 ($1, $2); }
          //adopt1sym( $1, $2, TYPEID ); }
          ;

/*
fieldlist : STRUCT IDENT '{' fielddecl      { $$ =  adopt1$1;$3)  }
          | fieldlist fielddecl             { $$ = adopt1 ($1, $2); }
          ;
*/

fieldlist : basetype IDENT ';'              { $$ = adopt1 ($1, $2); }
          |
          ;

basetype : VOID { $$ = $1;}
         | BOOL { $$ = $1;}
         | CHAR { $$ = $1;}
         | INT { $$ = $1;}
         | STRING { $$ = $1;}
         | IDENT  { $$ = changesym($1, TYPEID); }
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




