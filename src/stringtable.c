
#include"stringtable.h"

#define ALPHA 4 //loading factor slots/elements = 4
//#define HASH_INITSZ 1009
#define HASH_INITSZ 5 
//#define HASH_FUN(STR) (fnvhash64 (STR))
#define HASH_FUN(STR) (hhash (STR))
//#define TSFILE ("/etc/dictionaries-common/words")
#define TSFILE ("100words")

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

static char *hash_tag = "struct hashtable";

struct hashtable {
   char *tag;
   char **table;
   size_t len;  // number of actual items
   size_t size; // number of slots
};

hashtable *newhash (void) {
   hashtable *hp = malloc( sizeof(struct hashtable) );
   assert(hp != NULL);
   hp->tag = hash_tag;
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
   assert (*hashset != NULL);// && *hashset->tag == hashtag );
   //assert ( **hashset->tag == hashtag );
   //printf("debuging %s\n", string);
   hashtable *hp = *hashset;
   if ( hp->len * ALPHA > hp->size) { 
      size_t newsz = hp->size * 2 + 1;
      //printf("#element %lu double %lu -> %lu\n",
      //hp->len, hp->size, newsz);
      char **oldtable = hp->table;
      char **newtable = calloc( newsz, sizeof (char*) );
      assert (newtable != NULL);
      for (size_t j = 0; j < hp->size; j++) { //transfer to new table
         if (oldtable[j] == NULL) continue;
         size_t hashidx = HASH_FUN(oldtable[j]) % newsz;
         for (size_t i = hashidx; i != hashidx - 1;
               i = (i + 1) % newsz){
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
   for (size_t i = hashidx; i != hashidx - 1;
         i = (i + 1) % hp->size){
      if (hp->table[i] == NULL) {
         hp->table[i] = strdup (string);
         assert (hp->table[i] != NULL);
         hp->len++;
         //printf("inserted %lu %s\n", i, hp->table[i]);
         inserted = hp->table[i];
         break;
      }else if (strcmp(hp->table[i], string) == 0){  //skip duplicated
         //printf("DUP\n");
         inserted = hp->table[i];
         break;
      }
   }
   return inserted;
}

#define CMAX 128
void dumphashold(hashtable *hp, FILE *out, unsigned char details){
   int cluster[CMAX], dist;
   bzero(cluster, sizeof cluster );

   for(size_t i = 0; i < hp->len; /* i++ @ two places */ ){
      dist = 1;
      while(hp->table[i++] != NULL){
         fprintf(out, "%lu \n",i);
         if (details)
            fprintf(out, "array[%10lu] = %12lu \"%s\"\n", i,
                  (size_t) HASH_FUN(hp->table[i]), hp->table[i] );
         ++dist;
      }
      if(dist < CMAX) cluster[dist]++;
   }

   fprintf(out, "%12lu words in the hash set\n", hp->len);
   fprintf(out, "%12d length of the hash array\n", (int) hp->size);
   for (size_t i = 0; i < CMAX; i++)
      if (cluster[i])
         fprintf(out, "%12d clusters of size %lu\n", cluster[i], i );
}

void dumphash(hashtable *hp, FILE *out){
   size_t dist_max = 0;
   for(size_t i = 0 ; i< hp->size; i++){
      if(hp->table[i] != NULL){
         char labelon = 1;
         size_t dist = 0;
         do{
            if(labelon) fprintf(out, "stringset[%4lu]: ", i);
                      else fprintf(out, "          %4s   ", "");
            labelon = 0;
            fprintf (out, "%22lu %p->\"%s\"\n",
            //printf ("%22lu %p->\"%s\"",
               (size_t) HASH_FUN(hp->table[i]),
               hp->table[i], hp->table[i] );

            ++dist;
            ++i;
         }while(hp->table[i]);
         dist_max = dist_max > dist ? dist_max : dist;
      }
   }
   fprintf (out, "load_factor = %.3f\n", (float) hp->len/hp->size);
   fprintf (out, "bucket_count = %lu\n", hp->size);
   fprintf (out, "max_bucket_size = %lu\n", dist_max);
}

/*
int main (int argc, char **argv) {
   (void) argc;
   (void) argv;
   hashtable *h;
   srand(time(NULL));

   h = newhash();
   FILE *fp = fopen(TSFILE, "r");
   char buff[80];
   while(fgets(buff, 80, fp)){
      buff[strlen(buff)-1] = '\0';
      inserthash(&h, buff );
   }
   fclose(fp);

   //dumphash(h,1);
   dump(h);
   delhash(&h);
   return 0;
}
*/

RCSC(STRINGTABLE_C,"$Id: stringtable.c,v 1.1 2014-06-10 00:44:31-07 - - $")
