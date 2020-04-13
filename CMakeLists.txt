set(srcs "wifi_logger.c" "utils.cpp")

if(CONFIG_TRANSPORT_PROTOCOL_TCP)
    list(APPEND srcs "tcp_handler.c")
elseif(CONFIG_TRANSPORT_PROTOCOL_UDP)
    list(APPEND srcs "udp_handler.c")
elseif(CONFIG_TRANSPORT_PROTOCOL_WEBSOCKET)
    list(APPEND srcs "websocket_handler.c")
endif()

idf_component_register(SRCS "${srcs}"
                       INCLUDE_DIRS "include"
                       REQUIRES protocol_examples_common nvs_flash esp_websocket_client)