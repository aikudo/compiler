/*
 * There are two parts to this phase.
 * First build a simple table that stores identifiers
 * and their properties. This is done by top-down
 * traversing an abstract syntax tree.
 * Second, `strong' type-checking all the
 * expressions. This is done by a bottom-up
 * traversal. These two can be combined in one
 * traversal as in:
 *    traverse(root){
 *       preproc(root);
 *       traverse(root->child);
 *       postproc(root);
 *    }
 *
 *
 * */


// TODO:
// should change the TRUE -> TRU
// and FALSE -> FAL
// ad NULL -> NIL
//
// to prevent some random

#include "astree.rep.h"
#include "auxlib.h"
#include "symtable.h"

typedef unsigned long bitset_t;
hashstack idents, types;

#define BZ 1024
char strbuff[BZ];

#define GLOBAL 0
struct{
   int *blocks;
   int size;
   int last;
   int globalcount;
} blockcount_stack = {NULL, 0, -1, -1};

int pushblock(void){
   int block = ++blockcount_stack.globalcount;
   if (blockcount_stack.blocks == NULL) {
      blockcount_stack.size = 16;
      blockcount_stack.blocks = 
         malloc (blockcount_stack.size * sizeof (int));
      assert (blockcount_stack.blocks != NULL);
   }else if (blockcount_stack.last == blockcount_stack.size - 1){
      blockcount_stack.size *= 2;
      blockcount_stack.blocks = realloc (blockcount_stack.blocks,
                              blockcount_stack.size * sizeof(int));
      assert (blockcount_stack.blocks != NULL);
   }
   blockcount_stack.blocks[++blockcount_stack.last] = block;
   idents->block = block;
   return block;
}

//must call this for all entries into idents hashstack
int topblock(void){
   assert(blockcount_stack.last > -1);
   return blockcount_stack.blocks[blockcount_stack.last];
}

int popblock(void){
   assert(blockcount_stack.last > -1);
   int block = blockcount_stack.blocks[--blockcount_stack.last];
   idents->block = block;
   return block;
}
enum {
   ATTR_INDEX_VOID      =  0,
   ATTR_INDEX_BOOL      =  1,
   ATTR_INDEX_CHAR      =  2,
   ATTR_INDEX_INT       =  3,
   ATTR_INDEX_NULL      =  4,
   ATTR_INDEX_STRING    =  5,
   ATTR_INDEX_STRUCT    =  6,
   ATTR_INDEX_ARRAY     =  7,
   ATTR_INDEX_FUNCTION  =  8,
   ATTR_INDEX_VARIABLE  =  9,
   ATTR_INDEX_FIELD     = 10,
   ATTR_INDEX_TYPEID    = 11,
   ATTR_INDEX_PARAM     = 12,
   ATTR_INDEX_LVALUE    = 13,
   ATTR_INDEX_CONST     = 14,
   ATTR_INDEX_VREG      = 15,
   ATTR_INDEX_VADDR     = 16,
   ATTR_INDEX_PROTOTYPE = 17,
};

const bitset_t ATTR_VOID      = 1 << ATTR_INDEX_VOID;
const bitset_t ATTR_BOOL      = 1 << ATTR_INDEX_BOOL;
const bitset_t ATTR_CHAR      = 1 << ATTR_INDEX_CHAR;
const bitset_t ATTR_INT       = 1 << ATTR_INDEX_INT;
const bitset_t ATTR_NULL      = 1 << ATTR_INDEX_NULL;
const bitset_t ATTR_STRING    = 1 << ATTR_INDEX_STRING;
const bitset_t ATTR_STRUCT    = 1 << ATTR_INDEX_STRUCT;
const bitset_t ATTR_ARRAY     = 1 << ATTR_INDEX_ARRAY;
const bitset_t ATTR_FUNCTION  = 1 << ATTR_INDEX_FUNCTION;
const bitset_t ATTR_VARIABLE  = 1 << ATTR_INDEX_VARIABLE;
const bitset_t ATTR_FIELD     = 1 << ATTR_INDEX_FIELD;
const bitset_t ATTR_TYPEID    = 1 << ATTR_INDEX_TYPEID;
const bitset_t ATTR_PARAM     = 1 << ATTR_INDEX_PARAM;
const bitset_t ATTR_LVALUE    = 1 << ATTR_INDEX_LVALUE;
const bitset_t ATTR_CONST     = 1 << ATTR_INDEX_CONST;
const bitset_t ATTR_VREG      = 1 << ATTR_INDEX_VREG;
const bitset_t ATTR_VADDR     = 1 << ATTR_INDEX_VADDR;
const bitset_t ATTR_PROTOTYPE = 1 << ATTR_INDEX_PROTOTYPE;

// Use for reverse look-up
// The following initialization style is a gcc-ism and will
// not work with some C compilers, and confuses lint.
const char *attr_names[] = {
   [ATTR_INDEX_VOID      ] "void"     ,
   [ATTR_INDEX_BOOL      ] "bool"     ,
   [ATTR_INDEX_CHAR      ] "char"     ,
   [ATTR_INDEX_INT       ] "int"      ,
   [ATTR_INDEX_NULL      ] "null"     ,
   [ATTR_INDEX_STRING    ] "string"   ,
   [ATTR_INDEX_STRUCT    ] "struct"   ,
   [ATTR_INDEX_ARRAY     ] "array"    ,
   [ATTR_INDEX_FUNCTION  ] "function" ,
   [ATTR_INDEX_VARIABLE  ] "variable" ,
   [ATTR_INDEX_FIELD     ] "field"    ,
   [ATTR_INDEX_TYPEID    ] "typeid"   ,
   [ATTR_INDEX_PARAM     ] "param"    ,
   [ATTR_INDEX_LVALUE    ] "lvalue"   ,
   [ATTR_INDEX_CONST     ] "const"    ,
   [ATTR_INDEX_VREG      ] "vreg"     ,
   [ATTR_INDEX_VADDR     ] "vaddr"    ,
   [ATTR_INDEX_PROTOTYPE ] "prototype",
};

bitset_t bitset (int attribute_index) {
   return 1L << attribute_index;
}


char * print_attributes (bitset_t attributes) {
   ssize_t size = sizeof attr_names / sizeof *attr_names;
   char buff[100];

   strbuff[0] = 0;
   for (int index = 0; index < size; ++index) {
      if (attributes & bitset (index)) {
         snprintf(buff, 100, " %s", attr_names [index]);
         strncat(strbuff,buff,BZ);
      }
   }
   return(strdup(strbuff));
}

//
//categories of types
// void: neither primitive nor reference. Can be used only as a 
// return type for a function. It's an error otherwise
// primitive: bool, char and int. Can't be an array.
// reference: null, string, struct typename, basetype[] array.
//

bool is_primitive (bitset_t attributes) {
   return attributes
          & (ATTR_BOOL | ATTR_CHAR | ATTR_INT)
       && ! (attributes | ATTR_ARRAY);
}

bool is_reference (bitset_t attributes) {
   return attributes
          & (ATTR_NULL | ATTR_STRING | ATTR_STRUCT | ATTR_ARRAY) && 1;
}

char *tostr(hsnode it){
   snprintf(strbuff, BZ, "%s(%d.%d.%d)",
         it->lexeme, it->filenr, it->linenr, it->offset);
   return (strdup(strbuff)); //leaky
}

char *atostr(astree it){
   snprintf(strbuff, BZ, "%s(%d.%d.%d)",
         it->lexeme, it->filenr, it->linenr, it->offset);
   return (strdup(strbuff)); //leaky
}

//
//print duplicated declaration
//
void prndup(char *tag, hsnode ori, astree dup){
   eprintf("dup:%s %s is already defined at %s.\n",
         tag, atostr(dup), tostr(ori));
}

//
//Error complain: err(message, item)
//

void prnerr( hsnode it, char *errmsg){
   eprintf("%s: %s\n", tostr(it), errmsg);
}

/*----------------------------------------------------------------*/
//  Building a symbol table
//  Top-down a.k.a. pre-fix processing

void enterblock(astree root){ 
   pushblock();
   DEBUGF('b', "%s enterblock: %d\n", atostr(root), topblock());
}

//
// exiting a block will clear out current stack frame
// collided global variables are restored back to the
// hash table.
//

void exitblock(astree root){
   popblock();
   DEBUGF('b', "%s exitblock: %d\n", atostr(root), topblock());
   hsnode node = peak_hashstack(idents);
   //pop all node bigger than current idents->block.
   //except for global nodes with block:0 which will 
   //be restored back into the hash table.

   while( node &&  node->block != idents->block){
      if(node->block == GLOBAL) insert_hashstack(idents, node);
      node = pop_hashstack(idents);
      assert( rm_hashstack(idents, node->lexeme) != NULL);
      DEBUGF('s', "REMOVE blk:%d, %s\n", node->block, tostr(node));
      node = peak_hashstack(idents);
   }
}


bool checktypeid(astree type, astree ident){
   if(find_hashstack(types, type->lexeme)){
      return true;
   }else{
      eprintf("Type %s used by %s is not declared.\n",
            atostr(type), atostr(ident));
      return false;
   }
}


/*
   Find DECLID token based on its root.
   Return DECLID node with attributes are set.

grammar: basetype [ ‘[]’ ] FIELD | DECLID

e.g.:

   ARRAY "[]" 4.9.3
      INT "int" 4.9.0
      DECLID "fibonacci" 4.9.6

   INT "int" 4.14.0
      DECLID "index" 4.14.4

   TYPEID "stack" 4.20.3
      DECLID "stack" 4.20.9

*/

astree basetype(astree root){
   astree declid, type;
   bitset_t attributes = 0;
   if(root->symbol == ARRAY){
      type = root->first;
      declid = type->next;
      attributes = ATTR_ARRAY;
   } else{
      type = root;
      declid = type->first;
   }

   assert(declid->symbol == FIELD || declid->symbol == DECLID);

   switch(type->symbol){
      case VOID   : attributes |= ATTR_VOID; break; //return functype
      case BOOL   : attributes |= ATTR_BOOL; break;
      case CHAR   : attributes |= ATTR_CHAR; break;
      case INT    : attributes |= ATTR_INT; break;
      case STRING : attributes |= ATTR_STRING; break;
      case TYPEID : checktypeid(type, declid);
                    declid->stypeid = type->lexeme;
                    attributes |= ATTR_STRUCT; break;
      default: STUBPRINTF("ERROR: Uncaught symbol%s\n",
                     get_yytname(type->symbol)); exit(1);
   }

   declid->attributes = attributes;
   return declid;
}


void setattr(hsnode node, const astree const item){
   node->filenr     = item->filenr;
   node->linenr     = item->linenr;
   node->offset     = item->offset;
   node->block      = item->block;
   node->attributes = item->attributes;
   item->sym        = node;
}

//An add wrapper for add_hashstack()
//attributes from AST is copied to SYM node
//AST node hooks to SYM node for easy printout and cleanup

hsnode add(hashstack this, const astree const item){
   hsnode node = add_hashstack(this, item->lexeme);
   setattr(node,item);
   return node;
}

// 
// Struct has its own namespace.
//
// struct foo {}                 //declare
// struct foo { int a; foo a; }  //define
// struct foo {}  //fail
//
// Member fields are in their own namespace for each struct.
//
//   STRUCT "struct" 4.7.0
//      TYPEID "stack" 4.7.7
//      ARRAY "[]" 4.8.9
//         STRING "string" 4.8.3
//         FIELD "data" 4.8.12
//      INT "int" 4.9.3
//         FIELD "size" 4.9.7
//      INT "int" 4.10.3
//         FIELD "top" 4.10.7

void structblock(astree root){
   astree typeid = root->first;
   assert(typeid->symbol == TYPEID);
   DEBUGF('s', "struct %s\n",typeid->lexeme);
   hsnode found = find_hashstack(types, typeid->lexeme);

   if(found && found->fields){   //already defined
      prndup("struct", found, typeid);
      return;
   }else if(!found){
      typeid->attributes = ATTR_STRUCT | ATTR_TYPEID;
      found = add(types, typeid);
   }

   astree member = typeid->next;
   if(!member) return; // no definition; it's a declaration.

   hashstack fields = new_hashstack();
   for(; member; member = member->next){
      astree field = basetype(member);
      field->attributes |= ATTR_FIELD;
      hsnode orig = find_hashstack(fields, field->lexeme);
      if(orig) {
         prndup("field", orig, field);
      }else{
         hsnode addedfield = add(fields, field);
         push_hashstack(fields, addedfield);
      }
   }
   setattr(found, typeid); //overwrite declaration w/ definition
   found->fields = fields;
}

/*
 vardecl: variable declaration/initialization
 grammar:  vardecl: identdecl `=' expr `;'
 e.g.: 
      int [] a = 4; 
      stack stack = NULL;

VARDECL "=" 4.9.16
   ARRAY "[]" 4.9.3
      INT "int" 4.9.0
      DECLID "fibonacci" 4.9.6

VARDECL "=" 4.14.10
   INT "int" 4.14.0
      DECLID "index" 4.14.4
   INTCON "2" 4.14.12

VARDECL "=" 4.20.15
   TYPEID "stack" 4.20.3
      DECLID "stack" 4.20.9

VARDECL "=" 4.36.14
   STRING "string" 4.36.3
      DECLID "tmp" 4.36.10

   Functions and variables share a same name space.
   Functions are global, while variables can be global or local.
   Global can't overwrite global.
   Local variable can shadow global variable ONLY.
   Local can't be collided with upper scope declaration.
      
*/

void vardecl (astree root){
   astree type = root->first;
   astree declid = basetype(type);
   DEBUGF('v', "bid:%d %s %s\n",
         topblock(), type->lexeme, declid->lexeme);
   hsnode orig = find_hashstack(idents, declid->lexeme);
   if(orig){
      if(orig->block != GLOBAL ||   //upper scope that isn't global
            //global scope collision
            //(orig->block == GLOBAL && idents->block == GLOBAL)){
            (orig->block == GLOBAL && topblock() == GLOBAL)){
         prndup("vardecl", orig, declid);
         return;
      }else{ //shadow the global variable
         hsnode globalident = rm_hashstack(idents, orig->lexeme);
         push_hashstack(idents, globalident); //save global ident
      }
   }
   declid->attributes |= ATTR_LVALUE | ATTR_VARIABLE;
   declid->block = topblock();  //block-stamp the ident
   hsnode newident = add(idents, declid);
   push_hashstack(idents, newident);
}

//
//attach a list of parameters to a function
//If tmpscope is _not_ set, use the same namescape of idents 
//hashstable. Otherwise create a temperary namespace.
//

hsnode attachparam(astree fn, astree paramlist, bool tmpscope){

   hashstack tmphash = tmpscope ? new_hashstack() : idents;
   hsnode fn1 = add(tmphash, fn);

   if(!tmpscope) enterblock(fn);
   for( astree itor = paramlist->first; itor; itor = itor->next){
      astree param = basetype(itor);
      param->attributes |= ATTR_PARAM | ATTR_LVALUE | ATTR_VARIABLE;
      //printf("topblock %d scope %d\n", topblock(), tmpscope);
      param->block = topblock();
      hsnode paramdup = find_hashstack(tmphash, param->lexeme);
      if(paramdup) prndup("param", paramdup, param);
      else{
         hsnode paramident = add(tmphash, param);
         //printf("added node %s %d\n", tostr(paramident),
               //paramident->block);
         push_hashstack(tmphash, paramident);
         paramident->param = fn1->param; //threading params
         fn1->param = paramident;
      }
   }

   return fn1;
}



//
// Two functions are matched when all their params and functions are
// exactly the same.
//
// A function has its name at the head of the list subsequence 
// nodes are parameters.
//
// Note: prototype has to be in this form because of grammar
//    fun|proto -> identdecl ( identdecl* ) block
//    block -> { statements* } | ;
// e.g.: type proto ( type var1, type var2 );
//

bool functioncompare(hsnode this, hsnode that){

   if(this == NULL && that == NULL) return true;
   bool match = false;
   bool started = false;
   for(;;){
      if(!this) break;
      if(!that) break;
      if(!started){ match = true; started = true;}

      match = this->lexeme == that->lexeme &&
         this->attributes == that->attributes;

      if(!match) return false;
      this = this->param;
      that = that->param;
   }

   return match && (this == that) ;
}

//
//build a hsnode from astree nodes fn and paramlist.
//then compare this new hsnode fn against fn2
//
//Requirement: Can't touch existed idents table and block counter
//
bool matchfunction(const astree const fn, 
      const astree const paramlist, const hsnode const fn2){

   hsnode fn1 = attachparam(fn, paramlist, 1);
   bool ismatch = functioncompare(fn1,fn2);
   if(ismatch) DEBUGF('M', "MATCH %s %s \n", tostr(fn1), tostr(fn2));
   else DEBUGF('M', "NOTMATCH %s %s \n", tostr(fn1), tostr(fn2));
   //delete tmphash & clean other malloc/new at this spot
   return ismatch;
}

astree currentfunction = NULL; //processing this current function

void exitfunction(astree root){
   DEBUGF('F', "exit: %s\n", atostr(root->first->first));
   currentfunction = NULL;
   exitblock(root);
}

//
// Function and prototype is different only by a body block.
// Names of function and prototype are in global name space.
// While param and body block is in the same name space.
//
//   PROTOTYPE "<<PROTOTYPE>>" 4.8.0
//      INT "int" 4.8.0
//         DECLID "f0" 4.8.4
//      PARAM "(" 4.8.7
//   PROTOTYPE "<<PROTOTYPE>>" 4.9.0
//      INT "int" 4.9.0
//         DECLID "f1" 4.9.4
//      PARAM "(" 4.9.7
//         INT "int" 4.9.8
//            DECLID "a" 4.9.12
//
//   FUNCTION "<<FUNCTION>>" 4.14.0
//      BOOL "bool" 4.14.0
//         DECLID "empty" 4.14.5
//      PARAM "(" 4.14.11
//         TYPEID "stack" 4.14.12
//            DECLID "stack" 4.14.18
//      BLOCK "{" 4.14.25
//

void enterfunction(astree root){
   DEBUGF('F', "enter: %s\n", atostr(root->first->first));
   astree type = root->first;
   astree paramlist = type->next;
   astree ident = basetype(type);
   ident->block = GLOBAL;
   ident->attributes |= (paramlist->next) ?
                        ATTR_FUNCTION : ATTR_PROTOTYPE;

   DEBUGF('F', "FUNC: %s %s\n", atostr(type), atostr(ident));
   currentfunction = ident;

   //check whether there is an existed prototype of function
   //find(funcid)
   //    notfound:
   //       [just enter a new function into identifier table]
   //
   //    found:
   //       match(existed funcid with the new function)
   //          match:
   //             is existed funcid is a function?
   //                yes:
   //                   [print error: function redeclaration]
   //                no:
   //                   [update the funcid record]
   //
   //          notmatch:
   //             [print error: mismatch signature]
   //


   hsnode orig = find_hashstack(idents, ident->lexeme);
   if(orig){
      if(orig->attributes & ATTR_FUNCTION){     //function existed
         eprintf("%s is already defined by function:%s\n",
               atostr(ident), tostr(orig));
      }else if( ident->attributes & ATTR_PROTOTYPE ){
         if(matchfunction(ident,paramlist, orig)){
            eprintf("proto:%s is already declared %s\n",
               atostr(ident), tostr(orig));
         }else{
            eprintf("proto:%s is mismatched with a declared %s\n",
               atostr(ident), tostr(orig));
         }
      }else{ //ident is function and orig is a prototype
         ident->attributes &= ~(ATTR_FUNCTION);
         ident->attributes |= ATTR_PROTOTYPE;
         if(matchfunction(ident,paramlist, orig)){
            //update prototype ptr -> actually function obj
            orig->filenr = ident->filenr;
            orig->linenr = ident->linenr;
            orig->offset = ident->offset;
            orig->attributes &= ~(ATTR_PROTOTYPE);
            orig->attributes |= ATTR_FUNCTION;
         }else{
            eprintf("function:%s is mismatched with a prototype %s\n",
               atostr(ident), tostr(orig));
         }
         ident->attributes &= ~(ATTR_PROTOTYPE);
         ident->attributes |= ATTR_FUNCTION;
      }
      enterblock(ident);
      return;
   }

   //we'll enterblock() when attach paramlist to a function ident
   hsnode fn = attachparam(ident, paramlist, false);
   ident->sym = fn;
}


void setconst(astree root){
   bitset_t attributes;
   switch(root->symbol){
      case INTCON    : attributes = ATTR_INT; break;
      case CHARCON   : attributes = ATTR_CHAR; break;
      case STRINGCON : attributes = ATTR_STRING; break;
      case NIL       : attributes = ATTR_NULL; break;
      case TRUE      :
      case FALSE     : attributes = ATTR_BOOL; break;
   }
   root->attributes = attributes | ATTR_CONST;
}

/*----------------------------------------------------------------*/
//
//Type checking
// Bottom-up, a.k.a. post-fix processing
//

bool hasattrib( astree node, bitset_t attribute ){
   if(node->attributes & attribute) return true;
   else{
      eprintf( "%s doesn't have attribute: %s.\n",
            atostr(node), print_attributes(attribute));
      return false;
   }
}

bool hasboolean(astree node){
   if(node->attributes & ATTR_BOOL) return true;
   else{
      eprintf("A bad boolean expression %s.\n", atostr(node));
      return false;
   }
}

bool hasprimitive(astree node){
   if(is_primitive(node->attributes)) return true;
   else{
      eprintf("%s is not a primitive type.\n", atostr(node));
      return false;
   }
}

bool hasany(astree node){
   if(is_primitive(node->attributes) ||
         is_reference(node->attributes)) return true;
   else{
      eprintf("%s is not a primitive nor a reference type.\n",
            atostr(node));
      return false;
   }
}

// supported types
// bool, char, int, string, struct (user defined type), basetype []
//
bool iscompatible(astree left, astree right){
   bitset_t la = left->attributes;
   bitset_t ra = right->attributes;

   bool primitivematch =
      (is_primitive(la) && is_primitive(ra)) &&
         (  ((la & ATTR_BOOL)   &&  (ra & ATTR_BOOL))   ||
            ((la & ATTR_CHAR)   &&  (ra & ATTR_CHAR))   ||
            ((la & ATTR_INT)    &&  (ra & ATTR_INT))    ||
            ((la & ATTR_STRING) &&  (ra & ATTR_STRING))
         );

   bool structmatch = 
      ((la & ATTR_STRUCT)  == (ra & ATTR_STRUCT)) &&
         (  find_hashstack(types, left->lexeme) == 
            find_hashstack(types, right->lexeme)
         );

   bool referencecompatible =
      ((la & ATTR_NULL)  &&  is_reference(ra)) ||
      (is_reference(la)  &&  (ra & ATTR_NULL)) ;

   bool arraymatch =  (primitivematch || structmatch) &&
      ((la & ATTR_ARRAY ) && (ra & ATTR_ARRAY ));

   bool compatible = 
      primitivematch ||
      referencecompatible ||
      structmatch ||
      arraymatch ;

   if(compatible) return true;
   else{
      eprintf("%s [%s] is not compatible with %s [%s].\n",
            atostr(left), print_attributes(la),
            atostr(right), print_attributes(ra));
      return false;
   }
}

void checkequality(astree root){
   astree left = root->first;
   astree right = left->next;
   hasany(left);
   hasany(right);
   root->attributes = ATTR_BOOL | ATTR_VREG; 
}

// type checking vardecl... calls iscompatible for checking
// e.g.:
//int[] fibonacci = new int[FIB_SIZE];
//   VARDECL "=" 4.9.16
//      ARRAY "[]" 4.9.3
//         INT "int" 4.9.0
//         DECLID "fibonacci" 4.9.6
//      NEWARRAY "new" 4.9.18
//         INT "int" 4.9.22
//         INTCON "30" 4.9.26
//   '=' "=" 4.11.13
//      INDEX "[" 4.11.9
//         IDENT "fibonacci" 4.11.0
//         INTCON "0" 4.11.10
//      INTCON "0" 4.11.15
//   '=' "=" 4.12.13
//      INDEX "[" 4.12.9
//         IDENT "fibonacci" 4.12.0
//         INTCON "1" 4.12.10
//      INTCON "1" 4.12.15
//
//   VARDECL "=" 2.5.8
//      ARRAY "[]" 2.5.3
//         INT "int" 2.5.0
//         DECLID "b" 2.5.6
//      NIL "null" 2.5.10
//
//   VARDECL "=" 4.44.12
//      TYPEID "stack" 4.44.0
//         DECLID "stack" 4.44.6
//      CALL "(" 4.44.24
//         IDENT "new_stack" 4.44.14

void checkvardecl(astree root){
   astree left = root->first;
   astree right = left->next;
   if(!iscompatible(left, right)){
      eprintf("%s %s are not compatible for variable declaration \n", 
            print_attributes(left->attributes),
            print_attributes(right->attributes));
   }
}


void checkwhileif(astree root){
   astree expr = root->first;
   hasboolean(expr);
}


//check on return type vs function declaration of return type
//matched struct should be okay.
void checkreturn(astree root){
   assert(currentfunction != NULL);
   switch(root->symbol){
      case RETURNVOID:
         if(!(currentfunction->attributes & ATTR_VOID)){
            eprintf("%s is expected a void for function %s.\n", 
                  atostr(root), atostr(currentfunction));
            break;
         }

      case RETURN:
         if(!iscompatible(root->first, currentfunction->first)){
            eprintf("Mismatched return: %s function%s types\n",
                  atostr(root->first),
                  atostr(currentfunction->first));
            break;
         }
   }
}

void checkassignment(astree root){
   astree left = root->first;
   astree right = left->next;
   hasany(left);
   hasany(right);
   if(!(left->attributes & ATTR_LVALUE)){
      eprintf("%s is not lvalue.\n", atostr(root));
   }else{
      root->attributes = left->attributes | ATTR_VREG;
   }
}


//(e) Identifiers have the type attributes that they derive
//from the symbol table. In addition, either the function or
//variable attribute will be present, and for variables that are
//parameters, also the param attribute. All variables also have
//the lvalue attribute.


void checkident(astree root){
   hsnode ident = find_hashstack(idents, root->lexeme);
   if(!ident){
      eprintf("Identifier %s has not declared.\n", atostr(root));
   }else{
      eprintf("Ident found %s (%s)\n", tostr(ident),
      print_attributes(ident->attributes));
      root->attributes = ident->attributes;
      root->stypeid = (char *) ident->structnode->lexeme; 
   }
}

/*
TYPEID ‘.’ FIELD → symbol.lookup vaddr lvalue

(f) Field selection sets the selector (.) attribute as follows:
The left operand must be a struct type or an error message
is generated. Look up the field in the structure and copy its
type attributes to the selector, removing the field attribute
and adding the vaddr attribute.

*/


void checkcall(astree root){
   STUBPRINTF("check returning type%s\n", atostr(root));
}
 
//IDENT FILLED, FIELD has not filled
//  
//  example:
//  struct node { int a; int b}
//  ...
//  node somenode; 
//  ...
//  ...
//  somenode.b = 5;
//  

//stack.data = new string[size]; // Array of null pointers.
//
//
//         '=' "=" 4.15.14
//            '.' "." 4.15.8
//               IDENT "stack" 4.15.3
//               FIELD "data" 4.15.9
//            NEWARRAY "new" 4.15.16
//               STRING "string" 4.15.20
//               IDENT "size" 4.15.27

void checkselect(astree root){
   astree typeid = root->first;
   astree field = typeid->next;
   assert(field->symbol == FIELD);
   hasattrib(typeid, ATTR_STRUCT);
   hsnode structid = find_hashstack(types, typeid->stypeid);
   if(structid == NULL){
      eprintf("%s has unknown struct type %s\n", 
            atostr(typeid), typeid->stypeid);
      return;
   }

   hashstack fields = structid->fields;
   hsnode foundfield = find_hashstack(fields, field->lexeme);
   if(foundfield == NULL){
      eprintf("field %s is not in struct %s\n",
            tostr(foundfield), tostr(structid));
      return;
   }
   root->attributes = foundfield->attributes;
   root->attributes |= ATTR_VADDR;
   root->attributes &= ~ATTR_FIELD;

}

void checkindexselect(astree root){
   STUBPRINTF("check returning type%s\n", atostr(root));
}



void checkcompare(astree root){
   astree left = root->first;
   astree right = left->next;
   hasprimitive(left);
   hasprimitive(right);
   root->attributes = ATTR_BOOL | ATTR_VREG; 
}

void checkbinary(astree root){
   astree left = root->first;
   astree right = left->next;
   hasattrib(left, ATTR_INT);
   hasattrib(right, ATTR_INT);
   root->attributes = ATTR_INT | ATTR_VREG; 
}

void checkunary(astree root){
   astree child = root->first;
   switch(root->symbol){
      case NEG:
      case POS:
         hasattrib(child, ATTR_INT);
         root->attributes = ATTR_INT | ATTR_VREG; 
         break;
      case '!':
         hasattrib(child, ATTR_BOOL);
         root->attributes = ATTR_BOOL | ATTR_VREG; 
         break;
      case ORD:
         hasattrib(child, ATTR_CHAR);
         root->attributes = ATTR_INT | ATTR_VREG; 
         break;
      case CHR:
         hasattrib(child, ATTR_INT);
         root->attributes = ATTR_CHAR | ATTR_VREG; 
         break;
   }
}

void checkconst(astree root){
   setconst(root);
}


void checknew(astree root){
   root->attributes = root->first->attributes | ATTR_VREG;
}

// basetype is any type that can be used as a base type for an array

bool isbasetype(astree type){
   bitset_t attr = type->attributes;
   bool base = attr &
       (ATTR_BOOL | ATTR_CHAR | ATTR_INT| ATTR_STRING | ATTR_STRUCT)
       && !(attr |ATTR_ARRAY);
   return base;
}

//‘new’ basetype ‘[’ int ‘]’ → basetype[] vreg

void checknewarray(astree root){
   astree typeid = root->first;
   astree index = typeid->next;

   if(!isbasetype(typeid)){
      eprintf("%s is not a basetype usable for an array.\n",
            atostr(typeid));
   }
   hasattrib(index, ATTR_INT);
   root->attributes = typeid->attributes | ATTR_VREG;
}

void checknewstring(astree root){
   astree child = root->first;
   hasattrib(child, ATTR_INT);
   root->attributes = ATTR_STRING | ATTR_VREG;
}






void preproc(astree root){
   switch(root->symbol){
      case VARDECL   : vardecl(root); break;
      case STRUCT    : structblock(root); break;
      case BLOCK     : enterblock(root); break;

      case PROTOTYPE : 
      case FUNCTION  : enterfunction(root); break;

   }
}

//   sed 's/^[ \t]*//' *.ast | awk '{print $1}' |sort |uniq >symbols.txt 
//
//  '.'
//  ')'
//  ']'
//  '}'
//  '*'
//  '+'
//  ARR
//  BOOL
//  CHAR
//  DECLID
//  EQ
//  FALSE
//  FIELD
//  FUNCTION
//  GT
//  IDENT
//  INDEX
//  INT
//  NEW
//  NEWARRAY
//  NIL
//  ORD
//  PARAM
//  PROTOTYPE
//  RETURN
//  RETURNVOID
//  ROOT
//  STRING
//  STRINGCON
//  STRUCT
//  TRUE
//  TYPEID
//  VARDECL
//  VOID
//  WHILE
//  
//  should catch all of those symbols in the post processing
//  There is also a NEWSTRING that is not used in all test cases

void postproc(astree root){
   switch(root->symbol){
      case BLOCK      : exitblock(root); break;
      case PROTOTYPE  : 
      case FUNCTION   : exitfunction(root); break;

      case VARDECL    : checkvardecl(root); break;

      case WHILE      : 
      case IFELSE     : 
      case IF         : checkwhileif(root); break;

      case RETURNVOID :
      case RETURN     : checkreturn(root); break;

      case '='        : checkassignment(root); break;

      case EQ         : 
      case NE         : checkequality(root); break;

      case LT         : 
      case LE         : 
      case GT         : 
      case GE         : checkcompare(root); break;

      case '+'        : 
      case '-'        : 
      case '*'        : 
      case '/'        : 
      case '%'        : checkbinary(root); break;

      case NEG        : 
      case POS        : 
      case '!'        : 
      case ORD        : 
      case CHR        : checkunary(root); break;


      //4.4.4
      case NEW        : checknew(root); break;
      case NEWARRAY   : checknewarray(root); break;
      case NEWSTRING  : checknewstring(root); break;

                        //IDENT ‘(’ compatible ‘)’ → symbol.lookup

      case CALL       : checkcall(root); break;
      case IDENT      : checkident(root); break;

      case INDEX      : checkindexselect(root); break;

      case '.'        : checkselect(root); break;

      case INTCON     : 
      case CHARCON    : 
      case STRINGCON  : 
      case NIL        : 
      case FALSE      : 
      case TRUE       : checkconst(root); break;
   }
}


void traverse(astree node){
   if(node == NULL) return;
   for (astree child = node->first; child != NULL;
         child = child->next){
      preproc(child);
      traverse(child);
      postproc(child);
   }
}

bool is_ident(bitset_t attributes){
   return (attributes &
         (ATTR_FUNCTION | ATTR_VARIABLE | ATTR_FIELD
          | ATTR_TYPEID | ATTR_PARAM ));
}

int indentoffset = 0;
int indentstop = 0;
void printsym(astree node, int height){
   if(node == NULL) return;

   if (!indentstop) indentoffset++;
   //printf("%*s%s: {%d} %s", (height - indentoffset) * 3, "",
   //printf("%*s%s: {%d} %s", (height ) * 3, "",
    //  atostr(node), node->block,  node->sym ? "SYMBOL" : "");
   if(node->sym){
   //printf("%14s", "SYMBOL");
      indentstop = 1;
      //printf("%*s\"%s\": {%d} %s\n", (height - indentoffset) * 3, "",
      printf("%*s%s: {%d} %s\n", (height - indentoffset) * 3, "",
            atostr(node), node->block,
            print_attributes(node->attributes));

      //if(node->attributes & ATTR_FIELD){
      //   printf("

      //print_attributes (node->attributes);
      //if(node->structid){
      //   printf(" {%s}", node->structid->lexeme);
      //}
      //if(node->sym) printf(" [%s %s]",
      //    tostr(node->sym), print_attributes(node->sym->attributes));
      //printf("\n");

   }
   //printf("\n");
   for (astree child = node->first; child; child = child->next){
      //fixing struct indentation
      if(child->symbol == STRUCT) indentoffset--;
      printsym(child, height + 1);
      if(child->symbol == STRUCT) indentoffset++;
      //postproc(child);
   }
}


void buildsym(void){
   idents = new_hashstack();
   types = new_hashstack();
   pushblock(); //3.1 b) global node is set to 0.

   printf("root %p\n", yyparse_astree);
   traverse(yyparse_astree);
   printsym(yyparse_astree, 0);

   //exitblock();
}


RCSC(SYMTABLE_C,"$Id: symtable.c,v 1.19 2014-06-15 03:38:36-07 - - $")
