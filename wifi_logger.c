#include "wifi_logger.h"

static const char* tag_wifi_logger = "wifi_logger";
static QueueHandle_t wifi_logger_queue;

esp_err_t init_queue(void)
{
	wifi_logger_queue = xQueueCreate(MESSAGE_QUEUE_SIZE, sizeof(char*));

	if (wifi_logger_queue == NULL)
	{
		ESP_LOGE(tag_wifi_logger, "%s", "Queue creation failed");
		return ESP_FAIL;
	}
	else
	{
		ESP_LOGI(tag_wifi_logger, "%s", "Queue created");
		return ESP_OK;
	}
}

void init_wifi(void)
{
	ESP_ERROR_CHECK(nvs_flash_init());
	ESP_ERROR_CHECK(esp_netif_init());
	ESP_ERROR_CHECK(esp_event_loop_create_default());

	ESP_ERROR_CHECK(example_connect());
}

esp_err_t send_to_queue(char* log_message)
{
	BaseType_t qerror = xQueueSendToBack(wifi_logger_queue, (void*)&log_message, (TickType_t) 0/portTICK_PERIOD_MS);

	if(qerror == pdPASS)
	{
		ESP_LOGD(tag_wifi_logger, "%s", "Data sent to Queue");
		return ESP_OK;
	}
	else if(qerror == errQUEUE_FULL)
	{
		ESP_LOGE(tag_wifi_logger, "%s", "Data not sent to Queue, Queue full");
		return ESP_FAIL;
	}
	else
	{
		ESP_LOGE(tag_wifi_logger, "%s", "Unknown error");
		return ESP_FAIL;
	}
}

char* receive_from_queue(void)
{
	char* data;
	// ************************* IMPORTANT *******************************************************************
	// Timeout period is set to portMAX_DELAY, so if it doesnot receive a log message for ~50 days, config assert will fail and program will crash
	//
	BaseType_t qerror = xQueueReceive(wifi_logger_queue, &data, (TickType_t) portMAX_DELAY);
	configASSERT(qerror);
	//
	// *******************************************************************************************************

	if(qerror == pdPASS)
	{
		ESP_LOGD(tag_wifi_logger, "%s", "Data received from Queue");
	}
	else if(qerror == pdFALSE)
	{
		ESP_LOGW(tag_wifi_logger, "%s", "Data not received from Queue");
		data = NULL;
	}
	else
	{
		free((void*)data);

		ESP_LOGE(tag_wifi_logger, "%s", "Unknown error");
		data = NULL;
	}

	return data;
}

void generate_log_message(esp_log_level_t level, const char *TAG, int line, const char *func, const char *fmt, ...)
{
	char log_print_buffer[BUFFER_SIZE];

	memset(log_print_buffer, '\0', BUFFER_SIZE);
	snprintf(log_print_buffer, BUFFER_SIZE, "%s (%s:%d) ", TAG, func, line);
	va_list args;
	va_start(args, fmt);

	int len = strlen(log_print_buffer);

	if (BUFFER_SIZE - len > 1)
	{
		vsnprintf(&log_print_buffer[len], (BUFFER_SIZE - len), fmt, args);
	}
	else
	{
		memset(log_print_buffer, '\0', BUFFER_SIZE);
		sprintf(log_print_buffer, "%s", "Buffer overflowed, increase buffer size");
	}
	va_end(args);

	uint log_level_opt = 2;

	switch (level)
	{
	case ESP_LOG_ERROR:
		log_level_opt = 0;
		break;
	case ESP_LOG_WARN:
		log_level_opt = 1;
		break;
	case ESP_LOG_INFO:
		log_level_opt = 2;
		break;
	case ESP_LOG_DEBUG:
		log_level_opt = 3;
		break;
	case ESP_LOG_VERBOSE:
		log_level_opt = 4;
		break;
	default:
		log_level_opt = 2;
		break;
	}

	// ************************* IMPORTANT *******************************************************************
	// I am mallocing a char* inside generate_log_timestamp() function situated inside util.cpp, log_print_buffer is not being pushed to queue
	// The function returns the malloc'd char* and is passed to the queue
	//
	send_to_queue(generate_log_message_timestamp(log_level_opt, esp_log_timestamp(), log_print_buffer));
	//
	//********************************************************************************************************
}

int system_log_message_route(const char* fmt, va_list tag)
{
	char *log_print_buffer = (char*) malloc(sizeof(char) * BUFFER_SIZE);
	vsprintf(log_print_buffer, fmt, tag);

	send_to_queue(log_print_buffer);

	return vprintf(fmt, tag);
}

void start_wifi_logger(void)
{
	init_wifi();
	ESP_ERROR_CHECK(init_queue());

#ifdef CONFIG_ROUTE_ESP_IDF_API_LOGS_TO_WIFI
	esp_log_set_vprintf(system_log_message_route);
#endif

	xTaskCreatePinnedToCore(&wifi_logger, "wifi_logger", 4096, NULL, 2, NULL, 1);
	ESP_LOGI(tag_wifi_logger, "WiFi logger initialised");
}

bool is_connected(void* handle_t)
{
	bool ret = false;

#ifdef CONFIG_TRANSPORT_PROTOCOL_WEBSOCKET
	ret = esp_websocket_client_is_connected((esp_websocket_client_handle_t) handle_t);
#endif
#ifdef CONFIG_TRANSPORT_PROTOCOL_TCP
	/* Technically ENOTCONN indicates socket is not connected but I think there is a bug
	 * Check out https://github.com/espressif/esp-idf/issues/6092 to follow the issue ticket raised
	 * Till then keeping the following check instead of just 0 == errno
	 **/
	if (0 == errno || ENOTCONN == errno)
	{
		ret = true;
	}
#endif
#ifdef CONFIG_TRANSPORT_PROTOCOL_UDP
	ret = true;
#endif

	return ret;
}

#ifdef CONFIG_TRANSPORT_PROTOCOL_UDP
void wifi_logger()
{
	struct network_data* handle = malloc(sizeof(struct network_data));
	network_manager(handle);

	while (true)
	{
		char* log_message = receive_from_queue();

		if (log_message != NULL)
		{
			int len = send_data(handle, log_message);
			ESP_LOGD(tag_wifi_logger, "%d %s", len, "bytes of data sent");

			free((void*)log_message);
		}
		else
		{
			log_message = "Unknown error - receiving log message";
			int len = send_data(handle, log_message);
			ESP_LOGE(tag_wifi_logger, "%d %s", len, "Unknown error");
		}
	}

	close_network_manager(handle);
}
#endif

#ifdef CONFIG_TRANSPORT_PROTOCOL_TCP
void wifi_logger()
{
	char* log_message;
	while(1) // To keep trying over and over
	{
		while(1) // Try creating and connecting to socket
		{
			struct tcp_network_data* handle = malloc(sizeof(struct network_data));
			if (false == tcp_network_manager(handle))
			{
				// Slow down while retrying.
				vTaskDelay(2000 / portTICK_PERIOD_MS);
				break;
			}

			while (is_connected(handle))
			{
				log_message = receive_from_queue();

				if (log_message != NULL)
				{
					int len = tcp_send_data(handle, log_message);

					if(len < 0)
					{
						/* Trying to push it back to queue if sending fails, but might lose some logs if frequency is high
						 * Might see garbage at first when reconnected.
						 */
						xQueueSendToFront(wifi_logger_queue, (void*)log_message, (TickType_t) portMAX_DELAY);
						break;
					}
					ESP_LOGD(tag_wifi_logger, "%d %s", len, "bytes of data sent");
				}
				else
				{
					log_message = "Unknown error - receiving log message";
					int len = tcp_send_data(handle, log_message);
					ESP_LOGE(tag_wifi_logger, "%d %s", len, "Unknown error");
				}
				//Checkout following link to understand why we need this delay if want watchdog running.
				//https://github.com/espressif/esp-idf/issues/1646#issuecomment-367507724
				vTaskDelay(10 / portTICK_PERIOD_MS);
			}
			if (handle->sock != -1)
			{
				tcp_close_network_manager(handle);
			}
		}
	}
}
#endif

#ifdef CONFIG_TRANSPORT_PROTOCOL_WEBSOCKET
void wifi_logger()
{
	esp_websocket_client_handle_t handle = websocket_network_manager();

	while (true)
	{
		if(is_connected(handle))
		{
			char* log_message = receive_from_queue();

			if (log_message != NULL)
			{
				int len = websocket_send_data(handle, log_message);

				if(len > 0)
				{
					ESP_LOGD(tag_wifi_logger, "%d %s", len, "bytes of data sent");
				}

				free((void*)log_message);
			}
			else
			{
				log_message = "Unknown error - log message corrupt";
				int len = websocket_send_data(handle, log_message);
				ESP_LOGE(tag_wifi_logger, "%d %s", len, "Unknown error");
			}
		}
		else
		{
			//Checkout following link to understand why we need this delay if want watchdog running.
			//https://github.com/espressif/esp-idf/issues/1646#issuecomment-367507724
			vTaskDelay(10 / portTICK_PERIOD_MS);
		}
	}

	websocket_close_network_manager(handle);
}
#endif
