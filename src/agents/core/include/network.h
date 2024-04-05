#ifndef MONITORINGSYSTEM_SRC_AGENTS_CORE_INCLUDE_NETWORK_H_
#define MONITORINGSYSTEM_SRC_AGENTS_CORE_INCLUDE_NETWORK_H_

#include "defs.h"

#ifdef __cplusplus
extern "C" {
#endif
extern CoreError UrlAvailable(int* url_available, const char* url);
extern CoreError InetThroughput(double* inet_throughput, unsigned int delay);
#ifdef __cplusplus
}
#endif

#endif  // MONITORINGSYSTEM_SRC_AGENTS_CORE_INCLUDE_NETWORK_H_