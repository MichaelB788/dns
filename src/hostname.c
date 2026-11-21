#include "hostname.h"
#include <stddef.h>
#include <stdio.h>
#include <string.h>

bool validate_hostname(const char *host) {
  if (!host) {
    fprintf(stderr, "No hostname given.\n");
    return false;
  }

  const size_t host_len = strlen(host);

  // ensure length is correct
  if (!(1 <= host_len && host_len <= HOSTNAME_MAX)) {
    fprintf(stderr, "Hostname %s has an invalid size of %zu.\n", host,
            host_len);
    return false;
  }

  // no trailing dot
  if (host[host_len - 1] == '.') {
    fprintf(stderr, "Hostname %s ends with a trailing dot.\n", host);
    return false;
  }

  return true;
}

bool validate_label(const char *label) {
  if (!label) {
    fprintf(stderr, "No label given.\n");
    return false;
  }

  const size_t label_len = strlen(label);

  // ensure length is correct
  if (!(1 <= label_len && label_len <= LABEL_MAX)) {
    fprintf(stderr, "Label %s has an invalid size of %zu.\n", label, label_len);
    return false;
  }

  // should not begin or end with hyphen
  if (label[0] == '-' || label[label_len - 1] == '-') {
    fprintf(stderr, "Label %s begins or ends with a hyphen.\n", label);
    return false;
  }

  // check for invalid characters
  for (size_t i = 0; i < label_len; ++i) {
    const char ch = label[i];
    if (!(('0' <= ch && ch <= '9') || ('A' <= ch && ch <= 'Z') ||
          ('a' <= ch && ch <= 'z') || ch == '-')) {
      fprintf(stderr, "Label %s contains an invalid character: %c\n", label,
              ch);
      return false;
    }
  }

  return true;
}
