#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "wifi_logger.h"


void app_main(void)
{
    init_wifi();
    ESP_ERROR_CHECK(init_queue());
        
    xTaskCreatePinnedToCore(&wifi_logger, "wifi_logger", STACK_SIZE, NULL, 2, NULL, 1);
    // wifi_logger();

    while(1)
    {
        wifi_log("test", "%s", "hello world wifi logger");
            // vTaskDelay(1);
    }
}
