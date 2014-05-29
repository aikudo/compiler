# 1 "ocfiles/31-fib-2supn.oc"
# 1 "<command-line>"
# 1 "ocfiles/31-fib-2supn.oc"





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
# 7 "ocfiles/31-fib-2supn.oc" 2

int fibonacci (int n) {
   if (n < 2) return n;
   return fibonacci (n - 1) + fibonacci (n - 2);
}



int n = 0;
while (n < 10) {
   puts ("fibonacci(");
   puti (n);
   puts (" = ");
   puti (fibonacci (n));
   endl ();
}
