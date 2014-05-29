# 1 "ocfiles/42-viiiqueens.oc"
# 1 "<command-line>"
# 1 "ocfiles/42-viiiqueens.oc"


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
# 4 "ocfiles/42-viiiqueens.oc" 2


int[] board = new int[8];

bool is_safe (int newcol) {
   int col = 0;
   while (col < newcol) {
      if (board[col] == board[newcol]) return false;
      int diagonal = board[col] - board[newcol];
      if (diagonal == col - newcol) return false;
      if (diagonal == newcol - col) return false;
      col = col + 1;
   }
   return true;
}

void printqueens () {
   int col = 0;
   while (col < 8) {
      putc (chr (board[col] + ord '1'));
      col = col + 1;
   }
   putc ('\n');
}

void queens (int newcol) {
   if (newcol == 8) printqueens ();
   else {
      int row = 0;
      while (row < 8) {
         board[newcol] = row;
         if (is_safe (newcol)) queens (newcol + 1);
         row = row + 1;
      }
   }
}


queens (0);
