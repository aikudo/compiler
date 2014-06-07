
#include "astree.rep.h"
#include "symtable.h"

/*
hashstack *idents;
hashstack *types;

int gblblock = 0;

//change hsnode* to hsnode!

void basetype(astree type, hsnode attrib){
   astree idnode;
   if(type->symbol == ARRAY){
      type = type->first;
      idnode = type->next;
      attrib->at.isarray = 1;
   } else{
      idnode = type->first;
      switch(type->symbol){
         case VOID: //this should flag a error we do type checking
            attrib->at.isvoattrib = 1;
            break;
         case BOOL:
            attrib->at.isbool = 1;
            break;
         case CHAR:
            attrib->at.ischar = 1;
            break;
         case INT:
            attrib->at.isint = 1;
            break;
         case STRING:
            attrib->at.isstring = 1;
            break;
      }
   }
   idnode->attrib = attrib;
}

void structblock(astree child){
   //make sure that struct wasn't declared yet
   astree structnode = child->first;
   if(find_hashstack(types, structnode->lexeme)){
      printf("struct %s already declared\n", structnode->lexeme);
   }else{
      hsnode *node = add_hashstack(types, structnode->lexeme);
      node->at.isstruct = 1;
      node->at.istypeid = 1;
      //node->blocknr = 0;
      //setup line coordinate here
      //create as hashtable
      hashstack *fields = new_hashstack();
      for(astree field = structnode->next; field; field = field->next){
         astree fieldid = field->first; //field: type; fieldid: name
         hsnode  *fieldnode = add_hashstack(fields, fieldid->lexeme);
         fieldnode->at.isfield = 1;
         basetype(field, fieldnode);

      }
   }
}

void preproc(astree child){
   switch(child->symbol){
      case STRUCT:
         structblock(child);
         break;
      case PROTOTYPE:
         printf("PRE on PROTOTYPE:[%s] \n", child->lexeme);
         break;
      case PARAM:
         printf("PRE on PARAM:[%s] \n", child->lexeme);
         break;
      case BLOCK:
         printf("PRE on BLOCK:[%s] \n", child->lexeme);
         break;
      case FUNCTION:
         printf("PRE on FUNCTION:[%s] \n", child->lexeme);
         break;
   }
}

void postproc(astree child){
   switch(child->symbol){
      case PROTOTYPE:
         printf("POST on PROTOTYPE:[%s] \n", child->lexeme);
         break;
      case PARAM:
         printf("POST on PARAM:[%s] \n", child->lexeme);
         break;
      case BLOCK:
         printf("POST on BLOCK:[%s] \n", child->lexeme);
         break;
      case FUNCTION:
         printf("POST on FUNCTION:[%s] \n", child->lexeme);
         break;
   }
}


void traverse(astree node){
   if(node == NULL) return;
   for (astree child = node->first; child != NULL; child = child->next){
      preproc(child);
      traverse(child);
      postproc(child);
   }
}
*/

void buildsym(void){
   /*ident = new_hashstack();
   types = new_hashstack();
   */

   printf("root %p\n", yyparse_astree);
   //traverse(yyparse_astree);
}


RCSC(SYMTABLE_C,"$Id: symtable.c,v 1.2 2014-06-07 15:58:10-07 - - $")
