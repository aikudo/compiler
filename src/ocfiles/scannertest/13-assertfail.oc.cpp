# 1 "ocfiles/13-assertfail.oc"
# 1 "<command-line>"
# 1 "ocfiles/13-assertfail.oc"



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
# 5 "ocfiles/13-assertfail.oc" 2

puts (getargv()[0]);
puts (" was compiled ");
puts ("May 28 2014");
puts (" @ ");
puts ("21:54:30");
endl ();
{if (! ("assert" == "fail")) __assert_fail ("\"assert\" == \"fail\"", "ocfiles/13-assertfail.oc", 12);};
