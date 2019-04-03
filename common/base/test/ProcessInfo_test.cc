#include "../ProcessInfo.h"
#include <stdio.h>
#define __STDC_FORMAT_MACROS
#include <inttypes.h>

int main()
{
  printf("pid = %d\n", WJ::ProcessInfo::pid());
  printf("uid = %d\n", WJ::ProcessInfo::uid());
  printf("euid = %d\n", WJ::ProcessInfo::euid());
  printf("start time = %s\n", WJ::ProcessInfo::startTime().toFormattedString().c_str());
  printf("hostname = %s\n", WJ::ProcessInfo::hostname().c_str());
  printf("opened files = %d\n", WJ::ProcessInfo::openedFiles());
  printf("threads = %zd\n", WJ::ProcessInfo::threads().size());
  printf("num threads = %d\n", WJ::ProcessInfo::numThreads());
  printf("status = %s\n", WJ::ProcessInfo::procStatus().c_str());
}
