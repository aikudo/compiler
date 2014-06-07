#ifndef __HASH_STACK_H__
#define __HASH_STACK_H__

#include <stdlib.h>
#include <stdint.h> //for int types
#include <stdio.h>
#include <assert.h>
#include "auxlib.h"

typedef struct hsnode hsnode;
typedef struct hashstack{
   size_t size;            //number of slots
   size_t load;            //number of elements
   hsnode **chains;
   hsnode *stack;          //identifier stackA
   hsnode *gblhead;        //ptr to global stack block
}hashstack;

struct hsnode{
   const char* lexinfo;                // get from stringtable
   union{
      uint16_t attribs;
      uint16_t isvoid:1;
      uint16_t isbool:1;
      uint16_t ischar:1;
      uint16_t isint:1;
      uint16_t isnull:1;
      uint16_t isstring:1;
      uint16_t isstruct:1;
      uint16_t isfunction:1;
      uint16_t isvariable:1;
      uint16_t isfield:1;
      uint16_t istypeid:1;
      uint16_t isparam:1;
      uint16_t islvalue:1;
      uint16_t isconst:1;
      uint16_t isvreg:1;
      uint16_t isvaddr:1;
   }at;

   hashstack *structtablenode;   //TODO: if typeid is is present
   hashstack *fieldtable;        

   int filenr;                   // index into filename stack
   int linenr;                   // line number from source code
   int offset;                   // offset of token with current line
   int blocknr;                  // indicate block/scope level

   struct hsnode *paramlist;     // a list of parameters
   struct hsnode *link;          // hash link for channing
   struct hsnode *stacknext;     // a link for stack
};


hsnode* add_hashstack (hashstack *this, const char *ident);
hsnode* find_hashstack (hashstack *this, const char *ident);
hsnode* rm_hashstack (hashstack *this, const char *ident);

hsnode* push_hashstack (hashstack *this, const hsnode *item);
hsnode* pop_hashstack (hashstack *this);
RCSH(HASHSTACK_H,"$Id: hashstack.h,v 1.1 2014-06-06 16:42:24-07 - - $")
#endif
