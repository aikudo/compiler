#include "hashstack.h"


#define HASH_NEW_SIZE 15
#define HSZ  1000

hashstack new_hashstack (void) {
   hashstack this = calloc(1, sizeof (struct hashstack));
   assert (this != NULL);
   this->chains = calloc(HASH_NEW_SIZE, sizeof (hsnode *));
   assert (this->chains != NULL);
   this->size = HASH_NEW_SIZE;
   this->load = 0;
   DEBUGF ('H', "%p -> struct hashstack {size = %zd, chains=%p}\n",
                this, this->size, this->chains);
   return this;
}

hsnode find_hashstack (hashstack this, const char *item){
   assert (this != NULL && item != NULL);
   uintptr_t hashcode = (uintptr_t) item % this->size;
   hsnode list = this->chains[hashcode];
   while(list){
      if(list->lexeme == item) break;
      else list = list->link;
   }
   return list;
}



hsnode add_hashstack (hashstack this, const char *item) {
   assert (this != NULL && item != NULL);
   DEBUGF ('H', "adding %p %s\n", this, item);
   hsnode found; //no dup hash
   if ( (found = find_hashstack(this, item) ) != NULL ) return found;
   size_t sz = this->size;
   size_t newsz = this->size * 2 + 1;
   uintptr_t hashcode;
   size_t i;
   if(this->load * 2 > sz){
      hsnode *oldchain = this->chains;
      hsnode *newchain = calloc(newsz, sizeof(hsnode));
      assert (newchain != NULL);
      for(i = 0 ; i < sz; ++i){
         hsnode list = oldchain[i];
         hsnode curr;
         while(list){
            curr = list;
            list = list->link;
            hashcode =  (uintptr_t) curr->lexeme % newsz;
            curr->link = newchain[hashcode];
            newchain[hashcode] = curr;
         }
      }
      this->chains = newchain;
      this->size = newsz;
      free(oldchain);
   }
   hashcode  = (uintptr_t) item % this->size;
   hsnode node = calloc(1, sizeof(struct hsnode));
   assert (node != NULL);
   node->lexeme = item;
   node->link = this->chains[hashcode];
   this->chains[hashcode] = node;
   this->load++;
   return node;
}

hsnode insert_hashstack(hashstack this, hsnode node){
   assert (this !=NULL && node != NULL);
   uintptr_t hashcode  = (uintptr_t) node->lexeme % this->size;
   node->link = this->chains[hashcode];
   this->chains[hashcode] = node;
   this->load++;
   return node;
}


void print_hashstack (hashstack this, FILE *out, char detail) {
   assert (this != NULL && out != NULL);
   size_t i;
   unsigned histo[HSZ];
   for(i = 0; i<HSZ; i++) histo[i] = 0;
   for(i = 0; i < this->size ; ++i){
      hsnode list = this->chains[i];
      int len = 0;
      while(list){
         if(detail)
            fprintf(out, "array[%10lu] = %12p   \"%s\"\n",
                  i, list->lexeme, list->lexeme);
         list = list->link;
         len++;
      }
      assert(len < HSZ);
      histo[len]++;
   }
   fprintf(out, "%10lu lexemes in the hash set\n", this->load);
   fprintf(out, "%10lu size of the hash array\n", this->size);
   for(i = 1; i<HSZ; i++){
      if(histo[i])
         fprintf(out, "%10d chains of size%3lu\n", histo[i], i );
   }
}

hsnode rm_hashstack (hashstack this, const char *key){
   assert (this != NULL);
   uintptr_t hashcode = (uintptr_t) key % this->size;
   hsnode itor = this->chains[hashcode];
   hsnode prev;
   while(itor){
      if(itor->lexeme == key) break;
      else{
         prev = itor;
         itor = itor->link;
      }
   }
   if(itor != NULL){
      if(itor == this->chains[hashcode])
         this->chains[hashcode] = itor->link;
      else if(itor->link == NULL) prev->link = NULL;
      else prev->link = itor->link;
      this->load--;
   }
   return itor;
}

hsnode pop_hashstack (hashstack this){
   assert(this != NULL);
   hsnode node = this->stack;
   if(node != NULL) this->stack = node->next;
   return node;
}

void push_hashstack (hashstack this, hsnode node){
   assert (this != NULL && node != NULL);
   node->next = this->stack;
   this->stack = node;
}

hsnode peak_hashstack (hashstack this){ 
   assert(this != NULL);
   return (this->stack);
}

RCSC(HASHSTACK_C,"$Id: hashstack.c,v 1.4 2014-06-09 23:40:22-07 - - $")
