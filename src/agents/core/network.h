#ifndef MONITORINGSYSTEM_SRC_AGENTS_CORE_NETWORK_H_
#define MONITORINGSYSTEM_SRC_AGENTS_CORE_NETWORK_H_

#ifdef __cplusplus
extern "C" {
#endif
  int UrlAvailable(const char* url);
  double InetThroughput();
#ifdef __cplusplus
}
#endif

#endif // MONITORINGSYSTEM_SRC_AGENTS_CORE_NETWORK_H_