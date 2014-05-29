# 1 "ocfiles/40-arraystack.oc"
# 1 "<command-line>"
# 1 "ocfiles/40-arraystack.oc"


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
# 4 "ocfiles/40-arraystack.oc" 2



struct stack {
   string[] data;
   int size;
   int top;
}

stack new_stack (int size) {
   stack stack = new stack ();
   stack.data = new string[size];
   stack.size = size;
   stack.top = (-1);
   return stack;
}

void push (stack stack, string str) {
   {if (! (stack.top < stack.size - 1)) __assert_fail ("stack.top < stack.size - 1", "ocfiles/40-arraystack.oc", 22);};
   stack.top = stack.top + 1;
   stack.data[stack.top] = str;
}

string pop (stack stack) {
   {if (! (stack.top != (-1))) __assert_fail ("stack.top != EMPTY", "ocfiles/40-arraystack.oc", 28);};
   string tmp = stack.data[stack.top];
   stack.top = stack.top - 1;
   return tmp;
}

bool empty (stack stack) {
   return stack.top == (-1);
}


string[] argv = getargv ();
stack stack = new_stack (100);

int argi = 0;
while (argv[argi] != null) {
   push (stack, argv[argi]);
   argi = argi + 1;
}

while (! empty (stack)) {
   puts (pop (stack));
   endl ();
}
