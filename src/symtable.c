
#include "astree.h"
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

void prnerr(char *errmsg, hsnode it){
   eprintf("%s: %s\n", tostr(it), errmsg);
}

void enterblock(){ 
   pushblock();
   DEBUGF('b', "enter block%d\n", idents->block);
}

//
// exiting a block will clear out current stack frame
// collided global variables are restored back to the
// hash table.
//

void exitblock(astree root){
   popblock();
   DEBUGF('b', "exit. block is now%d on %s\n", idents->block, atostr(root));
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


bool checktype(astree type, astree ident){
   if(find_hashstack(types, type->lexeme)){
      return true;
   }else{
      eprintf("Type %s used by %s is not declared.\n",
            atostr(type), atostr(ident));
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


void setattr(hsnode node, const astree const item){
   node->filenr = item->filenr;
   node->linenr = item->linenr;
   node->offset = item->offset;
   node->block  = item->block;
   node->attributes = item->attributes;
   item->sym = node;
}

//An add wrapper for add_hashstack()
//attributes from AST is copied to SYM node
//AST node hooks to SYM node for easy printout and cleanup

hsnode add(hashstack this, const astree const item){
   hsnode node = add_hashstack(this, item->lexeme);
   setattr(node,item);
   return node;
}

void structblock(astree root){
   astree typeid = root->first;
   DEBUGF('s', "struct %s\n",typeid->lexeme);
   hsnode found = find_hashstack(types, typeid->lexeme);

   if(found && found->fields){
      prndup("struct", found, typeid);
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
         prndup("field", orig, field);
      }else{
         hsnode addedfield = add(fields, field);
         push_hashstack(fields, addedfield);
      }
   }
   setattr(found, typeid);
   found->fields = fields;
}
//
//Functions and variables share a same name space.
//Functions are global, while variables can be global or local.
//Global can't overwrite global.
//Local variable can shadow global variable ONLY.
//Local can't be collided with upper scope declaration.
//

void vardecl (astree root){
   astree type = root->first;
   astree ident = basetype(type);
   DEBUGF('v', "bid:%d %s %s\n",
         topblock(), type->lexeme, ident->lexeme);
   hsnode orig = find_hashstack(idents, ident->lexeme);
   if(orig){
      if(orig->block != GLOBAL || 
            (orig->block == GLOBAL && idents->block == GLOBAL)){
         prndup("vardecl", orig, ident);
         return;
      }else{ //shadow the global variable
         hsnode globalident = rm_hashstack(idents, orig->lexeme);
         push_hashstack(idents, globalident); //save global ident
      }
   }
   ident->attributes |= ATTR_LVALUE | ATTR_VARIABLE;
   ident->block = topblock();  //block-stamp the ident
   hsnode newident = add(idents, ident);
   push_hashstack(idents, newident);
}


hsnode attachparam(astree fn, astree paramlist, bool tmpscope){

   hashstack tmphash = tmpscope ? new_hashstack() : idents;
   hsnode fn1 = add(tmphash, fn);

   if(!tmpscope) enterblock();
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
// A function has its name at the head of the list
// subsequence nodes are parameters.
// Match the names and attributes.
//
// Note: prototype has to be in this form because of grammar
//    fun|proto -> identdecl ( identdecl* ) block
//    block -> { statements* } | ;
// e.g.: type proto ( type var1, type var2 );
//
// It's an error without variable names. It'll catch by parser.
//
//

bool match(hsnode this, hsnode that){
//   for(; this && that; this = this->param, that = that->param){
//      match = this->lexeme == that->lexeme &&
//         this->attributes == that->attributes;
//      if(!match) return false;

//   hsnode it = this;
//   printf("this: ");
//   while (it){
//      printf("%s ", tostr(it));
//      print_attributes(it->attributes);
//      printf("\n");
//      it = it->param;
//   }
//
//   it = that;
//   printf("that: ");
//   while (it){
//      printf("%s ", tostr(it));
//      print_attributes(it->attributes);
//      printf("\n");
//      it = it->param;
//   }
//


   if(this == NULL && that == NULL) return true;
   bool match = false;
   bool started = false;
   int i = 0;
   for(;;){
      if(!this) break;
      if(!that) break;
      if(!started){ match = true; started = true;}

      match = this->lexeme == that->lexeme &&
         this->attributes == that->attributes;

//      printf ("%d:%d check %s: %s ", i++, match, 
//            tostr(this), tostr(that));
//      print_attributes(this->attributes);
//      print_attributes(that->attributes);
//      printf("\n");
//      printf( "details %p %p %lu %lu\n",
//            this->lexeme, that->lexeme,
//            this->attributes, that->attributes );

      if(!match) return false;
      this = this->param;
      that = that->param;
   }

   (void)i;
   return match && (this == that) ;
}

//
//build a hsnode from astree nodes fn and paramlist.
//then compare this new hsnode fn against fn2
//
//Requirement: Can't touch existed idents table and block counter
bool matchfunction(const astree const fn, 
      const astree const paramlist, const hsnode const fn2){

   hsnode fn1 = attachparam(fn, paramlist, 1);
   bool ismatch = match(fn1,fn2);
   if(ismatch) DEBUGF('M', "MATCH %s %s \n", tostr(fn1), tostr(fn2));
   else DEBUGF('M', "NOTMATCH %s %s \n", tostr(fn1), tostr(fn2));
   //delete tmphash & clean other malloc/new
   return ismatch;
}

//
// Function and prototype is different only by a body block.
// Names of function and prototype are in global name space.
// While param and body block is in the same name space.
//


void function(astree root){
   astree type = root->first;
   astree paramlist = type->next;
   astree ident = basetype(type);
   ident->block = GLOBAL;
   ident->attributes |= (paramlist->next) ?
                        ATTR_FUNCTION : ATTR_PROTOTYPE;

   DEBUGF('F', "FUNC: %s %s\n", atostr(type), atostr(ident));


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
      }else{ //ident is function % orig is a prototype
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
      enterblock();
      return;
   }
   hsnode fn = attachparam(ident, paramlist, false);
   ident->sym = fn;
}

void proto( astree root){
   DEBUGF('F',"enter proto %s\n", atostr(root));
   function(root);
}

void proto_exit( astree root){
   DEBUGF('F',"exit proto %s\n", atostr(root));
   exitblock(root);//this doesn't work
}

void func( astree root){
   DEBUGF('F',"enter func %s\n", atostr(root));
   function(root);
}

void func_exit( astree root){
   DEBUGF('F',"exit func %s\n", atostr(root));
   exitblock(root);
}


void preproc(astree root){
   switch(root->symbol){
      case BLOCK:
         enterblock();
         break;
      case VARDECL:
         vardecl(root);
         break;
      case STRUCT:
         structblock(root);
         break;
      case PROTOTYPE:
         proto(root);
         break;
      case FUNCTION:
         func(root);
         break;
   }
}

void postproc(astree root){
   switch(root->symbol){
      case BLOCK:
         exitblock(root);
         break;

      case PROTOTYPE:
         proto_exit(root);
         break;
      case FUNCTION:
         func_exit(root);
         break;

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
      //      tostr(node->sym), print_attributes(node->sym->attributes));
      //printf("\n");

   }
   //printf("\n");
   for (astree child = node->first; child; child = child->next){
      if(child->symbol == STRUCT) indentoffset--; //fixing struct indentation
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


RCSC(SYMTABLE_C,"$Id: symtable.c,v 1.12 2014-06-11 15:38:49-07 - - $")
