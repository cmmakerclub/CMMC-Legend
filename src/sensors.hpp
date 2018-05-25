#include <CMMC_Sensor.hpp>


class CMMC_Gpio { 
  public:
    void setup() {
      pinMode(15, OUTPUT);
      digitalWrite(15, HIGH);
      // pinMode(2, OUTPUT);
      // digitalWrite(2, HIGH); 
    }

    void on() {
      digitalWrite(15, HIGH); 
      // digitalWrite(2, LOW); 
    }
    
    void off() {
      digitalWrite(15, LOW); 
      // digitalWrite(2, HIGH); 
    }
};

