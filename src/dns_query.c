#include "dns_query.h"
#include "hostname.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

bool dns_query_init(struct dns_query *query, uint16_t id, const char *host) {
  if (!validate_hostname(host))
    return false;

  memset(query, 0, sizeof(struct dns_query));

  query->host = host;
  query->header.id = htons(id);
  query->header.flags = htons(0x100); // QR = 0, OPCODE = 0, RD = 1
  query->header.qdcount = htons(1);
  query->question.qtype = htons(1);  // A
  query->question.qclass = htons(1); // IN

  char host_buf[HOSTNAME_MAX + 1] = {0};
  memcpy(host_buf, host, strlen(host));

  size_t i = 0;
  for (char *save, *label = strtok_r(host_buf, ".", &save); label != NULL;
       label = strtok_r(NULL, ".", &save)) {
    if (!validate_label(label))
      return false;

    const size_t label_len = strlen(label);
    query->question.qname[i++] = (uint8_t)label_len;
    memcpy(query->question.qname + i, label, label_len);

    i += label_len;
  }
  query->question.qname[i++] = 0;
  query->question.qname_len = i;

  query->length = sizeof(struct dns_header) + query->question.qname_len + 4;

  return true;
}

bool dns_query_send(struct dns_query *query, uint16_t id) {
  puts("=== DNS Query ===");
  dns_query_print(query);

  puts("Destination: 8.8.8.8:53 (UDP over IPv4)");
  struct sockaddr_in dest = {.sin_family = AF_INET, .sin_port = htons(53)};
  inet_pton(AF_INET, "8.8.8.8", &dest.sin_addr);

  bool success = true;
  int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if (sock == -1) {
    perror("Could not create socket");
    success = false;
    goto socket_cleanup;
  }
  struct timeval timeout = {.tv_sec = 5, .tv_usec = 0};
  if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof timeout) ==
      -1) {
    success = false;
    perror("Could not set receive timeout");
    goto socket_cleanup;
  }
  puts("Socket created");

  uint8_t query_buf[DNS_QUERY_MAX_LEN] = {0};
  dns_query_serialize(query, query_buf);

  uint8_t rcv_buf[65536] = {0};
  int attempt = 1;
  for (; attempt <= 3; ++attempt) {
    if (sendto(sock, query_buf, query->length, 0, (struct sockaddr *)&dest,
               sizeof dest) == -1) {
      perror("Failed to send packet");
      continue;
    }
    printf("Attempt %d/3: sent %zu bytes from local port %d\n", attempt,
           query->length, ntohs(dest.sin_port));

    if (recvfrom(sock, rcv_buf, sizeof rcv_buf, 0, NULL, NULL) == -1) {
      perror("Did not receive answer");
      continue;
    }

    printf("Attempt %d/3: received valid reply\n", attempt);
    break;
  }
  if (attempt > 3) {
    perror("Failure: no valid reply after 3 transmissions");
    success = false;
  }

socket_cleanup:
  shutdown(sock, SHUT_RDWR);
  close(sock);
  puts("Socket closed");

  return success;
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

void dns_query_print(const struct dns_query *query) {
  // header
  {
    const struct dns_header *h = &query->header;
    printf("Header:\n"
           "  ID:      0x%04x\n"
           "  Flags:   0x%04x\n"
           "  QDCOUNT: 0x%04x\n"
           "  ANCOUNT: 0x%04x\n"
           "  NSCOUNT: 0x%04x\n"
           "  ARCOUNT: 0x%04x\n",
           ntohs(h->id), ntohs(h->flags), ntohs(h->qdcount), ntohs(h->ancount),
           ntohs(h->nscount), ntohs(h->arcount));

    printf("  Hex:     ");
    for (size_t i = 0; i < sizeof(struct dns_header); ++i)
      printf("%02x ", ((uint8_t *)h)[i]);

    putchar('\n');
  }

  // question
  {
    const struct dns_question *q = &query->question;
    char qname_str[1000] = {0};
    size_t offset = 0;
    for (size_t i = 0; i < q->qname_len; ++i) {
      const size_t max_len = sizeof(qname_str) - offset;
      offset += snprintf(qname_str + offset, max_len, "%02x ", q->qname[i]);
    }

    const uint16_t qtype = ntohs(q->qtype), qclass = ntohs(q->qclass);
    printf("Question:\n"
           "  QNAME:  %s\n"
           "          %s\n"
           "  QTYPE:  0x%04x\n"
           "  QCLASS: 0x%04x\n"
           "  Hex:    %s%02x %02x %02x %02x\n",
           query->host, qname_str, qtype, qclass, qname_str, qtype >> 8,
           qtype & 0xff, qclass >> 8, qclass & 0xff);
  }

  // full
  uint8_t buf[sizeof(struct dns_header) + QNAME_MAX_LEN + 4] = {0};
  dns_query_serialize(query, buf);

  printf("Full query (%zu bytes):\n", query->length);
  for (size_t i = 0; i < query->length; ++i)
    printf("%02x ", buf[i]);
  printf("\n\n");
}
