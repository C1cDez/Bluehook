#pragma once

#include "bthutils.h"

int bthsocks_init(void);
int bthsocks_cleanup(void);

int bthsocks_ping(BLUETOOTH_ADDRESS_STRUCT bth_addr, int packets, int timeout);
