%{
// $Id: parser.y,v 1.1 2014-06-08 00:52:12-07 - - $

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
%token FALSE TRUE NIL NEW ARRAY
%token EQ NE LT LE GT GE
%token IDENT INTCON CHARCON STRINGCON

%token BLOCK CALL IFELSE INITDECL
%token POS NEG NEWARRAY NEWSTRING TYPEID FIELD
%token ORD CHR ROOT

%token INDEX RETURNVOID VARDECL DECLID
%token FUNCTION PARAM PROTOTYPE

//http://web.mit.edu/gnu/doc/html/bison_8.html
%right THEN ELSE
%right '='
%left EQ NE LT LE GT GE
%left '+' '-'
%left '*' '/' '%'
%right UPOS UNEG '!' ORD CHR
%left '[' '.'
%nonassoc NEW
%nonassoc '('

%start start

%%

start : program { yyparse_astree = $1; }
      ;

program : program struct         { $$ = adopt1 ($1, $2); }
        | program statement      { $$ =adopt1 ($1, $2); }
        | program function       { $$ =adopt1 ($1, $2); }
        | program error '}'      { $$ = adopt1 ($1, $2); fprintf(stderr, "CAUGHT ERROR }\n"); }
        | program error ';'      { $$ = adopt1 ($1, $2); fprintf(stderr, "CAUGHT ERROR ;\n"); }
        |   /*empty */           { $$ = new_parseroot (); }
        ;

struct    : STRUCT IDENT '{' '}'            { adopt1 ($1, csym($2,TYPEID)); }
          | fieldlist '}'                   { $$ = $1; } 
          ;

fieldlist : STRUCT IDENT '{' fielddecl ';'  { $$ = adopt2 ($1, csym($2,TYPEID), $4); }
          | fieldlist fielddecl ';'         { $$ = adopt1 ($1, $2); }
          ;

fielddecl : basetype IDENT                  { $$ = adopt1 ($1, csym($2,FIELD)); }
          | basetype ARRAY IDENT            { $$ = adopt2 ($2, $1, csym($3,FIELD)); }
          ;

basetype : VOID   { $$ = $1;}
         | BOOL   { $$ = $1;}
         | CHAR   { $$ = $1;}
         | INT    { $$ = $1;}
         | STRING { $$ = $1;}
         | IDENT  { $$ = csym($1, TYPEID); }
         ;

function : identdecl  params ')' funblock    { $$ =  adopt3( clone($1, FUNCTION), $1, $2, $4); }
         | identdecl  params ')' ';'         { $$ =  adopt2( clone($1, PROTOTYPE), $1, $2); }
         | identdecl  '(' ')' funblock       { $$ =  adopt3( clone($1, FUNCTION), $1, csym($2, PARAM), $4); }
         | identdecl  '(' ')' ';'            { $$ =  adopt2( clone($1, PROTOTYPE), $1, csym($2, PARAM) ); }
         ;

// one or more params with root on '('
params   : '(' identdecl        { $$ = adopt1 ( csym($1, PARAM), $2); }
         | params ',' identdecl { $$ = adopt1 ($1,  $3); }
         ;

funblock : '{' '}'       { $$ = csym($1, BLOCK); }
         | blocklist '}' {$$ = $1; }
         ;

statement : block     { $$ = $1; }
          | vardecl   { $$ = $1; }
          | while     { $$ = $1; }
          | ifelse    { $$ = $1; }
          | return    { $$ = $1; }
          | expr ';'  { $$ = $1; }
          ;

block : '{' '}'  { $$ = csym($1, BLOCK); }
      | blocklist '}' {$$ = $1; }
      //vacuous block: take it or drop?
      | ';'           { /* empty */ }
      ;

blocklist : '{' statement       { $$ = adopt1( csym($1, BLOCK), $2); }
          | blocklist statement { $$ = adopt1($1, $2); }
          ;

identdecl : basetype IDENT       { $$ = adopt1 ($1, csym($2, DECLID) ); }
          | basetype ARRAY IDENT { $$ = adopt2 ($2, $1, csym($3, DECLID) ); }
          ;

vardecl : identdecl '=' expr ';'     { $$ = adopt2( csym($2, VARDECL), $1, $3) ;}

while : WHILE '(' expr ')' statement { $$ = adopt2($1, $3, $5); }
      ;

ifelse : IF '(' expr ')' statement %prec THEN     { $$ = adopt2($1, $3, $5); }
ifelse : IF '(' expr ')' statement ELSE statement { $$ = adopt3(csym($1, IFELSE), $3, $5, $7); }

return : RETURN expr ';'   { $$ = adopt1 ($1, $2); }
       | RETURN ';'        { $$ = csym($1, RETURNVOID); }

expr : expr '=' expr       { $$ = adopt2($2, $1, $3); }
     | expr EQ expr        { $$ = adopt2($2, $1, $3); }
     | expr NE expr        { $$ = adopt2($2, $1, $3); }
     | expr LT expr        { $$ = adopt2($2, $1, $3); }
     | expr LE expr        { $$ = adopt2($2, $1, $3); }
     | expr GT expr        { $$ = adopt2($2, $1, $3); }
     | expr GE expr        { $$ = adopt2($2, $1, $3); }
     | expr '+' expr       { $$ = adopt2($2, $1, $3); }
     | expr '-' expr       { $$ = adopt2($2, $1, $3); }
     | expr '*' expr       { $$ = adopt2($2, $1, $3); }
     | expr '/' expr       { $$ = adopt2($2, $1, $3); }
     | expr '%' expr       { $$ = adopt2($2, $1, $3); }
     | '-' expr %prec UNEG { $$ = adopt1(csym($1, NEG), $2); }
     | '+' expr %prec UPOS { $$ = adopt1(csym($1, POS), $2); }
     | '!' expr            { $$ = adopt1($1, $2); }
     | ORD expr            { $$ = adopt1($1, $2); }
     | CHR expr            { $$ = adopt1($1, $2); }
     | allocator           { $$ = $1; }
     | call                { $$ = $1; }
     | '(' expr ')'        { $$ = $2; }
     | variable            { $$ = $1; } // memory access
     | constant            { $$ = $1; }
     ;

call : IDENT '(' ')'    { $$ = adopt1(csym($2, CALL), $1); }
     | callist ')'      { $$ = $1;}
     ;

callist : IDENT '(' expr              { $$ = adopt2(csym($2, CALL), $1, $3); }
        | callist ',' expr            { $$ = adopt1( $1, $3); }
        ;

allocator : NEW IDENT '(' ')'         { $$ = adopt1($1, csym($2, TYPEID)); }
          | NEW STRING '(' expr ')'   { $$ = adopt1( csym($1, NEWSTRING), $4); }
          | NEW basetype '[' expr ']' { $$ = adopt2( csym($1, NEWARRAY), $2, $4); }
          ;


variable : IDENT              { $$ = $1; }
         | expr '[' expr ']'  { $$ = adopt2(csym($2,INDEX), $1, $3); } //postfix
         | expr '.' IDENT     { $$ = adopt2( $2, $1, csym($3, FIELD)) ;}
         ;

constant : INTCON      { $$ = $1; }
         | CHARCON     { $$ = $1; }
         | STRINGCON   { $$ = $1; }
         | FALSE       { $$ = $1; }
         | TRUE        { $$ = $1; }
         | NIL         { $$ = $1; }
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




