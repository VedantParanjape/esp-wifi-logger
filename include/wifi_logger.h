#ifndef WIFI_LOGGER_H
#define WIFI_LOGGER_H

#include "tcp_handler.h"
#include "udp_handler.h"
#include "utils.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "freertos/queue.h"
#include <stdlib.h>
#include <string.h>

QueueHandle_t wifi_logger_queue;

#define MESSAGE_QUEUE_SIZE CONFIG_MESSAGE_QUEUE_SIZE
#define BUFFER_SIZE CONFIG_BUFFER_SIZE
#define STACK_SIZE CONFIG_MESSAGE_QUEUE_SIZE*8
#define wifi_log(TAG, fmt, ...) generate_log_message(TAG, __LINE__, __func__, fmt, __VA_ARGS__);

esp_err_t init_queue(void);
void init_wifi(void);
esp_err_t send_to_queue(const char* log_message);
char* receive_from_queue(void);
void generate_log_message(const char *TAG, int line, const char *func, const char *fmt, ...);
void start_wifi_logger(void);

#ifdef CONFIG_TRANSPORT_PROTOCOL_TCP
void wifi_logger();
#endif 

#ifdef CONFIG_TRANSPORT_PROTOCOL_UDP
void wifi_logger();
#endif

#endif

