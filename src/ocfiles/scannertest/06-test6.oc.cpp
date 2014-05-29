# 1 "ocfiles/06-test6.oc"
# 1 "<command-line>"
# 1 "ocfiles/06-test6.oc"


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
# 4 "ocfiles/06-test6.oc" 2

struct foo {}
struct bar {}

int f0 ();
int f1 (int a);
int f2 (int a, int b);
int f3 (string a, string b, string c);
int f4 (foo a, bar b);
string s = "a";
string[] sa = new string[10];
