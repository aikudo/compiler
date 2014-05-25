#ifndef __COMPILER__STRINGTABLE_H__
#define __COMPILER__STRINGTABLE_H__

typedef struct hashtable hashtable;
hashtable *newhash (void);
void delhash (hashtable **hashset);
char *inserthash (hashtable **hashset, const char *string);
void dumphash(hashtable *hashset, unsigned char details);
#endif
