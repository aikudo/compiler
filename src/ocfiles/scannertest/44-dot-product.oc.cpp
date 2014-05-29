# 1 "ocfiles/44-dot-product.oc"
# 1 "<command-line>"
# 1 "ocfiles/44-dot-product.oc"


# 1 "ocfiles/oclib.oh" 1
# 30 "ocfiles/oclib.oh"
void __assert_fail (string expr, string file, int line);


void putb (bool b);
void putc (char c);
void puti (int i);
void puts (string s);
void endl ();
int getc ();
string getw ();
string getln ();
string[] getargv ();
void exit (int status);
# 4 "ocfiles/44-dot-product.oc" 2

int dot_product (int size, int[] vec1, int[] vec2) {
   int index = 0;
   int dot = 0;
   while (index < size) {
      dot = dot + vec1[index] * vec2[index];
      index = index + 1;
   }
   return dot;
}


int[] vec1 = new int[10];
int[] vec2 = new int[10];
int i = 0;
while (i < 10) {
   vec1[i] = i + 10;
   vec2[i] = i * 10;
}
puti (dot_product (10, vec1, vec2));
endl ();
