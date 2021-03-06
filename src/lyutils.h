#ifndef __LYUTILS_H__
#define __LYUTILS_H__

// Lex and Yacc interface utility.

#include <stdio.h>

#include "stringtable.h"
#include "astree.h"
#include "auxlib.h"

#define YYEOF 0
//#define __cplusplus 0

extern FILE *yyin;
extern astree yyparse_astree;
extern int yyin_linenr;
extern char *yytext;
extern int yy_flex_debug;
extern int yydebug;
extern int yyleng;
extern hashtable *stringset;

int yylex (void);
int yyparse (void);
void yyerror (const char *message);
const char *get_yytname (int symbol);

char *scanner_filename (int filenr);
void scanner_newfilename (char *filename);
void scanner_badchar (unsigned char bad);
void scanner_badtoken (char *lexeme);
void scanner_newline (void);
void scanner_setecho (bool echoflag);
void scanner_useraction (void);

astree new_parseroot (void);
astree clone (astree src, int symbol);
int yylval_token (int symbol);

void scanner_include (void);
void scanner_destroy (void);
char *getfilename (int);
void dumptok(FILE *);

#define YYSTYPE astree //set to astree instead its default int
#include "yyparse.h"

// LINTED(static unused)
RCSH(LYUTILS_H,"$Id: lyutils.h,v 1.1 2014-06-10 00:44:31-07 - - $")
#endif
