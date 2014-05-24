#include<stdlib.h>
#include<stdio.h>
#include<stdint.h> //C99
#define HASH_OFFSET 2166136261
#define FNV_PRIME 16777619
uint32_t fnvhash (void *key, int len) {
   unsigned char *p = key;
   uint32_t hash = HASH_OFFSET;
   int i;
   i++;
   for (i = 0; i < len; i++) hash = (hash ^ p[i]) * FNV_PRIME;
   return hash;
}

int main (int argc, char **argv) {
   return 0;
}
