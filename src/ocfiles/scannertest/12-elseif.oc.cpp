# 1 "ocfiles/12-elseif.oc"
# 1 "<command-line>"
# 1 "ocfiles/12-elseif.oc"


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
# 4 "ocfiles/12-elseif.oc" 2

int a = 3;

if (a == 1) puts ("one");
else if (a == 2) puts ("two");
else if (a == 3) puts ("three");
else puts ("many");
endl ();
