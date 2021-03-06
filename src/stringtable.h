#ifndef __COMPILER__STRINGTABLE_H__
#define __COMPILER__STRINGTABLE_H__

#include <assert.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h> //C99
#include "auxlib.h"

typedef struct hashtable hashtable;
hashtable *newhash (void);
void delhash (hashtable **hashset);
char *inserthash (hashtable **hashset, const char *string);
void dumphash(hashtable *hp, FILE *out);
RCSH(STRINGTABLE_H,"$Id: stringtable.h,v 1.1 2014-06-10 00:44:31-07 - - $")
#endif
