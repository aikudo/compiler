#include <libgen.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "astree.h"
#include "astree.rep.h"
#include "stringtable.h"
#include "auxlib.h"
#include "lyutils.h"
#define CPP "/usr/bin/cpp"
#define BUFFSZ 256

//a global handler for memory cleanup
#define PRM    0
#define STR    1
#define TOK    2
#define AST    3
#define SYM    4
#define OIL    5
#define OFILES 6

struct gblinfo{
   char **names;
   int yydebug;
   int yy_flex_debug;
   int other;
   FILE *fp;
}gblinfo;


char *getprogm(const char *filename){
   char ext[] = ".oc";
   size_t ondot = strlen(filename) - strlen(ext);

   if (strcmp (filename + ondot, ext) != 0) {
      errprintf("%:incorrect suffix\n", filename);
      exit(get_exitstatus());
   }
   gblinfo.names = malloc(OFILES * sizeof (char*));
   for(int i=0; i< OFILES; i++){
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

FILE *scanopts(int argc, char **argv){
   int option;
   opterr = 0;
   char *cppdef = NULL;
   FILE *fp;
   set_execname(argv[0]);

   for(;;) {
      option = getopt (argc, argv, "@:lyD:");
      if (option == EOF) break;
      switch (option) {
         case '@': set_debugflags (optarg);   break;
         case 'l': gblinfo.yy_flex_debug = 1; break;
         case 'y': gblinfo.yydebug = 1;       break;
         case 'D': cppdef = optarg;           break;
         default:  errprintf ("%:bad option (%c)\n", optopt); break;
      }
   }

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
   //yyin = fp;
   //scanner_newfilename (filename);
   gblinfo.fp = fp;
   return fp;
}

void scanfile(FILE *fp, hashtable **stringset){
   char buff[BUFFSZ];
   char delim[] = " \t\n";
   char *saveptr, *token;
   while (fgets(buff, BUFFSZ, fp)){
      for (char *str = buff; ; str = NULL){
         token = strtok_r(str, delim, &saveptr);
         if (token == NULL) break;
         DEBUGF('t', "token: \"%s\"\n", token);
         inserthash(stringset, token);
      }
   }
}

struct {
   astree *tokens;
   int size;
   int last;
} tokens = {NULL, 0 , -1};

//void inserttokens

int main (int argc, char** argv) {
   FILE *fp = scanopts(argc, argv);
   yyin = fp ;
  // int parsecode = yyparse();

   extern astree yylval;
   //will have lots of memory leak
   while( yylex() != YYEOF ){
      //insert search on into the string table.
      //insert into a list of tokens

      printf("sizeof %d \n",  yylval->symbol);
      free(yylval->lexinfo);
      free(yylval);
   }


   /*
   hashtable *stringset = newhash();
   scanfile(fp, &stringset);
   dumphash(stringset, 0);
   delhash(&stringset);
   */

   //freemem();
   pclose(fp);
   for(int i=0; i < OFILES; i++){
      free( gblinfo.names[i]);
   }
   free(gblinfo.names);
   yylex_destroy();
   scanner_destroy();
   return 0;
}

