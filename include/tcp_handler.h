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

bool tcp_network_manager(struct tcp_network_data* nm);
int tcp_send_data(struct tcp_network_data* nm, char* payload);
char* tcp_receive_data(struct tcp_network_data* nm);
void tcp_close_network_manager(struct tcp_network_data* nm);

#ifdef __cplusplus
}
#endif

#endif
