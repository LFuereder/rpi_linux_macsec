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

#define HW_AEAD_AVAILABLE 1

/* cross-driver XDMA API */
extern ssize_t drv_access_char_sgdma_write(const char *buf, size_t count, loff_t *pos);
extern ssize_t drv_access_char_sgdma_read(const char *buf, size_t count, loff_t *pos);

#endif