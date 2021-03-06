#include <assert.h>
#include <inttypes.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "astree.h"
#include "astree.rep.h"
#include "lyutils.h"

static char *astree_tag = "struct astree_rep";

bool is_astree (void *object) {
   astree tree = (astree) object;
   return tree != NULL && tree->tag == astree_tag;
}

astree new_astree (int symbol, int filenr, int linenr, int offset,
                   char *lexeme) {
   size_t size = sizeof (struct astree_rep);
   astree tree = calloc (1, size);
   assert (tree != NULL);
   tree->tag = astree_tag;
   tree->symbol = symbol;
   tree->filenr = filenr;
   tree->linenr = linenr;
   tree->offset = offset;
   tree->lexeme = lexeme;
   DEBUGF ('f', "malloc (%d) = %p-> %d:%d.%d: %s: %p->\"%s\"\n",
           size, tree, tree->filenr, tree->linenr, tree->offset,
           get_yytname (tree->symbol), tree->lexeme, tree->lexeme);
   return tree;
}


astree adopt (astree root, ...) {
   va_list children;
   assert (is_astree (root));
   va_start (children, root);
   for(;;) {
      astree child = va_arg (children, astree);
      if (child == NULL) break;
      assert (is_astree (child));
      if (root->last == NULL) root->first = child;
                         else root->last->next = child;
      root->last = child;
      DEBUGF ('a', "%p (%s) adopting %p (%s)\n",
              root, root->lexeme,
              child, child->lexeme);
   }
   va_end (children);
   return root;
}

astree adopt3 (astree root, astree left, astree middle, astree right) {
   return adopt (root, left, middle, right, NULL);
}

astree adopt2 (astree root, astree left, astree right) {
   return adopt (root, left, right, NULL);
}

astree adopt1 (astree root, astree child) {
   return adopt (root, child, NULL);
}

astree csym (astree root, int symbol) {
   root->symbol = symbol;
   return root;
}

astree adopt1sym (astree root, astree child, int symbol) {
   root = adopt1 (root, child);
   root->symbol = symbol;
   return root;
}


static void dump_node (FILE *outfile, astree node, int depth) {
   assert (is_astree (node));
   fprintf (outfile, "%p-> astree {%s(%d), %d:%d.%03d, %p->\"%s\",\n",
             (void*) node, get_yytname (node->symbol), node->symbol,
             node->filenr, node->linenr, node->offset,
             node->lexeme, node->lexeme);
   fprintf (outfile, "%*sfirst=%p, last=%p, next=%p}",
             depth * 3 + 12, "", (void*) node->first,
             (void*) node->last, (void*) node->next);
}

static void dump_astree_rec (FILE *outfile, astree root, int depth) {
   astree child = NULL;
   if (root == NULL) return;
   assert (is_astree (root));
   fprintf (outfile, "%*s%s ", depth * 3, "", root->lexeme);
   dump_node (outfile, root, depth);
   fprintf (outfile, "\n");
   for (child = root->first; child != NULL; child = child->next) {
      dump_astree_rec (outfile, child, depth + 1);
   }
}

static void print_astree_rec (FILE *outfile, astree root, int depth) {
   astree child = NULL;
   if (root == NULL) return;
   assert (is_astree (root));
   fprintf (outfile, "%*s%s ", depth * 3, "", get_yytname(root->symbol));
   fprintf (outfile, "\"%s\" %d.%d.%d\n",
         root->lexeme, root->filenr, root->linenr, root->offset);
   for (child = root->first; child != NULL; child = child->next) {
      print_astree_rec (outfile, child, depth + 1);
   }
}

void dump_astree (FILE *outfile, astree root, char details) {
   if(details) dump_astree_rec (outfile, root, 0);
   else print_astree_rec (outfile, root, 0);
   fflush (NULL);
}

void yyprint (FILE *outfile, unsigned short toknum, astree yyvaluep) {
   fprintf (outfile, "%d=%s)\n%*s(",
             toknum, get_yytname (toknum), 9, "");
   if (is_astree (yyvaluep)) {
      dump_node (outfile, yyvaluep, 3);
   }else{
      fprintf (outfile, "yyvaluep = %p", (void*) yyvaluep);
   }
   fflush (NULL);
}

void freeast (astree root) {
   astree child = NULL;
   if (root == NULL) return;
   assert (is_astree (root));
   for (child = root->first; child != NULL;) {
      astree asttofree = child;
      assert (is_astree (asttofree));
      child = child->next;
      freeast (asttofree);
   }
   DEBUGF ('f', "free [%X]-> %d:%d.%d: %s: %p->\"%s\")\n",
           (uintptr_t) root, root->filenr, root->linenr, root->offset,
            get_yytname (root->symbol), root->lexeme, root->lexeme);
   memset (root, 0, sizeof (struct astree_rep));
   free (root);
}

// LINTED(static unused)
RCSC(ASTREE_C,"$Id: astree.c,v 1.1 2014-06-10 00:44:31-07 - - $")
