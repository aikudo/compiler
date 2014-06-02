%{
// $Id: parser.y,v 1.1 2014-06-02 04:56:59-07 - - $

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

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
%token POS NEG NEWARRAY NEWSTRING TYPEID FIELD
%token ORD CHR ROOT

%token INDEX

%right IF ELSE '=' UNEG UPOS ORD CHR '!'
%left '+' '-' '*' '/' '%'
%left EQ NE LT LE GT GE
%left '[' '.'



%start start

%%

start : program { yyparse_astree = $1; }
      ;

program : program struct { $$ = adopt1 ($1, $2); }
        | program listexpr { $$ =adopt1 ($1, $2); }
        /*
        | program function { $$ = adopt1 ($1, $2); }
        | program statement { $$ = adopt1 ($1, $2); }
        | program error '}' { $$ = adopt1 ($1, $2); }
        | program error ';' { $$ = adopt1 ($1, $2); }
        */
        |   /*empty */     { $$ = new_parseroot (); }
        ;

struct    : STRUCT IDENT '{' '}'             { adopt1 ($1, csym($2,TYPEID)); }
          | fieldlist '}'                    { $$ = $1; } 
          ;

fieldlist : STRUCT IDENT '{' fielddecl      { $$ = adopt2 ($1, csym($2,TYPEID), $4); }
          | fieldlist fielddecl             { $$ = adopt1 ($1, $2); }
          ;

fielddecl : basetype IDENT ';'              { $$ = adopt1 ($1, csym($2,FIELD)); }
//        | basetype ARR IDENT ';'          { $$ = adopt1 ($1, $2, csym($3,FIELD)); }
          ;

basetype : VOID { $$ = $1;}
         | BOOL { $$ = $1;}
         | CHAR { $$ = $1;}
         | INT { $$ = $1;}
         | STRING { $$ = $1;}
         | IDENT  { $$ = csym($1, TYPEID); }
         ;

listexpr : ';' { $$ = $1;  printf("never get here\n"); }
         | expr ';' {$$ = adopt1($1, $2); }
         ;

expr : expr '+' expr { $$ = adopt2($2, $1, $3); }
     | expr '-' expr { $$ = adopt2($2, $1, $3); }
     | expr '*' expr { $$ = adopt2($2, $1, $3); }
     | expr '/' expr { $$ = adopt2($2, $1, $3); }
     | expr '%' expr { $$ = adopt2($2, $1, $3); }
     | '-' expr %prec UNEG   { $$ = adopt1(csym($1, NEG), $2); }
     | '+' expr %prec UPOS   { $$ = adopt1(csym($1, POS), $2); }
     | '!' expr      { $$ = adopt1($1, $2); }
     | ORD expr      { $$ = adopt1($1, $2); }
     | CHR expr      { $$ = adopt1($1, $2); }
     | allocator { $$ = $1; }
     | call { $$ = $1; }
     | '(' expr ')' { $$ = $1; }
     | variable { $$ = $1; }
     | constant { $$ = $1; }
     ;


//similar like struct style 
call : IDENT '(' ')'    { $$ = adopt1(csym($2, CALL), $1); }
     | callist ')'      { $$ = $1;}
     ;

callist : IDENT '(' expr { $$ = adopt2(csym($2, CALL), $1, $3); }
        | callist ',' expr { $$ = adopt1( $1, $3); }
        ;



allocator : NEW IDENT '(' ')' { $$= adopt1($1, csym($2, TYPEID)); }
          | NEW STRING '(' expr ')'  { $$ = adopt1( csym($1, NEWSTRING), $4); }
          | NEW basetype '[' expr ']' { $$ = adopt2( csym($1, NEWARRAY), $2, $4); }
          ;


variable : IDENT  {$$ = $1; }
         | expr '[' expr ']'  { $$ = adopt2(csym($2,INDEX), $1, $3); } //postfix
         | expr '.' IDENT  { $$ = adopt2( $2, $1, csym($3, FIELD)) ;}
         ;

constant : INTCON {$$ = $1; }
         | CHARCON {$$ = $1; }
         | STRINGCON {$$ = $1;}
         | FALSE {$$ = $1; }
         | TRUE {$$ = $1; }
         | NIL {$$ = $1; }
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




