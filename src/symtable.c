
#include "astree.h"
#include "astree.rep.h"
#include "auxlib.h"
#include "symtable.h"

//http://www.rightcorner.com/code/CPP/Concepts/DeviceManager/sample.php
typedef unsigned long bitset_t;


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



hashstack *idents;
hashstack *types;

int gblblock = 0;

//set a flag on an identifier token and return that token
astree basetype(astree type, hsnode attrib){
   astree ident;
   bitset_t attributes = 0L;
   if(type->symbol == ARRAY){
      type = type->first;
      ident = type->next;
      attributes |= ATTR_ARRAY;
   } else{
      ident = type->first;
      switch(type->symbol){
         case VOID: //this should flag a error we do type checking
            attributes |= ATTR_VOID;
            break;
         case BOOL:
            attributes |= ATTR_BOOL;
            break;
         case CHAR:
            attributes |= ATTR_CHAR;
            break;
         case INT:
            attributes |= ATTR_INT;
            break;
         case STRING:
            attributes |= ATTR_STRING;
            break;
         case TYPEID:
            attributes |= ATTR_TYPEID;
            break;
         default:
            STUBPRINTF("ERROR: Uncaught symbol%s\n",
                  get_yytname(type->symbol));
            exit(1);
      }
   }

   //attributes |= ATTR_LVALUE; //struct's field can't be
   attrib->attributes = attributes;
   //attrib->location?? set location?
   ident->attrib = attrib; //attach hashstack node to astree node
   return ident;
}

void structblock(astree child){
   //TODO make sure that struct wasn't declared yet
   astree structnode = child->first;
   bitset_t attributes = 0;
   if(find_hashstack(types, structnode->lexeme)){
      printf("struct %s already declared\n", structnode->lexeme);
   }else{
      hsnode node = add_hashstack(types, structnode->lexeme);
      attributes |= ATTR_STRUCT;
      attributes |= ATTR_TYPEID;
      //node->blocknr = 0;
      //setup line coordinate here
      //create as hashtable
      hashstack *fields = new_hashstack();
      for(astree field = structnode->next; field; field = field->next){
         astree fieldid = field->first; //field: type; fieldid: name
         hsnode  fieldnode = add_hashstack(fields, fieldid->lexeme);
         fieldnode->attributes |= ATTR_FIELD;
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

void buildsym(void){
   /*ident = new_hashstack();
   types = new_hashstack();
   */

   printf("root %p\n", yyparse_astree);
   //traverse(yyparse_astree);
}


RCSC(SYMTABLE_C,"$Id: symtable.c,v 1.1 2014-06-08 00:52:12-07 - - $")
