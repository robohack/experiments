#include <stdio.h>
extern char pcap_version[];
int main() {
  int major, minor;
  printf("pcap_version = %s\n", pcap_version);
  sscanf(pcap_version,"%d.%d", &major, &minor);
  if (major > 0)
    exit(0);
  if (minor > 4)
    exit(0);
  if (minor < 4)
    exit(1);
  if (pcap_version[3] > 'a')
    exit(0);
  if (pcap_version[3] == 'a') {
    if(!sscanf(&pcap_version[4], "%d", &minor))
      exit(1);
    if (minor >= 6)
      exit(0);
    else
      exit(1);
  }
  exit(1);
}
