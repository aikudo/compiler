#include <libgen.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "stringtable.h"
#include "auxlib.h"
#define CPP "/usr/bin/cpp"
#define BUFFSZ 256


char *getprogm(const char *filename){
   char ext[] = ".oc";
   char *progm = strdup(filename); //free this
   size_t ondot = strlen(filename) - strlen(ext);

   if (strcmp (filename + ondot, ext) != 0) {
      errprintf("%:incorrect suffix\n", filename);
      exit(get_exitstatus());
   }else {
      progm[ondot] = '\0'; //still have 3 extra spaces
   }
   return progm;
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

int main (int argc, char** argv) {
   int option;
   opterr = 0;
   int yy_flex_debug = 0;
   int yydebug = 0;
   char *cppdef = NULL;
   FILE *fp;
   set_execname(argv[0]);

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

   (void) yydebug;
   (void) yy_flex_debug;
   if (optind >= argc) {
      errprintf ("Usage: %s [-ly] [filename]\n", get_execname());
      exit (get_exitstatus());
   }

   char* filepath =  argv[optind];
   const char* filename = basename(filepath);
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

   hashtable *stringset = newhash();
   scanfile(fp, &stringset);

   dumphash(stringset, 0);
   delhash(&stringset);
   //scanner_newfilename (filename);*/
   return 0;
}
