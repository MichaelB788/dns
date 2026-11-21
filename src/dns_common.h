#pragma once
#include "hostname.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define QNAME_MAX_LEN (1ul + HOSTNAME_MAX + 1ul)
#define NAME_MAX_LEN (1ul + HOSTNAME_MAX + 1ul)
#define DNS_QUERY_MAX_LEN (sizeof(struct dns_header) + QNAME_MAX_LEN + 4ul)

struct dns_header {
  uint16_t id;
  uint16_t flags;
  uint16_t qdcount;
  uint16_t ancount;
  uint16_t nscount;
  uint16_t arcount;
};

struct dns_question {
  size_t qname_len;
  uint8_t qname[QNAME_MAX_LEN];
  uint16_t qtype;
  uint16_t qclass;
};
