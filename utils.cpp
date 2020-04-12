#include "utils.h"

char* generate_log_message_timestamp(uint32_t timestamp, char* log_message)
{
    std::string log_string = "(" + std::string(std::to_string(timestamp)) + ") " + std::string(log_message);

    char *c_log_string = (char*) malloc(sizeof(char)*(log_string.size()+1));
    memcpy(c_log_string, log_string.c_str(), log_string.size()+1);
    
    return c_log_string;
}