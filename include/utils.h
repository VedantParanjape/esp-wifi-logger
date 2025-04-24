#ifndef UTILS_H
#define UTILS_H

#ifdef __cplusplus
#include <string>
#include <cstring>
extern "C" {
#endif

/**
 * @brief adds timestamp to the log message
 *
 * @param log_level log level of the log message
 * @param timestamp timestamp provided by ESP in milliseconds
 * @param log_message log message to be sent through wifi
 * @return char* final log message with timestamp
 */
char* generate_log_message_timestamp(uint log_level, uint32_t timestamp, char* log_message);

#ifdef __cplusplus
}
#endif

#endif

