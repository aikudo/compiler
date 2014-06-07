
#include "astree.rep.h"
#include "symtable.h"


void traverse(astree node){
   if(node == NULL) return;
   for (astree child = node->first; child != NULL; child = child->next){
      printf("%s\n", child->lexeme);
      //precproc()
      traverse(child);

      //postproc()
   }
}

void buildsym(void){
   printf("root %p\n", yyparse_astree);
   traverse(yyparse_astree);
}


RCSC(SYMTABLE_C,"$Id: symtable.c,v 1.1 2014-06-06 22:43:02-07 - - $")
