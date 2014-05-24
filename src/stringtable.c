#include<stdlib.h>
#include<stdio.h>
#include<stdint.h> //C99
#define HASH_OFFSET 2166136261
#define FNV_PRIME 16777619
#define HASH_INITSZ 7

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
      //free h->table[i].item;
   }
   free (h->table);
   free (h);
   h = NULL;
}

hashtable *inserthash(hashtable **hp, int item){
   hashtable *h = *hp;
   node *n = malloc(sizeof (node));
   n->item = item;
   return h;
}


int main (int argc, char **argv) {
   (void) argc;
   (void) argv;
   return 0;
}
