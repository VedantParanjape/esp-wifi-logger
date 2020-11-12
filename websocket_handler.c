#include "websocket_handler.h"

static const char* TAG = "websocket_handler";

/**
 * @brief Websocket event handler
 * 
 * @param handler_args args to be passed to handler
 * @param base event base 
 * @param event_id event id
 * @param event_data event data
 */
void websocket_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    esp_websocket_event_data_t *data = (esp_websocket_event_data_t *)event_data;
    
    switch (event_id) {
        case WEBSOCKET_EVENT_CONNECTED:
            ESP_LOGI(TAG, "WEBSOCKET_EVENT_CONNECTED");


            break;
        case WEBSOCKET_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "WEBSOCKET_EVENT_DISCONNECTED");
            break;

        case WEBSOCKET_EVENT_DATA:
        	  //Avoid printing logs if it is just a ping/pong frame.
        	  if(websocket_op_ping_frame != data->op_code && websocket_op_pong_frame != data->op_code)
        	  {
        		  ESP_LOGI(TAG, "WEBSOCKET_EVENT_DATA");
        		  ESP_LOGI(TAG, "Received opcode=%d", data->op_code);
        		  ESP_LOGW(TAG, "Received=%.*s\r\n", data->data_len, (char*)data->data_ptr);
        	  }
            break;
        
        case WEBSOCKET_EVENT_ERROR:
            ESP_LOGW(TAG, "WEBSOCKET_EVENT_ERROR");
            break;
    }
}

/**
 * @brief start the websocket client and connect to the server
 * 
 * @return esp_websocket_client_handle_t return a websocket client handle, used to communicate with server
 */
esp_websocket_client_handle_t websocket_network_manager()
{
	ESP_LOGI(TAG, "Connecting to %s...", WEBSOCKET_HOST_URI);

	esp_websocket_client_handle_t network_handle;

	const esp_websocket_client_config_t websocket_cfg = {
			.uri = WEBSOCKET_HOST_URI,
	};

	network_handle = esp_websocket_client_init(&websocket_cfg);
	esp_websocket_register_events(network_handle, WEBSOCKET_EVENT_ANY, websocket_event_handler, (void *)network_handle);

	esp_websocket_client_start(network_handle);

	return network_handle;
}

/**
 * @brief Sends data to the websocket server
 * 
 * @param network_handle network handle returned by network manager
 * @param payload data to be sent to the server
 * @return int returns number of bytes sent, -1 if any error occurs in sending
 */
int websocket_send_data(esp_websocket_client_handle_t network_handle, char* payload)
{

	if (esp_websocket_client_is_connected(network_handle))
	{
		int err = esp_websocket_client_send(network_handle, payload, strlen(payload), portMAX_DELAY);

		if (err < 0)
		{
			ESP_LOGE(TAG, "Error occured during sending: errno %d", errno);
		}
		else
		{
			ESP_LOGD(TAG, "%s", "Message sent");
		}

		return err;
	}

	return -1;
}

/**
 * @brief stop and destroy websocket client
 * 
 * @param network_handle client handle that needs to be stopped and destroyed
 */
void websocket_close_network_manager(esp_websocket_client_handle_t network_handle)
{
	esp_websocket_client_stop(network_handle);
	esp_websocket_client_destroy(network_handle);
	ESP_LOGI(TAG, "Websocket Stopped");
}
