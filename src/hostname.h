#pragma once
#include <stdbool.h>
#include <stddef.h>

#define HOSTNAME_MAX 253ul
#define LABEL_MAX 63ul

bool validate_hostname(const char *host);
bool validate_label(const char *label);
