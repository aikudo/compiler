# 1 "ocfiles/53-insertionsort.oc"
# 1 "<command-line>"
# 1 "ocfiles/53-insertionsort.oc"





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
# 7 "ocfiles/53-insertionsort.oc" 2

int strcmp (string s1, string s2) {
   int index = 0;
   bool contin = true;
   while (contin) {
      char s1c = s1[index];
      char s2c = s2[index];
      int cmp = ord s1c - ord s2c;
      if (cmp != 0) return cmp;
      if (s1c == '\0') contin = false;
      index = index + 1;
   }
   return 0;
}

void insertion_sort (int size, string[] array) {
   int sorted = 1;
   while (sorted < size) {
      int slot = sorted;
      string element = array[slot];
      bool contin = true;
      while (contin) {
         if (slot == 0) {
            contin = false;
         }else if (strcmp (array[slot - 1], element) <= 0) {
            contin = false;
         }else {
            array[slot] = array[slot - 1];
            slot = slot - 1;
         }
      }
      array[slot] = element;
      sorted = sorted + 1;
   }
}

void print_array (string label, int size, string[] array) {
   endl ();
   puts (label);
   puts (":\n");
   int index = 0;
   while (index < size) {
      puts (array[index]);
      endl ();
      index = index + 1;
   }
}

string[] argv = getargv ();
int argc = 0;
while (argv[argc] != null) argc = argc + 1;
print_array ("unsorted", argc, argv);
insertion_sort (argc, argv);
print_array ("sorted", argc, argv);
