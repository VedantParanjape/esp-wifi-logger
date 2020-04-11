idf_component_register(SRCS "wifi_logger.c" "udp_handler.c" "tcp_handler.c"
                       INCLUDE_DIRS "include"
                       REQUIRES protocol_examples_common nvs_flash)