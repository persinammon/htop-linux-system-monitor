#include "ncurses_display.h"
#include "system.h"
#include "linux_parser.h"
#include "process.h"


//#define printVariableNameAndValue(x) std::cout<<"The name of variable **"<<(#x)<<"** and the value of variable is => "<<x<<"\n"
// I am talking about this piece of code ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^


int main() {
  System system;


  NCursesDisplay::Display(system);
}

