# 1 "ocfiles/07-assert.oc"
# 1 "<command-line>"
# 1 "ocfiles/07-assert.oc"


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
# 4 "ocfiles/07-assert.oc" 2
{if (! ("null" == null)) __assert_fail ("\"null\" == null", "ocfiles/07-assert.oc", 4);};
