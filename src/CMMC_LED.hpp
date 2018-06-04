#ifndef CMMC_LED_H
#define CMMC_LED_H
#include <Arduino.h>
#include <Ticker.h>

class CMMC_LED
{
	public:

    typedef enum {
      BLINK_TYPE_TICKER = 1,
      BLINK_TYPE_INTERVAL,
    } blink_t;

    static const blink_t TYPE_TICKER = BLINK_TYPE_TICKER;
    static const blink_t TYPE_INTERVAL = BLINK_TYPE_INTERVAL;


    CMMC_LED init(blink_t type = BLINK_TYPE_TICKER) {
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

    CMMC_LED(blink_t type = BLINK_TYPE_TICKER) {
      _type = type;
    };

    CMMC_LED(Ticker *ticker) {
      _initialized = true;
      this->_ticker = ticker;
    };

    void toggle() {
      this->state = !this->state;
      digitalWrite(this->_ledPin, this->state);
    }

    void blink(uint32_t ms, uint8_t pin) {
      this->setPin(pin);
      this->blink(ms);
    }

    void detach() {
      this->_ticker->detach();
      this->_ticker2->detach();
    }

    void blink(uint32_t ms) {
      if (_initialized == false) return;
        if (_ledPin == 254) return;
        static int _pin = this->_ledPin;
        this->detach();
        delete this->_ticker;
        delete this->_ticker2; 
        this->_ticker = new Ticker;
        this->_ticker2 = new Ticker; 
        static CMMC_LED *_that = this;
        static auto lambda = []() {
            _that->state = !_that->state;
            if (_that->state == LOW) {
              _that->prev_active = millis();
            }
            digitalWrite(_pin, _that->state);
        };
        static auto wtf = []() {
          uint32_t diff = (millis() - _that->prev_active);
          if ( diff > 60L) {
            _that->prev_active = millis();
            _that->state = HIGH;
            digitalWrite(_pin, _that->state);
          } 
        };
        // auto function  = static_cast<void (*)(int)>(lambda);
        this->_ticker->attach_ms(ms, lambda);
        this->_ticker2->attach_ms(30, wtf);
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
