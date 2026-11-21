#include "dns_reply.h"
#include "dns_common.h"
#include <stddef.h>
#include <stdint.h>
#include <string.h>

void dns_resource_record_init(struct dns_resource_record *rr,
                              const uint8_t *buf) {
  size_t bytes_read = 0;
  memset(rr, 0, sizeof(struct dns_header));
  bytes_read += sizeof(struct dns_header);

  size_t label_len = 0;
  for (const uint8_t *ptr = buf + sizeof(struct dns_header); ptr[0] != 0;
       ptr += label_len) {
    label_len = ptr[0];
    memcpy(rr->question.qname, ptr + 1, label_len);
    bytes_read += label_len + 1;
  }
  rr->question.qname[bytes_read++] = 0;
  rr->question.qname_len = bytes_read - sizeof(struct dns_header);

  const uint16_t *ptr = (uint16_t *)(buf + bytes_read);
  rr->question.qtype = ptr[0];
  bytes_read += 2;
  rr->question.qtype = ptr[1];
  bytes_read += 2;
}
