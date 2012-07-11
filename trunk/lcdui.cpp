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
#include "lcdui.h"

using namespace std;

namespace liblcdui 
{
  
// This kicks off a menu (or a submenu)
// if level is zero (default) you can't go back from here
void lcdui::go(unsigned int level)
{
  // This mode is 0 for normal, 1 for changing a number, and
  // 2 for changing an enumeration
  int mode=0;  
   while (1)
    {
      // skip anything disabled
      while (menu[current].mstring && !menu[current].enabled) current++;
      // if at the end back up
      if (menu[current].mstring==NULL) 
	do 
	  {
	    current--;
	  } while (current && !menu[current].enabled);
      
      // now current is correct
      // get string

#ifndef NOSTRING
      string work=menu[current].mstring;
      ostringstream digits;
#else
      char work[81];
#endif
      // modify based on type
      switch (menu[current].menutype)
	{
	case T_INT:  // add number
#ifndef NOSTRING
	  work+="\t"; 
	  digits<<*menu[current].value;
	  work+=digits.str();
#else
	  // assume 8 bit char
	  work[0]='\t';
	  itoa(*menu[current].value,work+1,10);
#endif

	  break;
	  
	case T_ENUM:  // add enumerated value
#ifndef NOSTRING
	  work+="\t";
	  work+=+menu[current].enumeration[*menu[current].value].name;
#else
	  work[0]='\t';
	  strcpy(work+1,menu[current].enumeration[*menu[current].value].name);
#endif
	  break;
	}
      // write it
      output(work);
      // get input
      INTYPE in;
      do 
	{
	  in=getInput();
	  if (in==IN_NONE) idle(); // call idle if nothing
	} while (in==IN_NONE);
      
      // See what to do
      switch (in)
	{
	case IN_UP:  // Up arrow
	  if (mode==1)  // if modifying #
	    {
	      if (((*menu[current].value)-menu[current].step)<menu[current].min) break;
	      (*menu[current].value)-=menu[current].step;
	      callback(menu[current].id,menu[current].menutype,EV_CHANGE,menu[current].value);
	      break;
	    }
	  if (mode==2)  // if modifying an enum
	    {
	      if (*menu[current].value==0) break;
	      (*menu[current].value)--;
	      callback(menu[current].id,menu[current].menutype,EV_CHANGE, menu[current].value);
	      break;
	    }
	  // none of the above, just navigating the menu
	  if (current) current--;
	  break;
	  
	case IN_DN:  // down arrow
	  if (mode==1)  // change number
	    {
	      if (((*menu[current].value)+menu[current].step)>menu[current].max) break;
	      (*menu[current].value)+=menu[current].step;
	      callback(menu[current].id,menu[current].menutype,EV_CHANGE,menu[current].value);
	      break;
	    }
	  if (mode==2)  // change enum
	    {
	      (*menu[current].value)++;
	      if (menu[current].enumeration[*menu[current].value].name==NULL)
		(*menu[current].value)--;
	      callback(menu[current].id,menu[current].menutype,EV_CHANGE,menu[current].value);
	      break;
	    }
	  // none of the above, navigate the menu	  
	  current++;
	  break;
	  
	  // Select key either executes menu, 
	  // modifies int or enum
	  // or finishes modification
	case IN_OK:
	  if (mode)  // changing a value so exit change
	    {
	      mode=0;
	      // note we have no way to roll back with the current 
	      // scheme; changes are "hot" unless you override callback
	      callback(menu[current].id,menu[current].menutype,EV_SAVE,menu[current].value);
	      break;
	    }
	  

	  // Do a submenu
	  if (menu[current].menutype==T_MENU)
	    {
	      // remember where we are
	      MENU *parent=menu;
	      unsigned parentcur=current;
	      // go to new menu
	      menu=menu[current].submenu;
	      current=0;
	      callback(menu[current].id,menu[current].menutype,EV_ACTION,menu[current].value);
	      go(level+1);
	      // back, so restore
	      current=parentcur;
	      menu=parent;
	      break;
	    }

	  // integer and not read only
	  if (menu[current].menutype==T_INT && !menu[current].readonly)
	    {
	      callback(menu[current].id,menu[current].menutype,EV_EDIT,menu[current].value);
	      mode=1;  // start edit
	      break;
	      
	    }
	  // enum and not read only
	  if (menu[current].menutype==T_ENUM && !menu[current].readonly)
	    {
	      callback(menu[current].id,menu[current].menutype,EV_EDIT,menu[current].value);
	      mode=2;  // start edit
	      break;
	      
	    }
	  // none of the above, so must be T_ACTION	  
	  callback(menu[current].id,menu[current].menutype,EV_ACTION,menu[current].value);
	  break;
	  
	  // back button gets out of a menu;
	case IN_BACK:
	  if (mode)
	    {
	      mode=0;  // stop editing
	      callback(menu[current].id,menu[current].menutype,EV_CANCEL,menu[current].value);
	      break;  // don't leave submenu if editing
	    }
	  // note could save edited value and restore here
	  // or edit a local copy and only save on ok
	  if (level) return;  // only return if nested
	  break;

	}
    }
}

  void lcdui::callback(int id, MENUTYPE mtype, EVTYPE event, int *value)
  {
    switch (mtype)
      {
      case T_ACTION:
	dispatch(id);
	break;
	
	// A custom override could get notified when anything changes here
      case T_INT:
      case T_ENUM:
      case T_MENU:
	break;
      }
  }
  
  

}

  
  
  
 
