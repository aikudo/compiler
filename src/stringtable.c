#include<assert.h>
#include<time.h>
#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<stdint.h> //C99
#include"stringtable.h"
#define ALPHA 4 //loading factor slots/elements = 4
//#define HASH_INITSZ 1009
#define HASH_INITSZ 211
//#define HASH_FUN(STR) (fnvhash64 (STR))
#define HASH_FUN(STR) (hhash (STR))
#define TSFILE ("/etc/dictionaries-common/words")

uint32_t fnvhash32 (const char *string) { //FNV
   assert (string != NULL);
   uint32_t hash = 2166136261u;
   for (; *string != '\0'; ++string)
      hash = (hash ^ *string) * 16777619u;
   return hash;
}

uint64_t fnvhash64 (const char *string) { //FNV
   assert (string != NULL);
   uint64_t hash = 1099511628211u;
   for (; *string != '\0'; ++string)
      hash = (hash ^ *string) * 14695981039346656037u;
   return hash;
}

size_t hhash (const char *string) { //Horner's method
   assert (string != NULL);
   size_t hash = 0;
   for (; *string != '\0'; ++string)
      hash = *string + (hash << 6) + (hash << 16) - hash;
   return hash;
}

struct hashtable {
   char **table;
   size_t len;  // number of actual items
   size_t size; // number of slots
};

hashtable *newhash (void) {
   hashtable *hp = malloc( sizeof(struct hashtable) );
   assert(hp != NULL);
   hp->len = 0;
   hp->size = HASH_INITSZ;
   hp->table = calloc( hp->size, sizeof(char *) );
   assert(hp->table != NULL);
   return hp;
}

void delhash (hashtable **hashset) {
   hashtable *hp = *hashset;
   for (size_t i = 0; i < hp->size; i++)
      if (hp->table[i] != NULL) free(hp->table[i]);
   free (hp->table);
   free (hp);
   hp = NULL;
}

char *inserthash (hashtable **hashset, const char *string) {
   hashtable *hp = *hashset;
   if ( hp->len * ALPHA > hp->size) { 
      size_t newsz = hp->size * 2 + 1;
      //printf("#element %lu double %lu -> %lu\n", hp->len, hp->size, newsz);
      char **oldtable = hp->table;
      char **newtable = calloc( newsz, sizeof (char*) );
      assert (newtable != NULL);
      for (size_t j = 0; j < hp->size; j++) { //transfer to new table
         if (oldtable[j] == NULL) continue;
         size_t hashidx = HASH_FUN(oldtable[j]) % newsz;
         for (size_t i = hashidx; i != hashidx - 1; i = (i + 1) % newsz){
            if (newtable[i] == NULL) {
               newtable[i] = oldtable[j];
               break;
            }
         }
      }
      free(hp->table);
      hp->size = newsz;
      hp->table = newtable ;
   }

   size_t hashidx = HASH_FUN(string) % hp->size;
   char *inserted = NULL;
   for (size_t i = hashidx; i != hashidx - 1; i = (i + 1) % hp->size){
      if (hp->table[i] == NULL) {
         hp->table[i] = strdup (string);
         assert (hp->table[i] != NULL);
         hp->len++;
         inserted = hp->table[i];
         break;
      }else if (strcmp(hp->table[i], string) == 0){  //skip duplicated
         inserted = hp->table[i];
         break;
      }
   }
   return inserted;
}

#define CMAX 128
void dumphash(hashtable *hashset, unsigned char details){
   hashtable *hp = hashset;
   int cluster[CMAX], dist;
   bzero(cluster, sizeof cluster );

   for(size_t i = 0; i < hp->len; /* i++ @ two places */ ){
      dist = 1;
      while(hp->table[i] != NULL){
         if (details)
            printf("array[%10lu] = %12lu \"%s\"\n",
                  i, (long unsigned int) HASH_FUN(hp->table[i]), hp->table[i]);
         ++dist;
         ++i;
      }
      if(dist < CMAX) cluster[dist]++;
      ++i;
   }

   printf("%12lu words in the hash set\n", hp->len);
   printf("%12d length of the hash array\n", (int) hp->size);
   for (size_t i = 0; i < CMAX; i++)
      if (cluster[i])
         printf("%12d clusters of size %lu\n", cluster[i], i );
}

/*
int main (int argc, char **argv) {
   (void) argc;
   (void) argv;
   hashtable *h;
   srand(time(NULL));

   h = newhash();
   for(int i = 0; i<10; i++){
      FILE *fp = fopen(TSFILE, "r");
      char buff[80];
      while(fgets(buff, 80, fp)) inserthash(&h, buff );
      fclose(fp);
   }

   dumphash(h,0);
   delhash(&h);
   return 0;
}
*/
