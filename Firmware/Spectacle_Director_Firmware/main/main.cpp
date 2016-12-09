extern "C" {
	#include "freertos/FreeRTOS.h"
	#include "esp_system.h"
	#include "esp_event.h"
	#include "esp_event_loop.h"
	#include "nvs_flash.h"
	#include "sdkconfig.h"
}
#include "led_hal.h"

#define LED_STATUS CONFIG_LED_STATUS
#define LED_CONNECT CONFIG_LED_CONNECT

SpectacleLED statLED(LED_STATUS, 1);
SpectacleLED connectLED(LED_CONNECT, 1);

enum {
	STAT_LED_OFF,
	STAT_LED_ON,
	STAT_LED_BLINK
} stat_led_mode = STAT_LED_BLINK;
static unsigned int statLEDBlinkPeriod = 1000;

void stat_led_task(void *pvParameter)
{
	while (1)
	{
		switch (stat_led_mode)
		{
		case STAT_LED_OFF:
			statLED.off();
			vTaskDelay(100 / portTICK_RATE_MS);
			break;
		case STAT_LED_ON:
			statLED.on();
			vTaskDelay(100 / portTICK_RATE_MS);
			break;
		case STAT_LED_BLINK:
			statLED.off();
			vTaskDelay((statLEDBlinkPeriod / 2) / portTICK_RATE_MS);
			statLED.on();
			vTaskDelay((statLEDBlinkPeriod / 2) / portTICK_RATE_MS);
			break;
		}
	}
}

static void init_hardware(void)
{
    nvs_flash_init();
	
	statLED.init();
	connectLED.init();	
}

extern "C" void app_main(void)
{
	init_hardware();
    xTaskCreate(&stat_led_task, "stat_led_task", 512, NULL, 5, NULL);

    while (true) 
	{
		printf("Testing 123\r\n");
        vTaskDelay(1000 / portTICK_RATE_MS);
    }
}