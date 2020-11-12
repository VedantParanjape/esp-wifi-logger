#include "tcp_handler.h"

static const char *TAG = "tcp_handler";

/**
 * @brief Manages TCP connection to the server
 * 
 * @param nm tcp_network_data struct which contains necessary data for a TCP connection
 * @return void
 **/
bool tcp_network_manager(struct tcp_network_data* nm)
{
	nm->dest_addr.sin_addr.s_addr = inet_addr(TCP_HOST_IP_ADDR);
	nm->dest_addr.sin_family = AF_INET;
	nm->dest_addr.sin_port = htons(TCP_PORT);
	nm->addr_family = AF_INET;
	nm->ip_protocol = IPPROTO_IP;
	inet_ntoa_r(nm->dest_addr.sin_addr, nm->addr_str, sizeof(nm->addr_str) - 1);

	nm->sock = socket(nm->addr_family, SOCK_STREAM, nm->ip_protocol);
	if (nm->sock < 0)
	{
		ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
		tcp_close_network_manager(nm);
		return false;
	}
	else
	{
		ESP_LOGI(TAG, "Socket created");
	}

	int err = connect(nm->sock, (struct sockaddr *)&nm->dest_addr, sizeof(nm->dest_addr));
	if (err != 0) {
		ESP_LOGE(TAG, "Socket unable to connect: errno %d", errno);
		tcp_close_network_manager(nm);
		return false;
	}
	else
	{
		ESP_LOGI(TAG, "Successfully connected to %s:%d with status %d", TCP_HOST_IP_ADDR, TCP_PORT, errno);
	}
	return true;
}

/**
 * @brief Sends data to the server through a TCP socket
 * 
 * @param nm A pointer to tcp_network_data struct
 * @param payload char array which contains data to be sent
 * @return int - returns -1 if sending failed, number of bytes sent if successfully sent the data
 **/
int tcp_send_data(struct tcp_network_data* nm, char* payload)
{
	if(nm->sock < 0)
	{
		ESP_LOGE(TAG, "%s", "Socket does not exist");
		return -1;
	}
	else
	{
		int err = send(nm->sock, payload, strlen(payload), 0);
		if (err < 0)
		{
			ESP_LOGE(TAG, "Error occurred during sending: errno %d", errno);
		}
		else
		{
			ESP_LOGD(TAG, "%s", "Message sent");
		}
		return err;
	}
}

/**
 * @brief Receives data from TCP server
 * 
 * @param nm tcp_network_data struct which contains connection info
 * @return char array which contains data received
 **/
char* tcp_receive_data(struct tcp_network_data* nm)
{
	if (nm->sock < 0)
	{
		ESP_LOGE(TAG, "%s", "Socket doesnot exist");
		return NULL;
	}
	else
	{
		int len = recv(nm->sock, nm->rx_buffer, sizeof(nm->rx_buffer) - 1, 0);

		if (len < 0)
		{
			ESP_LOGE(TAG, "recvfrom failed: errno %d", errno);
			return NULL;
		}
		else
		{
			nm->rx_buffer[len] = 0; // Null-terminate whatever we received and treat like a string
			ESP_LOGD(TAG, "Received %d bytes from %s:", len, nm->addr_str);
			ESP_LOGD(TAG, "%s", nm->rx_buffer);
			return nm->rx_buffer;
		}
	}
}

/**
 * @brief Shutdown active connection, deallocate memory
 * 
 * @param nm tcp_network_data struct which contains connection info
 * @return void
 **/
void tcp_close_network_manager(struct tcp_network_data* nm)
{
	ESP_LOGI(TAG, "%s", "Shutting down socket and restarting...");
	shutdown(nm->sock, 0);
	close(nm->sock);
}
