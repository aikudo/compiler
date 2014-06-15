#ifndef __HASH_STACK_H__
#define __HASH_STACK_H__

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include "auxlib.h"
typedef struct hsnode *hsnode;
typedef struct hashstack *hashstack;

struct hashstack{
   size_t size;              //number of slots
   size_t load;              //number of elements
   hsnode *chains;           //array of chain
   hsnode stack;             //itemifier stackA
   hsnode gblhead;           //ptr to global block head stack
   int block;                //global current scope
};

struct hsnode{
   const char* lexeme;       // its adddress is hashed to table

   int filenr;               // index into filename stack
   int linenr;               // line number from source code
   int offset;               // offset of token with current line
   int block;                // indicate block/scope level

   unsigned long attributes; // do i need this? dupped in AST
   hashstack fields;         // if this is struct, it has fields
   hsnode* structnode;       // if typeid is set, ptr -> struct
   hsnode param;             // a list of parameters
   hsnode next;              // a link for stack
   hsnode link;              // hash link for hash-channing
};

hashstack new_hashstack (void);
void delete_hashstack (hashstack *this); //ptr will be nulled

//Make sure the key is unique for push() & add(),
//otherwise it will return existen item.
//
//Each of the operations returns the object operated on.
//Caller can modify the content directly with the above
//non-encapsulated fields
//
//hash operations
hsnode add_hashstack (hashstack this, const char *key);

//insert a node to hash. No allocation, no new
hsnode insert_hashstack(hashstack this, hsnode node);
//
// Find an item via a key. Return NULL if not found.
//
hsnode find_hashstack (hashstack this, const char *key);

//
// Remove a node with a given key from a hash table
// Return NULL if not found. Otherwise, 
// it returns the node to be free by the caller.
//
hsnode rm_hashstack (hashstack this, const char *key);

//
// Print out stats and all keys if detail is set.
//
void print_hashstack (hashstack this, FILE *out, char detail);

//stack operations
hsnode pop_hashstack (hashstack this);
void push_hashstack (hashstack this, hsnode node);
hsnode peak_hashstack (hashstack this);
//void list_hashstack (hashstack this, FILE *out);


RCSH(HASHSTACK_H,"$Id: hashstack.h,v 1.3 2014-06-14 23:54:31-07 - - $")
#endif
