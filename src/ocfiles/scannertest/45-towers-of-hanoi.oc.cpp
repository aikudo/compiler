# 1 "ocfiles/45-towers-of-hanoi.oc"
# 1 "<command-line>"
# 1 "ocfiles/45-towers-of-hanoi.oc"


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
# 4 "ocfiles/45-towers-of-hanoi.oc" 2

void move (string src, string dst) {
   puts ("Move a disk from ");
   puts (src);
   puts (" to ");
   puts (dst);
   puts (".\n");
}

void towers (int ndisks, string src, string tmp, string dst) {
   if (ndisks < 1) return;
   towers (ndisks - 1, src, dst, tmp);
   move (src, dst);
   towers (ndisks - 1, tmp, src, dst);
}

towers (4, "Source", "Temporary", "Destination");
