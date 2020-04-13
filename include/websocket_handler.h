#ifndef WEBSOCKET_HANDLER_C
#define WEBSOCKET_HANDLER_C

#include <stdio.h>
#include <string.h>
#include "esp_wifi.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_log.h"
#include "esp_websocket_client.h"
#include "esp_event.h"


#define WEBSOCKET_HOST_URI CONFIG_SERVER_WEBSOCKET_URI

void websocket_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data);
esp_websocket_client_handle_t websocket_network_manager();
int websocket_send_data(esp_websocket_client_handle_t network_handle, char* payload);
void websocket_close_network_manager(esp_websocket_client_handle_t network_handle);

#endif