#ifndef CMMCEasy_h
#define CMMCEasy_h

#endif
#include <Arduino.h>
#include "./CMMC_Blink.hpp"
#include "./CMMC_Interval.hpp"

class CMMCEasy
{
  private:

	public:
    CMMC_Blink blinker;
    CMMC_Interval interval;

    CMMCEasy() {
      // _ticker1 = new Ticker;
      // blinker = new CMMC_Blink;
    };

    ~CMMCEasy() {
      // delete _ticker1;
      // delete blinker;
    };


};
