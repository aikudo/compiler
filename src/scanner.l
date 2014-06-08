%{
// $Id: scanner.l,v 1.1 2014-06-08 00:52:12-07 - - $

//#include "auxlib.h"
#include "lyutils.h"
#include "yyparse.h"

#define YY_NO_INPUT 1
#define YY_USER_ACTION  { scanner_useraction (); }
#define IGNORE(THING)   { }

%}

%option 8bit
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
MISSQUOTESTR    \"([^\\"\n]|\\[\\'"0nt])*
BADESCAPESTR    \"([^\\"\n]|\\.?)*\"
BADSTR          {MISSQUOTESTR}|{BADESCAPESTR}
MISSQUOTECHR    '([^\\'\n]|\\[\\'"0nt])
BADESCAPECHR    '([^\\'\n]|\\.?)'
BADCHAR         {MISSQUOTECHR}|{BADESCAPECHR}

%%

"#".*        { scanner_include();                   }
[ \t]+       { IGNORE (white space)                 }
\n           { scanner_newline();                   }


"[]"         { return yylval_token (ARRAY);     } 
"=="         { return yylval_token (EQ);        } 
"!="         { return yylval_token (NE);        } 
"<="         { return yylval_token (LE);        } 
">="         { return yylval_token (GE);        } 
"<"          { return yylval_token (LT);        } 
">"          { return yylval_token (GT);        } 
"("          { return yylval_token ('(');           } 
")"          { return yylval_token (')');           } 
"["          { return yylval_token ('[');           } 
"]"          { return yylval_token (']');           } 
"{"          { return yylval_token ('{');           } 
"}"          { return yylval_token ('}');           } 
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
"|"          { return yylval_token ('|');           } 
"?"          { return yylval_token ('?');           } 
":"          { return yylval_token (':');           } 
"&"          { return yylval_token ('&');           } 

"bool"       { return yylval_token (BOOL);      } 
"char"       { return yylval_token (CHAR);      } 
"chr"        { return yylval_token (ORD);       } 
"else"       { return yylval_token (ELSE);      } 
"false"      { return yylval_token (FALSE);     } 
"if"         { return yylval_token (IF);        } 
"int"        { return yylval_token (INT);       } 
"new"        { return yylval_token (NEW);       } 
"null"       { return yylval_token (NIL);       } 
"ord"        { return yylval_token (ORD);       } 
"return"     { return yylval_token (RETURN);    } 
"string"     { return yylval_token (STRING);    } 
"struct"     { return yylval_token (STRUCT);    } 
"true"       { return yylval_token (TRUE);      } 
"void"       { return yylval_token (VOID);      } 
"while"      { return yylval_token (WHILE);     } 

{NUMBER}     { return yylval_token (INTCON);    }
{IDENT}      { return yylval_token (IDENT);     }
{STR_CONST}  { return yylval_token (STRINGCON); }
{CHAR_CONST} { return yylval_token (CHARCON);   }

{BADIDENT}   { scanner_badtoken (yytext);
               return yylval_token (IDENT);     }

{BADCHAR}    { scanner_badtoken (yytext);
               return yylval_token (CHARCON);   }

{BADSTR}     { scanner_badtoken (yytext);
               return yylval_token (STRINGCON); }

.            { scanner_badchar (*yytext);           }

%%

//
//int yywrap (void){
//   yy_delete_buffer (YY_CURRENT_BUFFER);
//   if (yy_flex_debug) eprintf ("yywrap() return %d\n", true);
//   return 1;
//}

// LINTED(static unused)
RCSC(SCANNER_L,"$Id: scanner.l,v 1.1 2014-06-08 00:52:12-07 - - $")

