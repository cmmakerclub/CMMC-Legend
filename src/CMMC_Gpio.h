#include <CMMC_Sensor.hpp>


class CMMC_Gpio { 
  public:
    void setup() {
      pinMode(15, OUTPUT);
      digitalWrite(15, HIGH);
    }

    void on() {
      digitalWrite(15, HIGH); 
    }
    
    void off() {
      digitalWrite(15, LOW); 
    }
};

