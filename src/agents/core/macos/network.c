#include "agents/core/network.h"
#include "agents/core/base/defs.h"

#include <assert.h>
#include <net/if.h>
#include <net/route.h>
#include <stdlib.h>
#include <string.h>
#include <sys/sysctl.h>
#include <unistd.h>

#include <curl/curl.h>
#include <curl/easy.h>

int UrlAvailable(const char *url, unsigned int delay) {
  usleep(delay);

  CURLcode response = CURLE_FAILED_INIT;
  CURL *curl = curl_easy_init();
  if (curl) {
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_NOBODY, 1);
    response = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
  }
  return response == CURLE_OK;
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