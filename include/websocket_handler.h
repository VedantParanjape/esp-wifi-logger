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

#ifdef __cplusplus
extern "C" {
#endif

#define WEBSOCKET_HOST_URI CONFIG_SERVER_WEBSOCKET_URI

/*
 * 0x00: this frame continues the payload from the last.
 * 0x01: this frame includes utf-8 text data.
 * 0x02: this frame includes binary data.
 * 0x08: this frame terminates the connection.
 * 0x09: this frame is a ping.
 * 0x0a: this frame is a pong.
 *
 * A ping or pong is just a regular frame, but it's a control frame.
 * Pings have an opcode of 0x9, and pongs have an opcode of 0xA.
 * When you get a ping, send back a pong with the exact same Payload Data
 * as the ping (for pings and pongs, the max payload length is 125).
 * You might also get a pong without ever sending a ping; ignore this if it happens.
 *
 * https://developer.mozilla.org/en-US/docs/Web/API/WebSockets_API/Writing_WebSocket_servers
 */

typedef enum websocket_op {
	websocket_op_continue_payload = 0,
	websocket_op_utf_8_text = 1,
	websocket_op_binary_data = 2,
	websocket_op_termination_frame = 8,
	websocket_op_ping_frame = 9,
	websocket_op_pong_frame = 10,
}websocket_op_t;

/**
 * @brief Websocket event handler
 *
 * @param handler_args args to be passed to handler
 * @param base event base
 * @param event_id event id
 * @param event_data event data
 */
void websocket_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data);
/**
 * @brief start the websocket client and connect to the server
 *
 * @return esp_websocket_client_handle_t return a websocket client handle, used to communicate with server
 */
esp_websocket_client_handle_t websocket_network_manager();
/**
 * @brief Sends data to the websocket server
 *
 * @param network_handle network handle returned by network manager
 * @param payload data to be sent to the server
 * @return int returns number of bytes sent, -1 if any error occurs in sending
 */
int websocket_send_data(esp_websocket_client_handle_t network_handle, char* payload);
/**
 * @brief stop and destroy websocket client
 *
 * @param network_handle client handle that needs to be stopped and destroyed
 */
void websocket_close_network_manager(esp_websocket_client_handle_t network_handle);

#ifdef __cplusplus
}
#endif

#endif
