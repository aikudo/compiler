# 1 "ocfiles/23-atoi.oc"
# 1 "<command-line>"
# 1 "ocfiles/23-atoi.oc"


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
# 4 "ocfiles/23-atoi.oc" 2

int atoi (string str) {
   {if (! (str != null)) __assert_fail ("str != null", "ocfiles/23-atoi.oc", 6);};
   bool neg = false;
   int num = 0;
   int digit = 0;
   if (str[0] != '\0') {
      if (str[0] == '-') {
         digit = digit + 1;
         neg = true;
      }
      bool contin = true;
      while (contin) {
         if (str[digit] == '\0') {
            contin = false;
         }else {
            char c = str[digit];
            digit = digit + 1;
            if (c < '0') contin = false;
            else if (c > '9') contin = false;
            else num = num * 10 + ord c - ord '0';
         }
      }
      if (neg) num = - num;
   }
   return num;
}

int argi = 0;
string[] argv = getargv ();
while (argv[argi] != null) {
   string arg = argv[argi];
   puts (arg);
   puts (" = ");
   puti (atoi (arg));
   endl ();
}
