# 1 "ocfiles/04-test4.oc"
# 1 "<command-line>"
# 1 "ocfiles/04-test4.oc"


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
# 4 "ocfiles/04-test4.oc" 2

struct foo {
   int a;
}

int a = 6;
foo b = new foo ();
b.a = 8;
a = a * b.a + 6;;

puti (a);
putc (' ');
puti (b.a);
endl ();
