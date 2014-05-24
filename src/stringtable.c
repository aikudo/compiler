#include<assert.h>
#include<time.h>
#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<stdint.h> //C99
#define HASH_OFFSET 2166136261
#define FNV_PRIME 16777619
#define ALPHA 3
//#define HASH_INITSZ 211
#define HASH_INITSZ 15 
//#define TSFILE ("/etc/dictionaries-common/words")
#define TSFILE ("words.txt")
//#define HASH_INITSZ 176001
//#define HASH_INITSZ 131071
//#define HASH_INITSZ 262143
//#define HASH_INITSZ 148757
//#define HASH_INITSZ 198343

uint32_t fnvhash (const char *string) {
   assert (string != NULL);
   uint32_t hash = HASH_OFFSET;
   for (; *string != '\0'; ++string)
      hash = (hash ^ *string) * FNV_PRIME;
   return hash;
}

size_t strhash (const char *string) {
   assert (string != NULL);
   size_t hash = 0;
   for (; *string != '\0'; ++string)
      hash = *string + (hash << 6) + (hash << 16) - hash;
   return hash;
}

typedef struct hashtable {
   struct node **table;
   size_t len;
   size_t size;
   size_t dist; //longest distance
}hashtable;

typedef struct node {
   char *item;
   int dist;
}node;

hashtable *newhash(void){
   hashtable *h = malloc(sizeof(hashtable));
   h->dist = 0;
   h->len = 0;
   h->size = HASH_INITSZ;
   h->table = calloc(h->size, sizeof(struct node *));
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

node *inserthash(hashtable **hp, char *item){
   hashtable *h = *hp;
   node **nt;
   size_t sz = h->size;
   size_t newsz = sz*2+1;
   size_t len = h->len; 
   //don't need to allocated for matched
   node *node = malloc(sizeof (node));
   //n->item = strdup(item);
   node->item = strdup(item);
   node->dist = 0;

   //if(len*2 > sz){ //always try to have at least 2x slots than elements
   if(len*ALPHA > sz){ //always try to have at least 2x slots than elements
      printf("#element %lu double %lu -> %lu\n", len, sz, newsz);
      nt = malloc(newsz * sizeof (struct node*));
      h->dist = 0;
      h->size = newsz;
      for(size_t j = 0; j < sz; j++){ //transfer old table to new
         if(h->table[j] == NULL) continue;
         //size_t hashcode = fnvhash(item, strlen(item)) % newsz;
         size_t hashcode = strhash(h->table[j]->item) % newsz;
         size_t dist = 0;
         for (size_t i = hashcode; i != hashcode - 1; i = (i + 1) % newsz){
            dist++;
            if (nt[i] == NULL){
               nt[i] = h->table[j];
               nt[i]->dist = dist;
               break;
            }
         }
         h->dist = (dist > h->dist) ? dist : h->dist;
      }
      free(h->table);
      h->table = nt;
   }

   size_t dist = 0;
   //size_t hashcode = fnvhash(item, strlen(item)) % h->size;
   size_t hashcode = strhash(item) % h->size;
   for (size_t i = hashcode; i != hashcode - 1; i = (i + 1) % h->size){
      dist++;
      if (h->table[i] == NULL){
         node->dist = dist;
         h->table[i] = node;
         h->len++;
         break;
      }
   }
   //printf("inserted %d /w hashcode %lu at distance %lu\n", item, hashcode, dist);
   h->dist = (dist > h->dist) ? dist : h->dist;
   return node;
}

#define CMAX 128
void dumphash(hashtable *h, unsigned char details){
   size_t empty = 0;
   for (size_t i = 0; i < h->size; ++i){
      if (h->table[i]){
         if (details)
            printf("dist %u table[%lu] = %s\n",
                  h->table[i]->dist, i, h->table[i]->item);
      } else empty++;
   }
   printf("size %lu, len %lu, vac %lu, dist %lu\n",
         h->size, h->len, empty, h->dist);



   int cluster[CMAX], dist;
   //memset(cluster, 0, sizeof cluster );
   bzero(cluster, sizeof cluster );
   //for(int i=0; i<CMAX; i++) cluster[i] = 0 ;
   for(size_t i = 0; i < h->len; ){
      dist = 0;
      while(h->table[i++] != NULL) dist++;
      if(dist < CMAX) cluster[dist]++;
   }

   printf("%12lu words in the hash set\n", h->len);
   printf("%12d length of the hash array\n", (int) h->size);
   for (int i = 0; i < CMAX; i++)
      if (cluster[i])
         printf("%12d clusters of size %d\n", cluster[i],  i );

}


int main (int argc, char **argv) {
   (void) argc;
   (void) argv;
   hashtable *h;
   srand(time(NULL));

   h = newhash();
   for(int i = 0; i<10; i++){

   //FILE *fp = fopen("/etc/dictionaries-common/words", "r");
   FILE *fp = fopen(TSFILE, "r");
   //FILE *fp = fopen("short.list", "r");
   //FILE *fp = fopen("100words", "r");

   //   for(size_t j = 0; j < 100000; j++){
   //for(size_t i = 0; i < 10; i++){

   char buff[80];
   while(fgets(buff, 80, fp)){
      //printf("%u\n", fnvhash(buff, strlen(buff)) % 131071 );
      inserthash(&h, buff );
   }
   //printf("distance %lu\n", h->dist);
   fclose(fp);
   //  }
   }

   dumphash(h,0);
   delhash(&h);
   return 0;
}
