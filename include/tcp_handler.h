#ifndef TCP_HANDLER_H
#define TCP_HANDLER_H

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

#define TCP_HOST_IP_ADDR CONFIG_SERVER_IP_ADDRESS
#define TCP_PORT CONFIG_SERVER_PORT

struct tcp_network_data
{
	char rx_buffer[128];
	char addr_str[128];
	int addr_family;
	int ip_protocol;
	struct sockaddr_in dest_addr;
	int sock;
};

/**
 * @brief Manages TCP connection to the server
 *
 * @param nm tcp_network_data struct which contains necessary data for a TCP connection
 * @return bool - returns false if an error occurred, true otherwise
 **/
bool tcp_network_manager(struct tcp_network_data* nm);
/**
 * @brief Sends data to the server through a TCP socket
 *
 * @param nm A pointer to tcp_network_data struct
 * @param payload char array which contains data to be sent
 * @return int - returns -1 if sending failed, number of bytes sent if successfully sent the data
 **/
int tcp_send_data(struct tcp_network_data* nm, char* payload);
/**
 * @brief Receives data from TCP server
 *
 * @param nm tcp_network_data struct which contains connection info
 * @return char array which contains data received
 **/
char* tcp_receive_data(struct tcp_network_data* nm);
/**
 * @brief Shutdown active connection, deallocate memory
 *
 * @param nm tcp_network_data struct which contains connection info
 * @return void
 **/
void tcp_close_network_manager(struct tcp_network_data* nm);

#ifdef __cplusplus
}
#endif

#endif
