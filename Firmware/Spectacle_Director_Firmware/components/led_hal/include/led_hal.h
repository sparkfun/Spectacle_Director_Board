#ifndef _LED_HAL_H_
#define _LED_HAL_H_

extern "C" {
	#include "esp_err.h"
	#include "driver/gpio.h"
}

class SpectacleLED {
public:
	SpectacleLED(unsigned char pin, uint32_t activeLevel = 1);
	
	esp_err_t init(void);
	
	esp_err_t on(void);
	esp_err_t off(void);
	esp_err_t blink(unsigned int period);
	
private:
	gpio_num_t _ledPin;
	uint32_t _active;
	
	esp_err_t setOutput(void);
	esp_err_t setLevel(uint32_t level);
	
	virtual void task() = 0;
	static void blinkTask(void * param);
};

#endif // _LED_HAL_H_