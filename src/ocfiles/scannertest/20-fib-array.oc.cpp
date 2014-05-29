# 1 "ocfiles/20-fib-array.oc"
# 1 "<command-line>"
# 1 "ocfiles/20-fib-array.oc"





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
# 7 "ocfiles/20-fib-array.oc" 2


int[] fibonacci = new int[30];

fibonacci[0] = 0;
fibonacci[1] = 1;

int index = 2;
while (index < 30) {
   fibonacci[index] = fibonacci[index - 1] + fibonacci[index - 2];
   index = index + 1;
}

index = 0;
puts ("Numeri di figlio Bonacci\n");
while (index < 30) {
   puts ("fibonacci[");
   puti (index);
   puts (" = ");
   puti (fibonacci[index]);
   endl ();
   index = index + 1;
}
