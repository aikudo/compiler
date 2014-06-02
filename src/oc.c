#include <errno.h>
#include <libgen.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include "astree.h"
#include "astree.rep.h"
#include "stringtable.h"
#include "auxlib.h"
#include "lyutils.h"
#define CPP "/usr/bin/cpp"
#define BUFFSZ 256

#define PRM      0
#define STR      1
#define TOK      2
#define AST      3
#define SYM      4
#define OIL      5
#define OUTFILES 6

//a global handler for a centralized memory cleanup
struct {
   char **names;
   FILE *fp;
   hashtable *stringset;
   int yydebug;
   int yy_flex_debug;
   int other;
}gblinfo = {NULL, NULL, NULL, 0, 0 ,0};


char *getprogm(const char *filename){
   char ext[] = ".oc";
   size_t ondot = strlen(filename) - strlen(ext);

   if (strcmp (filename + ondot, ext) != 0) {
      errprintf("%:incorrect suffix\n", filename);
      exit(get_exitstatus());
   }
   gblinfo.names = malloc(OUTFILES * sizeof (char*));
   for(int i=0; i< OUTFILES; i++){
      gblinfo.names[i] = malloc((strlen(filename) + 2) *sizeof(char));
      gblinfo.names[i] = strncpy(gblinfo.names[i], filename, ondot);
      gblinfo.names[i][ondot] = '\0';
   }
   gblinfo.names[STR] = strcat (gblinfo.names[STR], ".str");
   gblinfo.names[TOK] = strcat (gblinfo.names[TOK], ".tok");
   gblinfo.names[AST] = strcat (gblinfo.names[AST], ".ast");
   gblinfo.names[SYM] = strcat (gblinfo.names[SYM], ".sym");
   gblinfo.names[OIL] = strcat (gblinfo.names[OIL], ".oil");
   return gblinfo.names[PRM];
}

void scanopts(int argc, char **argv){
   int option;
   opterr = 0;
   char *cppdef = NULL;
   FILE *fp;
   set_execname(argv[0]);
   yy_flex_debug = 0;
   yydebug  = 0;

   for(;;) {
      option = getopt (argc, argv, "@:lyD:");
      if (option == EOF) break;
      switch (option) {
         case '@': set_debugflags (optarg);   break;
         case 'l': yy_flex_debug = 1;         break;
         case 'y': yydebug = 1;               break;
         case 'D': cppdef = optarg;           break;
         default:  errprintf ("%:bad option (%c)\n", optopt); break;
      }
   }

   cppdef = NULL;

   if (optind >= argc) {
      errprintf ("Usage: %s [-ly] [filename]\n", get_execname());
      exit (get_exitstatus());
   }

   char* filepath =  argv[optind];
   char* filename = basename(filepath);
   char* progm = getprogm(filename);
   char cpp_command[256];
   char *to = cpp_command;
   to = stpcpy(to, CPP);
   if(cppdef){
      to = stpcpy(to, " -D");
      to = stpcpy(to, cppdef);
   }
   to = stpcpy(to, " ");
   to = stpcpy(to, filepath);

   DEBUGF('f', "progm=%s cpp_path=%s\n", progm, cpp_command);

   if( (fp = popen(cpp_command, "r")) == NULL ){
      syserrprintf (cpp_command);
      exit (get_exitstatus());
   }

   yyin = fp;
   gblinfo.fp = fp;
}

struct {
   astree *tokens;
   int size;
   int last;
} tlist = {NULL, 0 , -1};

void addtokens(astree token){
   assert(token != NULL);
   if (tlist.tokens == NULL) {
      tlist.size = 16;
      tlist.tokens = malloc (tlist.size * sizeof (astree));
      assert (tlist.tokens != NULL);
   }else if (tlist.last == tlist.size - 1){
      tlist.size *= 2;
      tlist.tokens = realloc (tlist.tokens,
                              tlist.size * sizeof(astree));
      assert (tlist.tokens != NULL);
   }
   tlist.tokens[++tlist.last] = token;
}

void scanfile(void){
   extern astree yylval;
   hashtable *stringset = gblinfo.stringset;
   (void) yylval;
   (void) stringset;
   yyparse();
   /*
   while( yylex() != YYEOF ){
      char *istr = inserthash(&stringset, yylval->lexinfo);
      free(yylval->lexinfo);
      yylval->lexinfo = istr;
      addtokens(yylval);
   }
   */
}

void dumpstr(void){
   FILE *out = fopen(gblinfo.names[STR], "w");
   if(!out){
      fprintf(stderr, "%s: %s\n", gblinfo.names[STR],
            strerror(errno));
      exit(1);
   }
   assert (gblinfo.stringset != NULL);
   dumphash (gblinfo.stringset, out);
   fclose (out);
}


void dumptok(void){
   FILE *out = fopen(gblinfo.names[TOK], "w");
   if(!out){
      fprintf(stderr, "%s: %s\n", gblinfo.names[TOK],
            strerror(errno));
      exit(1);
   }
   unsigned char printfile = -1;
   for(int i=0; i<=tlist.last; i++){
      if(printfile != tlist.tokens[i]->filenr){
         printfile = tlist.tokens[i]->filenr;
         fprintf(out, "#%3d \"%s\"\n",
               printfile, getfilename(printfile));
      }
      fprintf(out, "%4d%4d.%03d%6d   %-15s (%s)\n",
            tlist.tokens[i]->filenr,
            tlist.tokens[i]->linenr,
            tlist.tokens[i]->offset,
            tlist.tokens[i]->symbol,
            get_yytname(tlist.tokens[i]->symbol),
            //need to convert symbol to literal here
            tlist.tokens[i]->lexinfo
            );
   }
   fclose (out);
}

extern int yylex_destroy (void);

void destroy_all(void){

   yylex_destroy();
   scanner_destroy();
   delhash(& (gblinfo.stringset) );
   //clean up a list of tokens
   for(int i=0; i<=tlist.last; i++)
      free(tlist.tokens[i]);
   free(tlist.tokens);

   for(int i=0; i < OUTFILES; i++){
      free( gblinfo.names[i]);
   }
   //for future opened files for dumping to should be
   //closed as well
   free(gblinfo.names);
   pclose(gblinfo.fp);
}

int main (int argc, char** argv) {
   scanopts(argc, argv);
   gblinfo.stringset = newhash();
   scanfile();
//   dumpstr();
//   dumptok();

dump_astree (stdout, yyparse_astree, 0);

//   destroy_all();
   return 0;
}

RCSC(OC_C,"$Id: oc.c,v 1.1 2014-06-02 04:56:59-07 - - $")
