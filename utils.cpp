#include "utils.h"

/**
 * @brief adds timestamp to the log message
 * 
 * @param timestamp timestamp provided by ESP in milliseconds
 * @param log_message log message to be sent through wifi
 * @return char* final log message with timestamp
 */
char* generate_log_message_timestamp(uint log_level, uint32_t timestamp, char* log_message)
{
    std::string log_string = std::string("\e[32m") + std::to_string(log_level) + std::string("(") + std::to_string(timestamp) + std::string(") ") + std::string(log_message) + std::string("\n");

    char *c_log_string = (char*) malloc(sizeof(char)*(log_string.size()+1));
    memcpy(c_log_string, log_string.c_str(), log_string.size()+1);
    
    return c_log_string;
}