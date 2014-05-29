# 1 "ocfiles/41-linkedstack.oc"
# 1 "<command-line>"
# 1 "ocfiles/41-linkedstack.oc"


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
# 4 "ocfiles/41-linkedstack.oc" 2

struct node {
   string data;
   node link;
}

struct stack {
   node top;
}

bool empty (stack stack) {
   {if (! (stack != null)) __assert_fail ("stack != null", "ocfiles/41-linkedstack.oc", 15);};
   return stack.top == null;
}

stack new_stack () {
   stack stack = new stack ();
   stack.top = null;
   return stack;
}

void push (stack stack, string str) {
   {if (! (stack != null)) __assert_fail ("stack != null", "ocfiles/41-linkedstack.oc", 26);};
   node tmp = new node ();
   tmp.data = str;
   tmp.link = stack.top;
   stack.top = tmp;
}

string pop (stack stack) {
   {if (! (stack != null)) __assert_fail ("stack != null", "ocfiles/41-linkedstack.oc", 34);};
   {if (! (! empty (stack))) __assert_fail ("! empty (stack)", "ocfiles/41-linkedstack.oc", 35);};
   string tmp = stack.top.data;
   stack.top = stack.top.link;
   return tmp;
}



string[] argv = getargv ();
stack stack = new_stack ();
int argi = 0;

while (argv[argi] != null) {
   push (stack, argv[argi]);
   argi = argi + 1;
}

while (! empty (stack)) {
   puts (pop (stack));
   endl ();
}
