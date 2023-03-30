// David A. Adam
// 24 March 2015
// version 1.0
//

#include "Arduino.h"
#include "Wait2.h"

    Wait2::Wait2 (unsigned long x) {   // use a constructor
      delaytime = x; 
      currenttime = millis();
      priortime = currenttime;
      counter = 0;
    }   //  end set_value
	
    boolean Wait2::ok_togo () {
      currenttime = millis();
      if (currenttime - priortime >= delaytime)
           {priortime = currenttime; 
	   //Serial.print("Loops "); 
	   //Serial.println(counter); 
	   counter =0; return (true);}
      else {counter++; return(false);}
    }  // end ok_togo
	
	
