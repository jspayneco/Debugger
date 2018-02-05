#include <Debugger.h>


void setup() {
  // put your setup code here, to run once:
  }


void loop() {
  // put your main code here, to run repeatedly:
static unsigned int Count = 0;

delay(10);
Count++;

char myString = "Test this one";
String myString2 = "this is a test";

cWATCH(myString); 
iWATCH(Count); 
dWATCH(myString2); 
BREAK(1);


delay(300);
}



