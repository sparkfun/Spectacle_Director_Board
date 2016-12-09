#include "led_hal.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

SpectacleLED::SpectacleLED(unsigned char pin, uint32_t activeLevel)
{
	_ledPin = (gpio_num_t)pin;
	_active = activeLevel;
}

esp_err_t SpectacleLED::init(void)
{
	esp_err_t retVal;
	
	retVal = setOutput();
	if (retVal == ESP_OK)
	{
		retVal = setLevel(!_active);
	}
	
	return retVal;
}

esp_err_t SpectacleLED::on(void)
{
	return setLevel(_active);
}

esp_err_t SpectacleLED::off(void)
{
	return setLevel(!_active);
}

esp_err_t SpectacleLED::blink(unsigned int period)
{
	xTaskCreate(&blinkTask, "blinkTask", 512, NULL, 5, NULL);
	
	return ESP_OK;
}

esp_err_t SpectacleLED::setOutput(void)
{
	return gpio_set_direction(_ledPin, GPIO_MODE_OUTPUT);
}

esp_err_t SpectacleLED::setLevel(uint32_t level)
{
	return gpio_set_level(_ledPin, level);
}

void SpectacleLED::blinkTask(void * param)
{
	static_cast(param)->task();
	while (1)
	{
		//gpio_set_level(_ledPin, 1);//this->on();
		//vTaskDelay(1000 / portTICK_RATE_MS);
		//this->off();
		//vTaskDelay(1000 / portTICK_RATE_MS);
	}
}