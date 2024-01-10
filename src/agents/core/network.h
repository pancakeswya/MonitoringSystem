#ifndef MONITORINGSYSTEM_SRC_AGENTS_CORE_NETWORK_H_
#define MONITORINGSYSTEM_SRC_AGENTS_CORE_NETWORK_H_

#ifdef __cplusplus
extern "C" {
#endif
  extern int UrlAvailable(const char* url, unsigned int delay);
  extern double InetThroughput(unsigned int delay);
#ifdef __cplusplus
}
#endif

#endif // MONITORINGSYSTEM_SRC_AGENTS_CORE_NETWORK_H_