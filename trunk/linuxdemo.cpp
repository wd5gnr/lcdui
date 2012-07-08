/* Example of using LCDUI 
   This is just a "simulation" using the console
*/

using namespace std;


#include "lcdui.h"
#include <ctype.h>

using namespace liblcdui;


// Simple sub menu */
MENU menu2[] = 
  {
        { "Entry 4.1", 1, 0, T_ACTION, NULL, 0, 0, 0, NULL, NULL, 41  },
        { "Entry 4.2", 1, 0, T_ACTION, NULL, 0, 0, 0, NULL, NULL, 42  },
	END_MENU
  };

// Values we want to remember */

int value1=5;  // simple integer
int tfval=0;   // true/false enum


// Enumeration for tfval
ENUM truefalse[] =
{
  { "FALSE"  },
  { "TRUE" },
  { NULL   }
};


// Main menu with some of each type of menu
MENU mainmenu[] =
  {
    { "Entry 1", 1, 0, T_ACTION, NULL, 0, 0, 0, NULL, NULL, 1  },
    { "Entry 2", 1, 0, T_ACTION, NULL, 0, 0, 0, NULL, NULL, 2  },
    { "Entry 3", 1, 0, T_ACTION, NULL, 0, 0, 0, NULL, NULL, 3  },
    { "Value", 1, 0, T_INT, NULL, 0, 10, 1, &value1, NULL, 1   },
    { "TF", 1, 0, T_ENUM, NULL, 0, 0, 0, &tfval, truefalse, 1   },
    { "SUBMENU", 1, 0, T_MENU, menu2,  0, 0, 0, NULL, NULL, 1  },
    END_MENU    
  };

  

// Derived class
class linuxdemo : public lcdui
{
public:
  linuxdemo(MENU *_menu) : lcdui(_menu) 
  {
    // nothing to do 
  };

  // just report to user
  void dispatch(int id) 
  {
    cout<<"Dispatch "<<id<<"\n";
  }

  // override callback for debugging
  // This would not normally be necessary
  // unless you want notification of events like
  // value editing or submenu opening
  void callback(int id, MENUTYPE mtype, EVTYPE event, int *value)
  {
    lcdui::callback(id,mtype,event,value);
    if ((mtype==T_INT || mtype==T_ENUM) && event==EV_SAVE)
      cout<<"Done editing "<<id<<"\n";
    if ((mtype==T_INT || mtype==T_ENUM) && event==EV_CANCEL)
      cout<<"Done editing (cancel) "<<id<<"\n";
  }

  // Get input (lazy, line oriented way so hit enter each time)
  INTYPE getInput(void)
  {
    int n;
    char c;
    cout<<"<U>p, <D>own, <.>OK, <X>Back - Press Enter after command: ";
    cin>>c;
    n=toupper(c);
    // U=up, D=down, .=OK, X=back
    switch (n)
      {
      case 'U': return IN_UP;
      case 'D': return IN_DN;
      case '.': return IN_OK;
      case 'X': return IN_BACK;
      }
    return IN_NONE;
  }
  
  void output(string &ostring)
  {
    // output to virtual LCD
    cout<<ostring<<" ";
  }
  
};

// Main just creates our subclass and go
int main(int argc, char *argv[])
{
  linuxdemo program(mainmenu);
  program.go();
  return 0;
}


  
