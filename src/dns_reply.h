#pragma once
#include "dns_common.h"
#include <stddef.h>
#include <stdint.h>

struct dns_resource_record {
  struct dns_header header;
  struct dns_question question;
  size_t name_len;
  uint8_t name[QNAME_MAX_LEN];
  uint16_t type;
  uint16_t _class;
  uint32_t ttl;
  uint16_t rdlength;
  uint8_t *rdata;
};

void dns_resource_record_init(struct dns_resource_record *rr,
                              const uint8_t *buf);
