#include<assert.h>
#include<time.h>
#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<stdint.h> //C99
#define HASH_OFFSET 2166136261
#define FNV_PRIME 16777619
#define ALPHA 4 //loading factor slots/elements = 4
//#define HASH_INITSZ 211
#define HASH_INITSZ 15 
#define TSFILE ("/etc/dictionaries-common/words")
//#define TSFILE ("words.txt")
//#define TSFILE ("100words")
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
   char **table;
   size_t len;  // number of actual items
   size_t size; // number of slots
}hashtable;

hashtable *newhash (void) {
   hashtable *hp = malloc( sizeof(struct hashtable) );
   assert( hp != NULL );
   hp->len = 0;
   hp->size = HASH_INITSZ;
   hp->table = calloc( hp->size, sizeof(char *) );
   assert( hp->table != NULL );
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
      printf("#element %lu double %lu -> %lu\n", hp->len, hp->size, newsz);

      /*
      for (size_t i = 0; i < hp->size; i++){
         printf("a[%lu]: @%p \n", i, hp->table[i]);
         if( hp->table[i] ) printf("%s\n",   hp->table[i]);
      }*/

      char **oldtable = hp->table;
      char **newtable = calloc( newsz, sizeof (char*) );
      assert (newtable != NULL);
      for (size_t j = 0; j < hp->size; j++) { //transfer old table to new
         if (oldtable[j] == NULL) continue;
         //size_t hashcode = fnvhash(string, strlen(string)) % newsz;
         size_t hashidx = strhash(oldtable[j]) % newsz;
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

   size_t hashidx = strhash(string) % hp->size;
   char *inserted = NULL;
   for (size_t i = hashidx; i != hashidx - 1; i = (i + 1) % hp->size){
      if (hp->table[i] == NULL) {
         hp->table[i] = strdup (string);
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
      dist = 0;
      while(hp->table[i] != NULL){
         if (details)
            printf("array[%10lu] = %12lu \"%s\"\n",
                  i, strhash(hp->table[i]), hp->table[i]);
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
