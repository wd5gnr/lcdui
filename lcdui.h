/*

lcdui - A simple framework for embedded systems with an LCD/button interface
Copyright (C) 2012 by Al Williams (al.williams@awce.com)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.


*/

#ifndef __LCDUI_H
#define __LCDUI_H
#include <iostream>
#include <sstream>
#include <string>
#include <cstdlib>


namespace liblcdui 
{
  
// The framework supports several types of menu items
// T_ACTION - Calls the dispatch function to execute a command
// T_INT - Manages an integer value associated with this item
// T_ENUM - Manages a list of things associated with this item
// T_MENU - A submenu
enum MENUTYPE 
  {
    T_ACTION,
    T_INT,
    T_ENUM,
    T_MENU
  };


// This is just an array of names used in enumerated types
// For example, "RED", "GREEN", "BLUE", NULL
struct ENUM
{
  const char *name;
};



// Input codes, Up, Down, OK, Back, and No input
enum INTYPE
  {
    IN_UP, IN_DN,
    IN_OK, IN_BACK, IN_NONE
  };


  
// Each menu is an array of these structures
struct MENU
{
  const char *mstring;  // Text to show
  int enabled:1;       // 1 if enabled
  int readonly:1;      // 1 if read only
  MENUTYPE  menutype;  // type of menu (see above)
  // The rest of these could be held in a union to save space
  // but for now at least we just waste a few bytes to make
  // life easier
  MENU *submenu;       // only for type T_MENU
  int min, max;        // minimum and maximum (only T_INT)
  // you could argue that step is unsigned
  // but that hoses the comparisons later
  int step;   // default step (only T_INT)
  int *value;         // pointer to value T_ENUM or T_INT only
  ENUM *enumeration;  // pointer to ENUM if applicable
  unsigned int id;  // used in dispatch (T_ACTION
};



class lcdui
{
 protected:
  MENU *menu;   // the current menu
  unsigned int current;  // current index in the menu
  
  public:
  // constructor
   lcdui(MENU *_menu) : menu(_menu) { current=0; };
  // execute the menu
  virtual void go(unsigned int menulevel=0);
  // override to handle T_ACTION menu items
  virtual void dispatch(int id) = 0;
  // called when no input available
  virtual void idle(void) {};
  // get an input code
  virtual INTYPE  getInput(void) = 0;
  // Write to the LCD
  virtual void output(std::string &ostring) = 0;
};
 
// Shorthand to end a menu (NULL entry)
#define END_MENU     { NULL, 1, 0, T_ACTION, NULL, 0, 0, 0, NULL, NULL, 1  }

}


#endif
