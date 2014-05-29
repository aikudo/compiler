# 1 "ocfiles/21-eratosthenes.oc"
# 1 "<command-line>"
# 1 "ocfiles/21-eratosthenes.oc"


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
# 4 "ocfiles/21-eratosthenes.oc" 2



bool[] sieve = new bool[100];
int index = 2;

while (index < 100) {
   sieve[index] = true;
   index = index + 1;
}

int prime = 2;
while (prime < 100) {
   if (sieve[prime]) {
      index = prime * 2;
      while (index < 100) {
         sieve[index] = false;
         index = index + prime;
      }
   }
   prime = prime + 1;
}

index = 2;
while (index < 100) {
   if (sieve[index]) {
      puti (index);
      endl ();
   }
}
