#pragma once
#include "dns_common.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

struct dns_query {
  size_t length;
  struct dns_header header;
  struct dns_question question;
  const char *host;
};

bool dns_query_init(struct dns_query *query, uint16_t id, const char *host);
bool dns_query_send(struct dns_query *query, uint16_t id);
void dns_query_serialize(const struct dns_query *query, uint8_t *buf);
void dns_query_print(const struct dns_query *query);
