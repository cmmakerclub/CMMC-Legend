#ifndef CMMC_SENSOR_H
#define CMMC_SENSOR_H
//abstract class Sensor
#include <CMMC_Interval.hpp>
class CMMC_Sensor {
    public:
		typedef std::function<void(void *, size_t len)> callback_t;
        inline virtual void setup(int a =0, int b=0, int c=0, int d=0){ /*nothing*/ }; 
        virtual void read()=0; 
		void onData(callback_t cb) {
			this->cb = cb; 
		}
		void every(uint32_t ms) {
			this->everyMs = ms;
		}
	protected:
		callback_t cb;
		CMMC_Interval interval;
		uint32_t everyMs;
};
#endif