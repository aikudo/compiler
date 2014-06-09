//standard C scope testing
int foo; 
//int foo(); // failed variable
int bar(int foo){
   //int foo;  //failed with param
   foo = 3;
   { 
      int foo; 
      {
         int foo;
      }
      int foo;  //failed
   }

   return foo;
}
void main (void){ };

