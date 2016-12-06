#include "freertos/FreeRTOS.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "nvs_flash.h"
#include "driver/gpio.h"

#define LED_STATUS GPIO_NUM_5

static void esp32_system_init(void);
static void hardware_init(void);

esp_err_t event_handler(void *ctx, system_event_t *event)
{
    return ESP_OK;
}


void esp32_system_init(void)
{
    nvs_flash_init();
    tcpip_adapter_init();
    ESP_ERROR_CHECK( esp_event_loop_init(event_handler, NULL) );	
}

void hardware_init(void)
{
    gpio_set_direction(LED_STATUS, GPIO_MODE_OUTPUT);
}

void app_main(void)
{
	esp32_system_init();
	
	hardware_init();

    while (true) 
	{
		static int level = 0;
        gpio_set_level(LED_STATUS, level);
        level = !level;
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}