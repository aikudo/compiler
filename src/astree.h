#ifndef __ASTREE_H__
#define __ASTREE_H__

#include <stdbool.h>

#include "hashstack.h"
#include "auxlib.h"

typedef struct astree_rep *astree;

bool is_astree (void *object);
astree new_astree (int symbol, int filenr, int linenr, int offset,
                   char *lexinfo);
astree adopt (astree root, /*ASTree*/ ... /*, NULL */);
astree adopt1 (astree root, astree child);
astree adopt2 (astree root, astree left, astree right);
astree adopt3 (astree root, astree left, astree middle, astree right);
astree adopt1sym (astree root, astree child, int symbol);
astree csym (astree root, int symbol);
void dump_astree (FILE *outfile, astree root, char details);
void yyprint (FILE *outfile, unsigned short toknum, astree yyvaluep);
void freeast (astree tree);

#define freeast2(T1,T2) { freeast (T1); freeast (T2); }

// LINTED(static unused)
RCSH(ASTREE_H,"$Id: astree.h,v 1.1 2014-06-08 00:52:12-07 - - $")
#endif
