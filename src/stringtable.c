#include<time.h>
#include<stdlib.h>
#include<stdio.h>
#include<stdint.h> //C99
#define HASH_OFFSET 2166136261
#define FNV_PRIME 16777619
#define HASH_INITSZ 211
//#define HASH_INITSZ 23

uint32_t fnvhash (void *key, size_t len) {
   uint8_t *p = key;
   uint32_t hash = HASH_OFFSET;
   for (size_t i = 0; i < len; i++) hash = (hash ^ p[i]) * FNV_PRIME;
   return hash;
}

typedef struct hashtable {
   struct node **table;
   size_t len;
   size_t size;
   size_t dist; //longest distance
}hashtable;

typedef struct node {
   int item;
}node;

hashtable *newhash(void){
   hashtable *h = malloc(sizeof(hashtable));
   h->len = 0;
   h->size = HASH_INITSZ;
   h->table = calloc(h->size, sizeof(node *));
   return h;
}

void delhash(hashtable **hp){
   hashtable *h = *hp;
   for (size_t i=0; i<h->size; i++){
      free(h->table[i]);
      //free h->table[i].item;
   }
   free (h->table);
   free (h);
   h = NULL;
}

node *inserthash(hashtable **hp, int item){
   hashtable *h = *hp;
   node *n = malloc(sizeof (node));
   n->item = item;
   size_t hashcode = item % h->size;

   size_t dist = 0;
   for (size_t i = hashcode; i != hashcode - 1; i = (i + 1) % h->size){
      dist++;
      if (h->table[i] == NULL){
         h->table[i] = n;
         break;
      }
   }
   printf("inserted %d /w hashcode %lu at distance %lu\n", item, hashcode, dist);

   h->dist = (dist > h->dist) ? dist : h->dist;
   return n;
}

void dumphash(hashtable *h){
   for (size_t i = 0; i < h->size; ++i){
      if (h->table[i]){
         printf("table[%lu] = %d\n", i, h->table[i]->item);
      }
   }
}


int main (int argc, char **argv) {
   (void) argc;
   (void) argv;
   hashtable *h = newhash();
   srand(time(NULL));

   for(size_t i = 0; i < 100; i++){
      inserthash(&h, rand());
   }

   dumphash(h);
   delhash(&h);
   return 0;
}
