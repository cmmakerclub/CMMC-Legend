#ifndef CMMC_SENSOR_H
#define CMMC_SENSOR_H
//abstract class Sensor
#include <CMMC_Interval.hpp>
class CMMC_Sensor {
    public:
		typedef std::function<void(void *)> callback_t;
        inline virtual void setup(){ /*nothing*/ }; 
        virtual int read(uint32_t every, callback_t) = 0; 
	protected:
		CMMC_Interval interval;
};
#endif