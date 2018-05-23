#ifndef CMMC_SENSOR_H
#define CMMC_SENSOR_H
//abstract class Sensor
#include <CMMC_Interval.hpp>
class CMMC_Sensor {
    public:
		typedef std::function<void(void *, size_t len)> callback_t;
        inline virtual void setup(int a =0, int b=0, int c=0, int d=0){ /*nothing*/ }; 
        virtual int read(uint32_t every, callback_t) = 0; 
	protected:
		CMMC_Interval interval;
};
#endif