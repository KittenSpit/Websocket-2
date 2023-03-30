// David A. Adam
// 24 March 2015
// version 1.0
//

using namespace std;
#ifndef Wait2_h
#define Wait2_h

#include "Arduino.h"

class Wait2 {
    unsigned long priortime;
    unsigned long currenttime;
    unsigned long delaytime;
    unsigned long counter;
  public:
    Wait2 (unsigned long x) ;
    boolean ok_togo ();
};
#endif
