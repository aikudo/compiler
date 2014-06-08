#ifndef __HASH_STACK_H__
#define __HASH_STACK_H__

#include <stdlib.h>
#include <stdint.h> //for int types
#include <stdio.h>
#include <assert.h>
#include "auxlib.h"

typedef struct hsnode *hsnode;
typedef struct hashstack{
   size_t size;            //number of slots
   size_t load;            //number of elements
   hsnode *chains;
   hsnode stack;          //itemifier stackA
   hsnode gblhead;        //ptr to global stack block
}hashstack;

struct hsnode{
   const char* lexeme;                // get from stringtable
   unsigned long attributes;

   hashstack *structtablenode;   //TODO: if typeid is is present
   hashstack *fields;        

   int filenr;                   // index into filename stack
   int linenr;                   // line number from source code
   int offset;                   // offset of token with current line
   int blocknr;                  // indicate block/scope level

   struct hsnode *paramlist;     // a list of parameters
   struct hsnode *link;          // hash link for channing
   struct hsnode *stacknext;     // a link for stack
};

void print_hashstack (hashstack *this, FILE *out, char detail);
hashstack *new_hashstack (void);

hsnode add_hashstack (hashstack *this, const char *item);
hsnode find_hashstack (hashstack *this, const char *item);
hsnode rm_hashstack (hashstack *this, const char *item);

hsnode push_hashstack (hashstack *this, const hsnode *item);
hsnode pop_hashstack (hashstack *this);
RCSH(HASHSTACK_H,"$Id: hashstack.h,v 1.2 2014-06-08 03:18:56-07 - - $")
#endif
