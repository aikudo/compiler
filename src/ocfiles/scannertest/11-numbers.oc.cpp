# 1 "ocfiles/11-numbers.oc"
# 1 "<command-line>"
# 1 "ocfiles/11-numbers.oc"


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
# 4 "ocfiles/11-numbers.oc" 2

int number = 1;
while (number > 0) {
   puti (number);
   putc ('\n');
   number = number + number;
}
puti (number);
putc ('\n');
