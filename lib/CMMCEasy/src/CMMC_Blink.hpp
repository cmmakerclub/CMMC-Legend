#ifndef CMMCBlink_h
#define CMMCBlink_h
#include <Arduino.h>
#include <Ticker.h>

class CMMC_Blink
{
	public:

    typedef enum {
      BLINK_TYPE_TICKER = 1,
      BLINK_TYPE_INTERVAL,
    } blink_t;

    static const blink_t TYPE_TICKER = BLINK_TYPE_TICKER;
    static const blink_t TYPE_INTERVAL = BLINK_TYPE_INTERVAL;


    CMMC_Blink init(blink_t type = BLINK_TYPE_TICKER) {
      if (type == BLINK_TYPE_TICKER) {
        this->_ticker = new Ticker;
        this->_ticker2 = new Ticker;
      }
      _initialized = true;
      return *this;
    }

    void setPin(uint8_t pin) {
      _ledPin = pin;
      pinMode(_ledPin, OUTPUT);
      digitalWrite(_ledPin, LOW);
    }

    CMMC_Blink(blink_t type = BLINK_TYPE_TICKER) {
      _type = type;
    };

    CMMC_Blink(Ticker *ticker) {
      _initialized = true;
      this->_ticker = ticker;
    };

    void blink(int ms, uint8_t pin) {
      this->setPin(pin);
      this->blink(ms);
    }

    void blink(int ms) {
      if (_initialized == false) return;
        if (_ledPin == 254) return;
        static int _pin = this->_ledPin;
        this->_ticker->detach();
        this->_ticker2->detach();
        static CMMC_Blink *_that = this;
        auto lambda = []() {
            _that->state = !_that->state;
            if (_that->state == LOW) {
              _that->prev_active = millis();
            }
            digitalWrite(_pin, _that->state);
        };
        // auto function  = static_cast<void (*)(int)>(lambda);
        this->_ticker->attach_ms(ms, lambda);
        this->_ticker2->attach_ms(30, []() {
          if ( (millis() - _that->prev_active) > 60) {
            _that->state = HIGH;
            digitalWrite(_pin, _that->state);
          } 
        });
        // this->_ticker->attach_ms()
      }

    private:
      unsigned int _ledPin = 254; 
      Ticker *_ticker;
      Ticker *_ticker2;
      blink_t  _type;
      uint8_t state = LOW;
      uint32_t prev_active;
      bool _initialized = false;

};


#endif
