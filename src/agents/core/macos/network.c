#include "agents/core/network.h"
#include "agents/core/base/defs.h"

#include <assert.h>
#include <net/if.h>
#include <net/route.h>
#include <stdlib.h>
#include <string.h>
#include <sys/sysctl.h>
#include <unistd.h>

int UrlAvailable(const char *url, unsigned int delay) {
  usleep(delay);
  const char util[] = "curl --head --silent --fail --output /dev/null ";
  size_t util_len = strlen(util), url_len = strlen(url);
  char *command = (char *)malloc(util_len + url_len + 1);
  strcpy(command, util);
  strcpy(command + util_len, url);
  command[util_len + url_len] = '\n';
  int res = system(command);
  free(command);
  return res == 0;
}

static size_t GetTotalBytes(void) {
  int mib[] = {CTL_NET, PF_ROUTE, 0, 0, NET_RT_IFLIST2, 0};
  size_t len;
  int res = sysctl(mib, 6, NULL, &len, NULL, 0);
  assert(res == 0);
  char *buf = (char *)malloc(len);
  res = sysctl(mib, 6, buf, &len, NULL, 0);
  assert(res == 0);
  unused(res);
  char *lim = buf + len;
  char *next = NULL;
  size_t totalibytes = 0, totalobytes = 0;
  for (next = buf; next < lim;) {
    struct if_msghdr *ifm = (struct if_msghdr *)next;
    next += ifm->ifm_msglen;
    if (ifm->ifm_type == RTM_IFINFO2) {
      struct if_msghdr2 *if2m = (struct if_msghdr2 *)ifm;
      totalibytes += if2m->ifm_data.ifi_ibytes;
      totalobytes += if2m->ifm_data.ifi_obytes;
    }
  }
  return totalibytes + totalobytes;
}

double InetThroughput(unsigned int delay) {
  size_t prev_bytes = GetTotalBytes();
  usleep(delay);
  size_t curr_bytes = GetTotalBytes();
  return (double)(curr_bytes - prev_bytes);
}