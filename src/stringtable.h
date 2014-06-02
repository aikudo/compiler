#ifndef __COMPILER__STRINGTABLE_H__
#define __COMPILER__STRINGTABLE_H__

typedef struct hashtable hashtable;
hashtable *newhash (void);
void delhash (hashtable **hashset);
char *inserthash (hashtable **hashset, const char *string);
void dumphash(hashtable *hp, FILE *out);
#endif
/* $Id: stringtable.h,v 1.1 2014-06-02 04:56:59-07 - - $ */
