
#include "hashstack.h"


#define HASH_NEW_SIZE 15
#define HSZ 10 
//let use a chaining hash table instead of linear probing like in stringset
//

hashstack *new_hashstack (void) {
   hashstack *this = calloc(1, sizeof (struct hashstack));
   assert (this != NULL);
   this->chains = calloc(HASH_NEW_SIZE, sizeof (hsnode *));
   assert (this->chains != NULL);
   this->size = HASH_NEW_SIZE;
   this->load = 0;
   DEBUGF ('H', "%p -> struct hashstack {size = %zd, chains=%p}\n",
                this, this->size, this->chains);
   return this;
}

hsnode* add_hashstack (hashstack *this, const char *item) {
   assert (this != NULL && item != NULL);
   size_t sz = this->size;
   size_t newsz = this->size * 2 + 1;
   uintptr_t hashcode = (uintptr_t) item;
   size_t i;
   if(this->load * 2 > sz){
      hsnode **oldchain = this->chains;
      hsnode **newchain = calloc(newsz, sizeof(hsnode *));
      for(i = 0 ; i < sz; ++i){
         hsnode *list = oldchain[i];
         hsnode *curr;
         while(list){
            curr = list;
            list = list->link;
            hashcode %=  newsz;
            curr->link = newchain[hashcode];
            newchain[hashcode] = curr;
         }
      }
      this->chains = newchain;
      this->size = newsz;
      free(oldchain);
   }
   hashcode  %= this->size;
   hsnode *node = calloc(1, sizeof(hsnode));
   node->lexinfo = item;
   node->link = this->chains[hashcode];
   this->chains[hashcode] = node;
   this->load++;
   return node;
}

RCSC(HASHSTACK_C,"$Id: hashstack.c,v 1.1 2014-06-06 16:42:24-07 - - $")
