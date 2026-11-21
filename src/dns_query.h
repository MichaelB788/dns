#pragma once
#include "hostname.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define QNAME_MAX_LEN (1ul + HOSTNAME_MAX + 1ul)
#define NAME_MAX_LEN (1ul + HOSTNAME_MAX + 1ul)

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

struct dns_query {
  size_t length;
  struct dns_header header;
  struct dns_question question;
  const char *host;
};

struct dns_resource_record {
  size_t name_len;
  uint8_t name[QNAME_MAX_LEN];
  uint16_t type;
  uint16_t _class;
  uint32_t ttl;
  uint16_t rdlength;
  uint8_t *rdata;
};

bool dns_query_send(const char *host, uint16_t id);
