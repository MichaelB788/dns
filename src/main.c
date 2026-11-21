#include "dns_query.h"
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, const char **argv) {
  if (argc != 2) {
    fprintf(stderr,
            "Invalid argument count: %d arguments given, expected only 1 "
            "argument.\n",
            argc - 1);
    return 1;
  }

  const uint16_t id = getpid();

  dns_query_send(argv[1], id);
}
