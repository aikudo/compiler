# 1 "ocfiles/30-fac-fnloop.oc"
# 1 "<command-line>"
# 1 "ocfiles/30-fac-fnloop.oc"





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
# 7 "ocfiles/30-fac-fnloop.oc" 2

int fac (int n) {
   int f = 1;
   while (n > 1) {
      f = f * n;
      n = n - 1;
   }
   return f;
}

int n = 1;
while (n <= 5) {
   puti (fac (n));
   endl ();
   n = n + 1;
}
