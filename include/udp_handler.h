#ifndef UDP_HANDLER_H
#define UDP_HANDLER_H

#include <string.h>
#include <sys/param.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "protocol_examples_common.h"
#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include <lwip/netdb.h>

#ifdef __cplusplus
extern "C" {
#endif

#define HOST_IP_ADDR CONFIG_SERVER_IP_ADDRESS
#define PORT CONFIG_SERVER_PORT

struct network_data
{
	char rx_buffer[128];
	char addr_str[128];
	int addr_family;
	int ip_protocol;
	struct sockaddr_in dest_addr;
	int sock;
};

/**
 * @brief Manages UDP connection to the server
 *
 * @param nm network_data struct which contains necessary data for a UDP connection
 * @return void
 **/
void network_manager(struct network_data* nm);
/**
 * @brief Sends data to the server through a UDP socket
 *
 * @param nm A pointer to network_data struct
 * @param payload char array which contains data to be sent
 * @return int - returns -1 if sending failed, number of bytes sent if successfully sent the data
 **/
int send_data(struct network_data* nm, char* payload);
/**
 * @brief Receives data from UDP server
 *
 * @param nm network_data struct which contains connection info
 * @return char array which contains data received
 **/
char* receive_data(struct network_data* nm);
/**
 * @brief Shutdown active connection, deallocate memory
 *
 * @param nm tcp_network_data struct which contains connection info
 * @return void
 **/
void close_network_manager(struct network_data* nm);

#ifdef __cplusplus
}
#endif

#endif
