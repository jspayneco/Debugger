/*
  debugger.h

  Copyright (c) 2018, Stephen Payne

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef DEBUG_H_
#define DEBUG_H_

#include "Arduino.h"

#if defined(ARDUINO_ARCH_AVR)
  // AVR-specific code
#elif defined(ARDUINO_ARCH_SAM)
  #error "not tested with SAM-specific boards"
#else
  // generic, non-platform specific code
#endif

#define VAR_INTEGER 1
#define VAR_STRING 2
#define VAR_DATA 3


#ifdef MIN_DEBUG
#define TMEP_BUFF_SIZE 20
#else
#define TMEP_BUFF_SIZE 100
#endif

#ifndef MAX_BREAKPOINTS
#define MAX_BREAKPOINTS 4
#endif

#ifndef MAX_WATCHES
#define MAX_WATCHES 8
#endif

#define FALSE 0
#define TRUE 1



typedef struct {
void* VarAddress;
char* VarName;
char VarType;
//char GroupID;
}DebugVar;


   void debugger_init();
	 void addBp(char ID = 0);
   void addWatchi(int* var,  char*  VarNameStr);
	 void addWatchc(char* str,  char*  VarNameStr);
	 void addWatchd(void* data,  char*  VarNameStr);
	 void addWatch(void* var, char*   VarNameStr, int Type);






#define iSPLIT(a) &a, (char*)#a
#define cSPLIT(a) &a, (char*)#a
#define dSPLIT(a) &a, (char*)#a
#define WATCH(a) addWatchi(iSPLIT(a)); 
#define iWATCH(a) addWatchi(iSPLIT(a)); 
#define cWATCH(a) addWatchc(cSPLIT(a)); 
#define dWATCH(a) addWatchd(dSPLIT(a)); 


#define BREAK(a) addBp(a);\
TIMSK1 |= (1 << TOIE1);   // enable timer overflow interrupt\
asm( "sei\n" \
"sleep\n"); \


#endif


