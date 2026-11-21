#include "dns_query.h"
#include "hostname.h"
#include <netinet/in.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

static bool dns_query_update_qname(struct dns_question *question,
                                   const char *host);
static void dns_query_serialize(const struct dns_query *query, uint8_t *buf);
static void dns_question_print(const struct dns_question *question);

void dns_query_send(const char *host, uint16_t id) {
  if (!validate_hostname(host))
    return;

  uint8_t recv_buf[65536];

  // replies
  struct dns_resource_record answers[20], auth[20], addit[20];

  int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if (sock == -1) {
    perror("Could not create socket");
    return;
  }

  struct sockaddr_in dest = {.sin_family = AF_INET, .sin_port = htons(58)};

  struct dns_query query = {
      .header = {.id = htons(id),
                 .flags = htons(0x100), // QR = 0, OPCODE = 0, RD = 1
                 .qdcount = htons(1)},
      .question = {
          .qtype = htons(1), // A
          .qclass = htons(1) // IN
      }};
  if (!dns_query_update_qname(&query.question, host))
    return;

  uint8_t query_buf[sizeof(struct dns_header) + DNS_QNAME_MAX_LEN + 4] = {0};
  dns_query_serialize(&query, query_buf);
}

bool dns_query_update_qname(struct dns_question *question, const char *host) {
  char host_buf[HOSTNAME_MAX + 1] = {0};
  memcpy(host_buf, host, strlen(host));

  size_t i = 0;
  for (char *save, *label = strtok_r(host_buf, ".", &save); label != NULL;
       label = strtok_r(NULL, ".", &save)) {
    if (!validate_label(label))
      return false;

    const size_t label_len = strlen(label);
    question->qname[i++] = (uint8_t)label_len;
    memcpy(question->qname + i, label, label_len);

    i += label_len;
  }
  question->qname[i++] = 0;
  question->qname_len = i;

  return true;
}

void dns_query_serialize(const struct dns_query *query, uint8_t *buf) {
  memcpy(buf, &query->header, sizeof(struct dns_header));
  memcpy(buf + sizeof(struct dns_header), query->question.qname,
         query->question.qname_len);

  uint16_t *p =
      (uint16_t *)(buf + sizeof(struct dns_header) + query->question.qname_len);
  p[0] = query->question.qtype;
  p[1] = query->question.qclass;
}

void dns_question_print(const struct dns_question *question) {
  char qname_str[1000] = {0};
  size_t offset = 0;
  for (size_t i = 0; i < question->qname_len; ++i) {
    const size_t max_len = sizeof(qname_str) - offset;
    offset +=
        snprintf(qname_str + offset, max_len, "%02x ", question->qname[i]);
  }

  printf("Question:\n"
         "  QNAME:  %s\n"
         "          %s\n"
         "  QTYPE:  0x%04x\n"
         "  QCLASS: 0x%04x\n"
         "  Hex:    %s%02x %02x %02x %02x",
         query->hostname, qname_str, dns_query_read16(query, qtype),
         dns_query_read16(query, qclass), qname_str, query->data[qtype + 0],
         query->data[qtype + 1], query->data[qclass + 0],
         query->data[qclass + 1]);
}
