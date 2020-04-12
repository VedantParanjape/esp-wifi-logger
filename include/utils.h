#ifdef __cplusplus
#include <string>
#include <cstring>
extern "C" {
#endif

char* generate_log_message_timestamp(uint log_level, uint32_t timestamp, char* log_message);

#ifdef __cplusplus
}
#endif