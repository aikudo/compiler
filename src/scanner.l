%{
// $Id: scanner.l,v 1.2 2014-05-28 19:42:39-07 - - $

#include <stdlib.h>
#include <stdbool.h>

#include "auxlib.h"
#include "lyutils.h"
#include "parser.h"

#define YY_USER_ACTION  { scanner_useraction (); }
#define IGNORE(THING)   { }

%}

%option 8bit
%option backup
%option debug
%option ecs
%option nodefault
%option nounput
%option noyywrap
%option perf-report
%option verbose
%option warn

LETTER          [A-Za-z_]
DIGIT           [0-9]
NUMBER          {DIGIT}+
CHAR_CONST      '([^\\'\n]|\\[\\'"0nt])'
STR_CONST       \"([^\\"\n]|\\[\\'"0nt])*\"
IDENT           {LETTER}({LETTER}|{DIGIT})*

BADIDENT        {DIGIT}+{IDENT}
BADCHAR         '([^\\'\n]|\\[\\'"0nt]?)[^']?
BADSTR          \"([^\\"\n]|\\[\\'"0nt])*[^\"]?
BADTOK          {BADIDENT}|{BADCHAR}|{BADSTR}
/*BADCHAR and BADSTR might be wrong */

%%

"#".*        { scanner_include();                   }
[ \t]+       { IGNORE (white space)                 }
\n           { scanner_newline();                   }


"[]"         { return yylval_token (TOK_ARRAY);     } 
"=="         { return yylval_token (TOK_EQ);        } 
"!="         { return yylval_token (TOK_NE);        } 
"<="         { return yylval_token (TOK_LE);        } 
">="         { return yylval_token (TOK_GE);        } 
"<"          { return yylval_token (TOK_LT);        } 
">"          { return yylval_token (TOK_GT);        } 
"("          { return yylval_token ('(');           } 
")"          { return yylval_token (')');           } 
"["          { return yylval_token ('[');           } 
"]"          { return yylval_token (']');           } 
"{"          { return yylval_token ('{');           } 
"} "         { return yylval_token ('}');           } 
";"          { return yylval_token (';');           } 
","          { return yylval_token (',');           } 
"."          { return yylval_token ('.');           } 
"+"          { return yylval_token ('+');           } 
"-"          { return yylval_token ('-');           } 
"*"          { return yylval_token ('*');           } 
"/"          { return yylval_token ('/');           } 
"%"          { return yylval_token ('%');           } 
"!"          { return yylval_token ('!');           } 
"="          { return yylval_token ('=');           } 

"bool"       { return yylval_token (TOK_BOOL);      } 
"char"       { return yylval_token (TOK_CHAR);      } 
"chr"        { return yylval_token (TOK_ORD);       } 
"else"       { return yylval_token (TOK_ELSE);      } 
"false"      { return yylval_token (TOK_FALSE);     } 
"if"         { return yylval_token (TOK_IF);        } 
"int"        { return yylval_token (TOK_INT);       } 
"new"        { return yylval_token (TOK_NEW);       } 
"null"       { return yylval_token (TOK_NULL);      } 
"ord"        { return yylval_token (TOK_ORD);       } 
"return"     { return yylval_token (TOK_RETURN);    } 
"string"     { return yylval_token (TOK_STRING);    } 
"struct"     { return yylval_token (TOK_STRUCT);    } 
"true"       { return yylval_token (TOK_TRUE);      } 
"void"       { return yylval_token (TOK_VOID);      } 
"while"      { return yylval_token (TOK_WHILE);     } 

{NUMBER}     { return yylval_token (TOK_INTCON);    }
{IDENT}      { return yylval_token (TOK_IDENT);     }
{STR_CONST}  { return yylval_token (TOK_STRINGCON); }
{CHAR_CONST} { return yylval_token (TOK_CHARCON);   }

{BADIDENT}   { scanner_badtoken (yytext);
               return yylval_token (TOK_IDENT);     }

{BADCHAR}    { scanner_badtoken (yytext);
               return yylval_token (TOK_CHARCON);   }

{BADSTR}     { scanner_badtoken (yytext);
               return yylval_token (TOK_STRINGCON); }

.            { scanner_badchar (*yytext);           }

%%

//
//int yywrap (void){
//   yy_delete_buffer (YY_CURRENT_BUFFER);
//   if (yy_flex_debug) eprintf ("yywrap() return %d\n", true);
//   return 1;
//}

// LINTED(static unused)
RCSC(SCANNER_L,"$Id: scanner.l,v 1.2 2014-05-28 19:42:39-07 - - $")

