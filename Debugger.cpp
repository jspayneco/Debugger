/*
  debugger.cpp

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

#include "Arduino.h"
#include "Debugger.h"


#ifdef MIN_DEBUG

const char helloText[] PROGMEM = "\n";
const char BreakLine[] PROGMEM = "\n[Debug]> ";

const char NL[] PROGMEM = "\n";
const char CMD_NOT_RECOGNIZED[] PROGMEM = "?";

const char RUNNING[] PROGMEM = "Running...";
const char STOPPED[] PROGMEM = "Stopped...";

const char ACTIVE_BREAKPOINTS[] PROGMEM = "Act bps: ";
const char BREAKPOINTS_CLEARED[] PROGMEM = "Brks Cleared";
const char WATCH_VARS_CLEARED[] PROGMEM = "";

#else
const char helpText1[] PROGMEM = "\ndm [address (l len)] -> \"dm 300\" (displays memory contents.)\n";
const char helpText2[] PROGMEM = "bp [break point ID]  -> \"bp 3\"   (activates breakpoint(s) with ID of 3)\n";
const char helpText3[] PROGMEM = "cb                   -> \"cb\"     (clear breakpoint(s))\n";
const char helpText4[] PROGMEM = "go                   -> \"go\"     (continues program execution)\n";
const char helpText5[] PROGMEM = "st                   -> \"st\"     (stop program execution)\n";
const char helpText6[] PROGMEM = "dv [(repeat in ms)]  -> \"dv\"     (display variables)\n";

const char helloText[] PROGMEM = "----- Debugger Started.  h for help. -----\n";
const char BreakLine[] PROGMEM = "\n[Debugger Ready]> ";

const char NL[] PROGMEM = "\n";
const char CMD_NOT_RECOGNIZED[] PROGMEM = "Command Not Recognized.";

const char RUNNING[] PROGMEM = "Running...";
const char STOPPED[] PROGMEM = "Stopped...";

const char ACTIVE_BREAKPOINTS[] PROGMEM = "Active breakpoints: ";
const char BREAKPOINTS_CLEARED[] PROGMEM = "All BreakPoints Cleared";
const char WATCH_VARS_CLEARED[] PROGMEM = "All Watch Variables Cleared";

#endif



   char AtBreakPoint;
  
   char ArmedBreakPoints[MAX_BREAKPOINTS];
   DebugVar Vars[MAX_WATCHES];
  
   char Inited;


void debugger_init()
{
#if 1 
  
char tmpbuf[TMEP_BUFF_SIZE];  
  
  // initialize timer1 
  noInterrupts();           // disable all interrupts
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1  = 0;

  OCR1A = 0;              // compare match register 

  TCCR1B |= (1 << CS10);    // no prescaler 
  TIMSK1 &= ~(1 << TOIE1);   // disable timer overflow interrupt
  interrupts();             // enable all interrupts
  
  
  Serial.begin(9600);
  
#ifndef MIN_DEBUG
Serial.write(strcpy_P(tmpbuf, helloText));
#endif
Serial.write(strcpy_P(tmpbuf, BreakLine));


memset(Vars, 0, sizeof(Vars));
memset(ArmedBreakPoints, 0, sizeof(ArmedBreakPoints)); 

Inited = TRUE;
#endif
}


void addBp(char ID)
{
  if(!Inited)
    init();
  
AtBreakPoint = ID;  
}

#if 1
void addWatch(void* var, char* VarNameStr, int Type)
{
  if(!Inited)
    init();
  
  for(int i=0; i<MAX_WATCHES; i++)
    {
    if(Vars[i].VarAddress == var)
      {
      break;
      }     
    if(Vars[i].VarAddress)
      {
      continue;
      }     
    Vars[i].VarAddress = var;
    Vars[i].VarName = VarNameStr;
    Vars[i].VarType = Type;

    break;
    }
} 
#endif

void addWatchi(int* var, char* VarNameStr)
{
  addWatch((void*)var, VarNameStr, VAR_INTEGER);
} 

void addWatchc(char* str, char* VarNameStr)
{
  addWatch((void*)str, VarNameStr, VAR_STRING);
} 
 
void addWatchd(void* data, char* VarNameStr)
{
  addWatch(data, VarNameStr, VAR_DATA);
} 
 
 
 
ISR(TIMER1_OVF_vect)          // timer compare interrupt service routine
{
#if 1
  if(!Inited)
    debugger_init();

  char tmpbuf[TMEP_BUFF_SIZE];  

//check to see if this break point is armed
for(int i=0; i<MAX_BREAKPOINTS; i++)
  {
  if(ArmedBreakPoints[i] == AtBreakPoint)
    {
    Serial.write("At BP: ");
    Serial.write(itoa(AtBreakPoint, tmpbuf, 10));
    Serial.write(strcpy_P(tmpbuf, BreakLine));
    TIMSK1 &= ~(1 << TOIE1);   // disable timer overflow interrupt
    interrupts();             // enable all interrupts

    while(AtBreakPoint)
      {
      delay(200);
      serialEventRun();
      }
      
    break;
    }
  }

#endif  
}  


void serialEvent() {

if(!Inited)
  debugger_init();

char tmpbuf[TMEP_BUFF_SIZE];  

#if 1

#define CMD_LEN 10
  static char Command[CMD_LEN] = "";
  static char index = 0;
  char Error = TRUE;
  
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
  
  if(index < CMD_LEN)
    {
    Command[index++] = inChar;
    }
    
    if (inChar == '\n' || inChar == '\r') 
  {

#ifndef MIN_DEBUG
  
  /////// Display memory ///////////
  if(strstr(Command, "dm"))
    {
DM:
    int len = 16*2;
    
    char* ptrlen = strstr(Command, "l");
    if(ptrlen)
      {
      len = atoi(ptrlen+1)+15;
      }
      
    Serial.write("\n"); 
    
    char* ptr = (char*)strtol(&Command[2], NULL, 16);
    #define WIDTH_DISP_MEM 16
    for(int j=0; j<len; j+=WIDTH_DISP_MEM)
      {
      for(int i=0; i<WIDTH_DISP_MEM; i++)
        {
        sprintf(tmpbuf, "%02x", ptr[i+(j*WIDTH_DISP_MEM)]);
        tmpbuf[2] = 0;
        Serial.write(tmpbuf);
        Serial.write(' ');
        }
      
      for(int i=0; i<WIDTH_DISP_MEM; i++)
        {
        if(ptr[i]>=' ' && ptr[i+(j*WIDTH_DISP_MEM)] < 128)
          {
          Serial.write(ptr[i+(j*WIDTH_DISP_MEM)]);
          }
        else
          {
          Serial.write('.');
          }
        }
      Serial.write(strcpy_P(tmpbuf, NL));
      }
    Error = FALSE;
    Command[0] = 0;
    }
#endif

  /////// arm breakpoint ///////////
  if(strstr(Command, "bp"))
    {
    for(int i=0; i<MAX_BREAKPOINTS; i++)
      {
      if(ArmedBreakPoints[i] == atoi(&Command[2]))
        {
        break;
        }
      if(ArmedBreakPoints[i] == 0)
        {
        ArmedBreakPoints[i] = atoi(&Command[2]);
        break;
        }
      }

    Serial.write(strcpy_P(tmpbuf, ACTIVE_BREAKPOINTS));
        
    for(int i=0; i<MAX_BREAKPOINTS; i++)
      {
      if(ArmedBreakPoints[i])
        {
        Serial.write(itoa(ArmedBreakPoints[i], tmpbuf, 10));
        Serial.write(", ");
        }
      }
    Error = FALSE;
    }

  /////// clear armed breakpoints ///////////
  if(strstr(Command, "cb"))
    {
    memset(ArmedBreakPoints, 0, sizeof(ArmedBreakPoints));
    Serial.write(strcpy_P(tmpbuf, BREAKPOINTS_CLEARED));
    Error = FALSE;
    }

#if 0
  /////// clear watch variables //////////
  if(strstr(Command, "cv"))
    {
    memset(Vars, 0, sizeof(Vars));
    Serial.write(strcpy_P(tmpbuf, WATCH_VARS_CLEARED));
    Error = FALSE;
    }
#endif

  /////// display variables ///////////
  if(strstr(Command, "dv"))
    { 
    for(int i=0; i<MAX_WATCHES; i++)
      {
      if(Vars[i].VarAddress)
        {
        switch(Vars[i].VarType)
          {
            case VAR_INTEGER:
              sprintf(tmpbuf, "\n%s(@0x%x) = %i", Vars[i].VarName, Vars[i].VarAddress, *(int*)Vars[i].VarAddress);    
              Serial.write(tmpbuf);
              continue;
          

            case VAR_STRING:
            {
            char* ptr = *(int*)(int)(Vars[i].VarAddress); 
              sprintf(tmpbuf, "\n%s(@0x%x) = %s", Vars[i].VarName, ptr, ptr);    
              Serial.write(tmpbuf);
              continue;
            }
            
            case VAR_DATA:
            {
            char* ptr = *(int*)(int)(Vars[i].VarAddress); 
              sprintf(Command, "dm 0x%x", ptr);    
              
            }
#ifndef MIN_DEBUG
      goto DM;
#endif
          }
        }
      Error = FALSE;
      break;
      }
    }

#ifndef MIN_DEBUG
  /////// stop ///////////
  if(strstr(Command, "st"))
    {
    Command[0] = 0;
    AtBreakPoint = 99;
    Serial.write(strcpy_P(tmpbuf, STOPPED));
    Serial.write(strcpy_P(tmpbuf, BreakLine));
    index = 0;
    memset(Command, 0, CMD_LEN);  
    while(AtBreakPoint)
      {
      delay(2);
      serialEventRun();
      }
    //Error = FALSE;
    index = 0;
    memset(Command, 0, CMD_LEN);  
    return;
    }
#endif

  /////// go,run ///////////
  if(strstr(Command, "go"))
    {
    TIMSK1 &= ~(1 << TOIE1);   // disable timer overflow interrupt
    AtBreakPoint = 0;
    Serial.write(strcpy_P(tmpbuf, RUNNING));
    Error = FALSE;
    }

#ifndef MIN_DEBUG
  /////// help ///////////
  if(strstr(Command, "h"))
    {
    Serial.write(strcpy_P(tmpbuf, helpText1));
    Serial.write(strcpy_P(tmpbuf, helpText2));
    Serial.write(strcpy_P(tmpbuf, helpText3));
    Serial.write(strcpy_P(tmpbuf, helpText4));
    Serial.write(strcpy_P(tmpbuf, helpText5));
    Serial.write(strcpy_P(tmpbuf, helpText6));
    Error = FALSE;
    }
#endif


  if(Error)
    {
    Serial.write(strcpy_P(tmpbuf, CMD_NOT_RECOGNIZED));
    }
      
    
  Serial.write(strcpy_P(tmpbuf, BreakLine));
  
  index = 0;
  memset(Command, 0, CMD_LEN);  

    }
  }

 #endif 
 }

