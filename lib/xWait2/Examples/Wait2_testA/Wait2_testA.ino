// David A. Adam
// 24 March 2015
//
// example: implement 2 non-blocking wait times

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

Wait2 outputNow (10000);  // input parameter at assignment
Wait2 printHello (3500);
  
void setup()
{
Serial.begin(9600);
}

void loop () {
    if (outputNow.ok_togo()) {Serial.print("Output Now "); Serial.println(millis());}
    if (printHello.ok_togo()) {Serial.print("Hello there "); Serial.println(millis());}
}
