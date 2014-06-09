
#include "astree.h"
#include "astree.rep.h"
#include "auxlib.h"
#include "symtable.h"

typedef unsigned long bitset_t;
hashstack idents, types;


enum {
   ATTR_INDEX_VOID     =  0,
   ATTR_INDEX_BOOL     =  1,
   ATTR_INDEX_CHAR     =  2,
   ATTR_INDEX_INT      =  3,
   ATTR_INDEX_NULL     =  4,
   ATTR_INDEX_STRING   =  5,
   ATTR_INDEX_STRUCT   =  6,
   ATTR_INDEX_ARRAY    =  7,
   ATTR_INDEX_FUNCTION =  8,
   ATTR_INDEX_VARIABLE =  9,
   ATTR_INDEX_FIELD    = 10,
   ATTR_INDEX_TYPEID   = 11,
   ATTR_INDEX_PARAM    = 12,
   ATTR_INDEX_LVALUE   = 13,
   ATTR_INDEX_CONST    = 14,
   ATTR_INDEX_VREG     = 15,
   ATTR_INDEX_VADDR    = 16,
};

const bitset_t ATTR_VOID     = 1 << ATTR_INDEX_VOID;
const bitset_t ATTR_BOOL     = 1 << ATTR_INDEX_BOOL;
const bitset_t ATTR_CHAR     = 1 << ATTR_INDEX_CHAR;
const bitset_t ATTR_INT      = 1 << ATTR_INDEX_INT;
const bitset_t ATTR_NULL     = 1 << ATTR_INDEX_NULL;
const bitset_t ATTR_STRING   = 1 << ATTR_INDEX_STRING;
const bitset_t ATTR_STRUCT   = 1 << ATTR_INDEX_STRUCT;
const bitset_t ATTR_ARRAY    = 1 << ATTR_INDEX_ARRAY;
const bitset_t ATTR_FUNCTION = 1 << ATTR_INDEX_FUNCTION;
const bitset_t ATTR_VARIABLE = 1 << ATTR_INDEX_VARIABLE;
const bitset_t ATTR_FIELD    = 1 << ATTR_INDEX_FIELD;
const bitset_t ATTR_TYPEID   = 1 << ATTR_INDEX_TYPEID;
const bitset_t ATTR_PARAM    = 1 << ATTR_INDEX_PARAM;
const bitset_t ATTR_LVALUE   = 1 << ATTR_INDEX_LVALUE;
const bitset_t ATTR_CONST    = 1 << ATTR_INDEX_CONST;
const bitset_t ATTR_VREG     = 1 << ATTR_INDEX_VREG;
const bitset_t ATTR_VADDR    = 1 << ATTR_INDEX_VADDR;

// The following initialization style is a gcc-ism and will
// not work with some C compilers, and confuses lint.
// Use for reverse look-up
const char *attr_names[] = {
   [ATTR_INDEX_VOID    ] "void"    ,
   [ATTR_INDEX_BOOL    ] "bool"    ,
   [ATTR_INDEX_CHAR    ] "char"    ,
   [ATTR_INDEX_INT     ] "int"     ,
   [ATTR_INDEX_NULL    ] "null"    ,
   [ATTR_INDEX_STRING  ] "string"  ,
   [ATTR_INDEX_STRUCT  ] "struct"  ,
   [ATTR_INDEX_ARRAY   ] "array"   ,
   [ATTR_INDEX_FUNCTION] "function",
   [ATTR_INDEX_VARIABLE] "variable",
   [ATTR_INDEX_FIELD   ] "field"   ,
   [ATTR_INDEX_TYPEID  ] "typeid"  ,
   [ATTR_INDEX_PARAM   ] "param"   ,
   [ATTR_INDEX_LVALUE  ] "lvalue"  ,
   [ATTR_INDEX_CONST   ] "const"   ,
   [ATTR_INDEX_VREG    ] "vreg"    ,
   [ATTR_INDEX_VADDR   ] "vaddr"   ,
};

bitset_t bitset (int attribute_index) {
   return 1L << attribute_index;
}


void print_attributes (bitset_t attributes) {
   ssize_t size = sizeof attr_names / sizeof *attr_names;
   for (int index = 0; index < size; ++index) {
      if (attributes & bitset (index)) {
         printf (" %s", attr_names [index]);
      }
   }
}

bool is_primitive (bitset_t attributes) {
   return attributes
          & (ATTR_BOOL | ATTR_CHAR | ATTR_INT)
       && ! (attributes | ATTR_ARRAY);
}

bool is_reference (bitset_t attributes) {
   return attributes
          & (ATTR_NULL | ATTR_STRING | ATTR_STRUCT | ATTR_ARRAY)
       && TRUE;
}

/*
int main (void) {
   printf ("Number of bits in a bitset = %lu\n",
           CHAR_BIT * sizeof (bitset_t));
   for (bitset_t set = 0xF; set < 1L << 32; set <<= 4) {
      printf ("bitset 0x%016lX =", set);
      print_attributes (set);
      printf ("\n");
   }
   return EXIT_SUCCESS;
}
*/


////START HERE

bool checktype(astree type, astree ident){
   if(find_hashstack(types, type->lexeme)){
      return true;
   }else{
      eprintf("Type %s(%d.%d.%d) used by %s is not declared.\n",
            type->lexeme,
            type->filenr,
            type->linenr,
            type->offset,
            ident->lexeme);
      return false;
   }
}


//
//grammar:= fielddecl | identdecl -> basetype []? FIELD
//basetype is root. If exist '[]', it's the first child.
//FIELD/DECLID is the next child.
//
//set flags on an identifier AST node and return that token
astree basetype(astree root){
   astree ident, type;
   bitset_t attributes = 0;
   if(root->symbol == ARRAY){
      type = root->first;
      ident = type->next;
      attributes = ATTR_ARRAY;
   } else{
      type = root;
      ident = type->first;
   }

   switch(type->symbol){
      case VOID:     attributes |= ATTR_VOID; break;
      case BOOL:     attributes |= ATTR_BOOL; break;
      case CHAR:     attributes |= ATTR_CHAR; break;
      case INT:      attributes |= ATTR_INT; break;
      case STRING:   attributes |= ATTR_STRING; break;
      case TYPEID:   checktype(type, ident);
                     ident->structid = type;
                     attributes |= ATTR_STRUCT; break;
      default:
                     STUBPRINTF("ERROR: Uncaught symbol%s\n",
                        get_yytname(type->symbol));
                     exit(1);
   }

   ident->attributes = attributes;
   return ident;
}

// Struct has its own namespace.
// It can't collide except when it defines e.g.
//
// struct foo {}                 //declare
// struct foo { int a; foo a; }  //define
// struct foo {}  //fail
//
// Member fields are in their own name space for each struct.
//



void setattr(hsnode node, astree item){
   node->filenr = item->filenr;
   node->linenr = item->linenr;
   node->offset = item->offset;
   node->attributes = item->attributes;
   item->sym = node;
}

//An add wrapper for add_hashstack()
//attributes from AST is copied to SYM node
//AST node hooks to SYM node for easy printout and cleanup

hsnode add(hashstack this, astree item){
   hsnode node = add_hashstack(this, item->lexeme);
   setattr(node,item);
   return node;
}

//
//Duplicated complain:  dup(tag, original, duplicate)
//
//void dcomplain(char *tag, hsnode ori, hsnode dup){ //orig->sym

void dcomplain(char *tag, hsnode ori, astree dup){
   eprintf("DUP:%s %s(%d.%d.%d) is already defined at (%d.%d.%d).\n",
         tag, dup->lexeme, dup->filenr, dup->linenr, dup->offset,
                           ori->filenr, ori->linenr, ori->offset);
}

//
//Error complain: err(message, item)
//
void ecomplain(char *errmsg, hsnode it){
   eprintf("ERR:%s(%d.%d.%d): %s\n",
         it->lexeme, it->filenr, it->linenr, it->offset, errmsg);
}

void structblock(astree root){
   astree typeid = root->first;
   DEBUGF('s', "struct %s\n",typeid->lexeme);
   hsnode found = find_hashstack(types, typeid->lexeme);

   if(found && found->fields){
      dcomplain("struct", found, typeid);
      return;
   }else if(!found){
      typeid->attributes = ATTR_STRUCT | ATTR_TYPEID;
      found = add(types, typeid);
   }

   astree member = typeid->next;
   if(!member) return; // no definition

   hashstack fields = new_hashstack();
   for(; member; member = member->next){
      astree field = basetype(member);
      field->attributes |= ATTR_FIELD;
      hsnode orig = find_hashstack(fields, field->lexeme);
      if(orig) {
         dcomplain("field", orig, field);
      }else{
         add(fields, field);
      }
   }
   setattr(found, typeid);
   found->fields = fields;
}

//void enterblock(){ 
//   ident->blocknr++;
//}
//
//void exitblock(){
//
//   ident->blocknr;
//}
//
//
////only need to process the first child
//// char a = 3; // ->proccess a
//// 
////good case: not found
////or found: but i'm not the current global scope
////
//

//Functions and variables share a same name space.
//Functions are global, while variables can be global or local.
//Global can't overwrite global.
//Local variable can shadow global variable ONLY.
//Local can't be collided with upper scope declaration.
//

void valdeclar(astree root){
   astree type = root->first;
   astree ident = basetype(type);
   ident->attributes |= ATTR_LVALUE;

   hsnode identsym = find_hashstack(idents, root->lexeme);
   //if(ident && idents->blocknr == 0){
   if(identsym){  //ignore global case for now
      eprintf("Duplicate declaration %s (%d.%d.%d),  (%d.%d.%d)\n",
            root->lexeme,
            root->filenr, root->linenr, root->offset,
            ident->filenr, ident->linenr, ident->offset);
      return;
   } else{
      //identsym = push_hashstack(idents, ident);
   }
}



void preproc(astree root){
   switch(root->symbol){
      case VARDECL:
         valdeclar(root);
         break;
      case STRUCT:
         structblock(root);
         break;
      case PROTOTYPE:
         DEBUGF('P',"PRE on PROTOTYPE:[%s] \n", root->lexeme);
         break;
      case PARAM:
         DEBUGF('P',"PRE on PARAM:[%s] \n", root->lexeme);
         break;
      case BLOCK:
         DEBUGF('P',"PRE on BLOCK:[%s] \n", root->lexeme);
         break;
      case FUNCTION:
         //functionblock(root);
         DEBUGF('P',"PRE on FUNCTION:[%s] \n", root->lexeme);
         break;
   }
}

void postproc(astree root){
   switch(root->symbol){
      case PROTOTYPE:
         DEBUGF('P',"POST on PROTOTYPE:[%s] \n", root->lexeme);
         break;
      case PARAM:
         DEBUGF('P',"POST on PARAM:[%s] \n", root->lexeme);
         break;
      case BLOCK:
         DEBUGF('P',"POST on BLOCK:[%s] \n", root->lexeme);
         break;
      case FUNCTION:
         DEBUGF('P',"POST on FUNCTION:[%s] \n", root->lexeme);
         break;
   }
}


void traverse(astree node){
   if(node == NULL) return;
   for (astree child = node->first; child != NULL;
         child = child->next){
      preproc(child);
      traverse(child);
      //postproc(child);
   }
}

bool is_ident(bitset_t attributes){
   return (attributes &
         (ATTR_FUNCTION | ATTR_FUNCTION | ATTR_VARIABLE | ATTR_FIELD
          | ATTR_TYPEID | ATTR_PARAM ));
}

void printsym(astree node){
   if(node == NULL) return;

   if(is_ident(node->attributes)){
      printf("%*s%s: ", node->blocknr * 3, "", node->lexeme);
      print_attributes (node->attributes);
      if(node->structid){
         printf(" {%s}", node->structid->lexeme);
      }

      printf("\n");
   }

   for (astree child = node->first; child != NULL;
         child = child->next){
      printsym(child);
      //postproc(child);
   }
}


void buildsym(void){
   idents = new_hashstack();
   types = new_hashstack();

   printf("root %p\n", yyparse_astree);
   traverse(yyparse_astree);
   printf("---------------------\n\n\n");
   printsym(yyparse_astree);
}


RCSC(SYMTABLE_C,"$Id: symtable.c,v 1.5 2014-06-09 05:03:17-07 - - $")
