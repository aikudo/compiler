
#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "astree.rep.h"
#include "lyutils.h"
#include "auxlib.h"

astree yyparse_astree = NULL;
int scan_linenr = 1;
int scan_offset = 0;
bool scan_echo = false;

struct {
   char **filenames;
   int size;
   int last_filenr;
} filename_stack = {NULL, 0, -1};

struct {
   astree *tokens;
   int size;
   int last;
} token_list = {NULL, 0 , -1};

void addtokens(astree token){
   assert(token != NULL);
   if (token_list.tokens == NULL) {
      token_list.size = 16;
      token_list.tokens = malloc (token_list.size * sizeof (astree));
      assert (token_list.tokens != NULL);
   }else if (token_list.last == token_list.size - 1){
      token_list.size *= 2;
      token_list.tokens = realloc (token_list.tokens,
                              token_list.size * sizeof(astree));
      assert (token_list.tokens != NULL);
   }
   token_list.tokens[++token_list.last] = token;
}

char *getfilename ( int index ){
   assert (index <= filename_stack.last_filenr && index >= 0);
   return filename_stack.filenames[index];
}

char *scanner_filename (int filenr) {
   assert (filename_stack.filenames != NULL);
   return filename_stack.filenames[filenr];
}

void scanner_newfilename (char *filename) {
   assert (filename != NULL);
   if (filename_stack.filenames == NULL) {
      filename_stack.size = 16;
      filename_stack.last_filenr = -1;
      filename_stack.filenames
            = malloc (filename_stack.size * sizeof (char*));
      assert (filename_stack.filenames != NULL);
   }else if (filename_stack.last_filenr == filename_stack.size - 1) {
      filename_stack.size *= 2;
      filename_stack.filenames
            = realloc (filename_stack.filenames,
                       filename_stack.size * sizeof (char*));
      assert (filename_stack.filenames != NULL);
   }
   //char *newfilename = strdup (filename);
   char *newfilename = filename; //already strduped line 113
   assert (newfilename != NULL);
   filename_stack.filenames[++filename_stack.last_filenr]
         = newfilename;
}

void scanner_newline (void) {
   ++scan_linenr;
   scan_offset = 0;
}

void scanner_setecho (bool echoflag) {
   scan_echo = echoflag;
}


void scanner_useraction (void) {
   if (scan_echo) {
      if (scan_offset == 0) printf (";%5d: ", scan_linenr);
      printf ("%s", yytext);
   }
   scan_offset += yyleng;
}

void yyerror (const char *message) {
   assert (filename_stack.filenames != NULL);
   errprintf ("%:%s: %d: %s\n",
              filename_stack.filenames[filename_stack.last_filenr],
              scan_linenr, message);
}

void scanner_badchar (unsigned char bad) {
   char char_rep[16];
   sprintf (char_rep, isgraph ((int) bad) ? "%c" : "\\%03o", bad);
   errprintf ("%:%s: %d: invalid source character (%s)\n",
              filename_stack.filenames[filename_stack.last_filenr],
              scan_linenr, char_rep);
}

void scanner_badtoken (char *lexeme) {
   errprintf ("%:%s: %d: invalid token (%s)\n",
              filename_stack.filenames[filename_stack.last_filenr],
              scan_linenr, lexeme);
}

//all scanned tokens are sent to this function. marker HERE
int yylval_token (int symbol) {
   int offset = scan_offset - yyleng;
   char *lexeme = inserthash(&stringset, yytext);
   yylval = new_astree (symbol, filename_stack.last_filenr,
                        scan_linenr, offset, lexeme);
   addtokens(yylval);
   return symbol;
}

astree new_parseroot (void) {
   yyparse_astree = new_astree (ROOT, 0, 0, 0, "<<ROOT>>");
   return yyparse_astree;
}

astree clone (astree src, int symbol) {
   astree clone;
   switch (symbol){
   case ROOT:
      clone = new_astree (symbol, src->filenr, 
            src->linenr, src->offset, "<<ROOT>>");
      break;
   case PROTOTYPE:
      clone = new_astree (symbol, src->filenr, 
            src->linenr, src->offset, "<<PROTOTYPE>>");
      break;
   case FUNCTION:
      clone = new_astree (symbol, src->filenr, 
            src->linenr, src->offset, "<<FUNCTION>>");
      break;
   }
   return clone;
}


void scanner_include (void) {
   scanner_newline();
   char *filename = alloca (strlen (yytext) + 1);
   int linenr;
   int scan_rc = sscanf (yytext, "# %d \"%[^\"]\"", &linenr, filename);
   if (scan_rc != 2) {
      errprintf ("%: %d: [%s]: invalid directive, ignored\n",
                 scan_rc, yytext);
   }else {
      //alloc is on stack so we still need to make a copy
      char *newfilename = strdup (filename);
      assert (newfilename != NULL);
      scanner_newfilename (newfilename);
      scan_linenr = linenr - 1;
      DEBUGF ('m', "filename=%s, scan_linenr=%d\n",
              filename_stack.filenames[filename_stack.last_filenr],
              scan_linenr);
   }
}

void scanner_destroy(void){
   //clean up a list of tokens
   /* TO BE delete in lyutils
   for(int i=0; i<=tlist.last; i++)
      free(tlist.tokens[i]);
   free(tlist.tokens);
   */
   for (int i = 0; i <= filename_stack.last_filenr; i++)
      free(filename_stack.filenames[i]);
   free(filename_stack.filenames);
}



void dumptok(FILE *out){
   unsigned char printfile = -1;
   for(int i=0; i<=token_list.last; i++){
      if(printfile != token_list.tokens[i]->filenr){
         printfile = token_list.tokens[i]->filenr;
         fprintf(out, "#%3d \"%s\"\n",
               printfile, getfilename(printfile));
      }
      fprintf(out, "%4d%4d.%03d%6d   %-15s (%s)\n",
            token_list.tokens[i]->filenr,
            token_list.tokens[i]->linenr,
            token_list.tokens[i]->offset,
            token_list.tokens[i]->symbol,
            get_yytname(token_list.tokens[i]->symbol),
            //need to convert symbol to literal here
            token_list.tokens[i]->lexeme
            );
   }
}

// LINTED(static unused)
RCSC(LYUTILS_C,"$Id: lyutils.c,v 1.1 2014-06-06 18:49:21-07 - - $")
