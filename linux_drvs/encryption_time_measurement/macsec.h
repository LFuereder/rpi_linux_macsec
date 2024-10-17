#ifndef HW_MACSEC_H
#define HW_MACSEC_H

#include <linux/types.h>
#include <linux/skbuff.h>
#include <linux/socket.h>
#include <linux/module.h>
#include <crypto/aead.h>
#include <linux/etherdevice.h>
#include <linux/netdevice.h>
#include <linux/rtnetlink.h>
#include <linux/refcount.h>
#include <net/genetlink.h>
#include <net/sock.h>
#include <net/gro_cells.h>
#include <net/macsec.h>
#include <net/dst_metadata.h>
#include <linux/phy.h>
#include <linux/byteorder/generic.h>
#include <linux/if_arp.h>

#include <uapi/linux/if_macsec.h>

#include "gpio_driver.h"

/* API to access GPIO driver operations within the kernel space */
extern void driver_toggle_gpio(char state);

#endif