# 1 "ocfiles/14-ocecho.oc"
# 1 "<command-line>"
# 1 "ocfiles/14-ocecho.oc"


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
# 4 "ocfiles/14-ocecho.oc" 2

string[] argv = getargv ();
int argi = 1;
while (argv[argi] != null) {
   if (argi > 1) putc (' ');
   puts (argv[argi]);
   argi = argi + 1;
}
endl ();
