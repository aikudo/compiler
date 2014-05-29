#ifndef __LYUTILS_H__
#define __LYUTILS_H__

// Lex and Yacc interface utility.

#include <stdio.h>

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
int yylval_token (int symbol);

void scanner_include (void);
void scanner_destroy (void);

#define YYSTYPE astree //set to astree instead its default int
#include "yyparse.h"

// LINTED(static unused)
RCSH(LYUTILS_H,"$Id: lyutils.h,v 1.3 2014-05-29 12:39:22-07 - - $")
#endif
