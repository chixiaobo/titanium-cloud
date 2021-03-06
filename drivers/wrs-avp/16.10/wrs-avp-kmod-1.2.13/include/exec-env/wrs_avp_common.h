/*-
 *   This file is provided under a dual BSD/LGPLv2 license.  When using or
 *   redistributing this file, you may do so under either license.
 *
 *   GNU LESSER GENERAL PUBLIC LICENSE
 *
 *   Copyright(c) 2010-2013 Intel Corporation. All rights reserved.
 *   Copyright(c) 2014-2015 Wind River Systems, Inc. All rights reserved.
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of version 2.1 of the GNU Lesser General Public License
 *   as published by the Free Software Foundation.
 *
 *   This program is distributed in the hope that it will be useful, but
 *   WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *   Lesser General Public License for more details.
 *
 *   You should have received a copy of the GNU Lesser General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.
 *
 *   Contact Information:
 *   Wind River Systems, Inc.
 *
 *
 *   BSD LICENSE
 *
 *   Copyright(c) 2010-2013 Intel Corporation. All rights reserved.
 *   Copyright(c) 2014 Wind River Systems, Inc. All rights reserved.
 *   All rights reserved.
 *
 *   Redistribution and use in source and binary forms, with or without
 *   modification, are permitted provided that the following conditions
 *   are met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in
 *     the documentation and/or other materials provided with the
 *     distribution.
 *   * Neither the name of Intel Corporation nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 *    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *    "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *    LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *    A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *    OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *    SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *    LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *    DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *    THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#ifndef _WRS_AVP_COMMON_H_
#define _WRS_AVP_COMMON_H_

#ifdef __KERNEL__
#include <linux/if.h>
#endif

/**
 * AVP name is part of network device name.
 */
#define WRS_AVP_NAMESIZE 32

/**
 * AVP alias is a user-defined value used for lookups from secondary
 * processes.  Typically, this is a UUID.
 */
#define WRS_AVP_ALIASSIZE 128

/**
 * Memory aligment (cache aligned)
 */
#ifndef WRS_AVP_ALIGNMENT
#define WRS_AVP_ALIGNMENT 64
#endif

/*
 * Request id.
 */
enum wrs_avp_req_id {
	WRS_AVP_REQ_UNKNOWN = 0,
	WRS_AVP_REQ_CHANGE_MTU,
	WRS_AVP_REQ_CFG_NETWORK_IF,
	WRS_AVP_REQ_CFG_DEVICE,
	WRS_AVP_REQ_SHUTDOWN_DEVICE,
	WRS_AVP_REQ_MAX,
};

/**@{ AVP device driver types */
#define WRS_AVP_DRIVER_TYPE_UNKNOWN 0
#define WRS_AVP_DRIVER_TYPE_DPDK 1
#define WRS_AVP_DRIVER_TYPE_KERNEL 2
#define WRS_AVP_DRIVER_TYPE_QEMU 3
/**@} */

/**@{ AVP device operational modes */
#define WRS_AVP_MODE_HOST 0  /**< AVP interface created in host */
#define WRS_AVP_MODE_GUEST 1 /**< AVP interface created for export to guest */
#define WRS_AVP_MODE_TRACE 2 /**< AVP interface created for packet tracing */
/**@} */

/*
 * Structure for AVP queue configuration query request/result
 */
struct wrs_avp_device_config {
	uint64_t device_id;  /**< Unique system identifier */
    uint32_t driver_type; /**< Device Driver type */
    uint32_t driver_version; /**< Device Driver version */
    uint32_t features; /**< Negotiated features */
	uint16_t num_tx_queues; /**< Number of active transmit queues */
	uint16_t num_rx_queues; /**< Number of active receive queues */
    uint8_t if_up; /**< 1: interface up, 0: interface down */
} __attribute__((__packed__));

/*
 * Structure for AVP request.
 */
struct wrs_avp_request {
	uint32_t req_id;             /**< Request id */
	union {
		uint32_t new_mtu;    /**< New MTU */
		uint8_t if_up;       /**< 1: interface up, 0: interface down */
		struct wrs_avp_device_config config; /**< Queue configuration */
	};
	int32_t result;               /**< Result for processing request */
} __attribute__((__packed__));

/*
 * FIFO struct mapped in a shared memory. It describes a circular buffer FIFO
 * Write and read should wrap around. FIFO is empty when write == read
 * Writing should never overwrite the read position
 */
struct wrs_avp_fifo {
	volatile unsigned write;     /**< Next position to be written*/
	volatile unsigned read;      /**< Next position to be read */
	unsigned len;                /**< Circular buffer length */
	unsigned elem_size;          /**< Pointer size - for 32/64 bit OS */
	void * volatile buffer[0];   /**< The buffer contains mbuf pointers */
};


/*
 * AVP packet buffer header used to define the exchange of packet data.
 */
struct wrs_avp_mbuf {
	uint64_t pad0;
	void *pkt_mbuf;         /**< Reference to packet mbuf */
	uint8_t pad1[14];
	uint16_t ol_flags;      /**< Offload features. */
	void *next;             /**< Reference to next buffer in chain */
	void *data;             /**< Start address of data in segment buffer. */
	uint16_t data_len;      /**< Amount of data in segment buffer. */
    uint8_t nb_segs;        /**< Number of segments */
	uint8_t pad2;
	uint16_t pkt_len;       /**< Total pkt len: sum of all segment data_len. */
    uint32_t pad3;
    uint16_t vlan_tci;      /**< VLAN Tag Control Identifier (CPU order). */
    uint32_t pad4;
} __attribute__((__aligned__(WRS_AVP_ALIGNMENT), __packed__));


/**{ AVP device features */
#define WRS_AVP_FEATURE_VLAN_OFFLOAD (1<<0) /**< Emulated HW VLAN offload */
/**@} */


/**@{ Offload feature flags */
#define WRS_AVP_TX_VLAN_PKT 0x0001 /**< TX packet is a 802.1q VLAN packet. */
#define WRS_AVP_RX_VLAN_PKT 0x0800 /**< RX packet is a 802.1q VLAN packet. */
/**@} */


/**@{ AVP PCI identifiers */
#define WRS_AVP_PCI_VENDOR_ID   0x1af4
#define WRS_AVP_PCI_DEVICE_ID   0x1110
/**@} */

/**@{ AVP PCI subsystem identifiers */
#define WRS_AVP_PCI_SUB_VENDOR_ID WRS_AVP_PCI_VENDOR_ID
#define WRS_AVP_PCI_SUB_DEVICE_ID 0x1104
/**@} */

/**@{ AVP PCI BAR definitions */
#define WRS_AVP_PCI_MMIO_BAR   0
#define WRS_AVP_PCI_MSIX_BAR   1
#define WRS_AVP_PCI_MEMORY_BAR 2
#define WRS_AVP_PCI_MEMMAP_BAR 4
#define WRS_AVP_PCI_DEVICE_BAR 5
#define WRS_AVP_PCI_MAX_BAR    6
/**@} */

/**@{ AVP PCI BAR name definitions */
#define WRS_AVP_MMIO_BAR_NAME   "avp-mmio"
#define WRS_AVP_MSIX_BAR_NAME   "avp-msix"
#define WRS_AVP_MEMORY_BAR_NAME "avp-memory"
#define WRS_AVP_MEMMAP_BAR_NAME "avp-memmap"
#define WRS_AVP_DEVICE_BAR_NAME "avp-device"
/**@} */

/**@{ AVP PCI MSI-X vectors */
#define WRS_AVP_MIGRATION_MSIX_VECTOR 0 /**< Migration interrupts */
#define WRS_AVP_MAX_MSIX_VECTORS 1
/**@} */

/**@} AVP Migration status/ack register values */
#define WRS_AVP_MIGRATION_NONE      0 /**< Migration never executed */
#define WRS_AVP_MIGRATION_DETACHED  1 /**< Device attached during migration */
#define WRS_AVP_MIGRATION_ATTACHED  2 /**< Device reattached during migration */
#define WRS_AVP_MIGRATION_ERROR     3 /**< Device failed to attach/detach */
/**@} */

/**@} AVP MMIO Register Offsets */
#define WRS_AVP_REGISTER_BASE 0
#define WRS_AVP_INTERRUPT_MASK_OFFSET (WRS_AVP_REGISTER_BASE + 0)
#define WRS_AVP_INTERRUPT_STATUS_OFFSET (WRS_AVP_REGISTER_BASE + 4)
#define WRS_AVP_MIGRATION_STATUS_OFFSET (WRS_AVP_REGISTER_BASE + 8)
#define WRS_AVP_MIGRATION_ACK_OFFSET (WRS_AVP_REGISTER_BASE + 12)
/**@} */

/**@} AVP Interrupt Status Mask */
#define WRS_AVP_MIGRATION_INTERRUPT_MASK (1 << 1)
#define WRS_AVP_ALL_INTERRUPTS_MASK      (0xFFFFFFFF)
#define WRS_AVP_NO_INTERRUPTS_MASK       (0)
/**@} */

/*
 * Maximum number of memory regions to export
 */
#define WRS_AVP_MAX_MAPS  2048

/*
 * Description of a single memory region
 */
struct wrs_avp_memmap {
    void *addr;
    phys_addr_t phys_addr;
    uint64_t length;
};

/*
 * AVP memory mapping validation marker
 */
#define WRS_AVP_MEMMAP_MAGIC (0x20131969)

/**@{  AVP memory map versions */
#define WRS_AVP_MEMMAP_VERSION_1 1
#define WRS_AVP_MEMMAP_VERSION WRS_AVP_MEMMAP_VERSION_1
/**@} */

/*
 * Defines a list of memory regions exported from the host to the guest
 */
struct wrs_avp_memmap_info {
    uint32_t magic; /**< Memory validation marker */
    uint32_t version; /**< Data format version */
    uint32_t nb_maps;
    struct wrs_avp_memmap maps[WRS_AVP_MAX_MAPS];
};

/*
 * AVP device memory validation marker
 */
#define WRS_AVP_DEVICE_MAGIC (0x20131975)

/**@{  AVP device map versions
 * WARNING:  do not change the format or names of these variables.  They are
 * automatically parsed from the build system to generate the SDK package
 * name.
 **/
#define WRS_AVP_RELEASE_VERSION_1 1
#define WRS_AVP_RELEASE_VERSION WRS_AVP_RELEASE_VERSION_1
#define WRS_AVP_MAJOR_VERSION_0 0
#define WRS_AVP_MAJOR_VERSION_1 1
#define WRS_AVP_MAJOR_VERSION_2 2
#define WRS_AVP_MAJOR_VERSION WRS_AVP_MAJOR_VERSION_2
#define WRS_AVP_MINOR_VERSION_0 0
#define WRS_AVP_MINOR_VERSION_1 1
#define WRS_AVP_MINOR_VERSION_13 13
#define WRS_AVP_MINOR_VERSION WRS_AVP_MINOR_VERSION_13
/**@} */


/**
 * Generates a 32-bit version number from the specified version number
 * components
 */
#define WRS_AVP_MAKE_VERSION(_release, _major, _minor) \
    ((((_release) & 0xffff) << 16) | (((_major) & 0xff) << 8) | ((_minor) & 0xff))


/**
 * Represents the current version of the AVP host driver
 * WARNING:  in the current development branch the host and guest driver
 * version should always be the same.  When patching guest features back to
 * GA releases the host version number should not be updated unless there was
 * an actual change made to the host driver.
 */
#define WRS_AVP_CURRENT_HOST_VERSION \
    WRS_AVP_MAKE_VERSION(WRS_AVP_RELEASE_VERSION_1, WRS_AVP_MAJOR_VERSION_0, WRS_AVP_MINOR_VERSION_1)


/**
 * Represents the current version of the AVP guest drivers
 */
#define WRS_AVP_CURRENT_GUEST_VERSION \
    WRS_AVP_MAKE_VERSION(WRS_AVP_RELEASE_VERSION_1, WRS_AVP_MAJOR_VERSION_2, WRS_AVP_MINOR_VERSION_13)

/**
 * Access AVP device version values
 */
#define WRS_AVP_GET_RELEASE_VERSION(_version) (((_version) >> 16) & 0xffff)
#define WRS_AVP_GET_MAJOR_VERSION(_version) (((_version) >> 8) & 0xff)
#define WRS_AVP_GET_MINOR_VERSION(_version) ((_version) & 0xff)
/**@}*/


/**
 * Remove the minor version number so that only the release and major versions
 * are used for comparisons.
 */
#define WRS_AVP_STRIP_MINOR_VERSION(_version) ((_version) >> 8)


/**
 * Defines the number of mbuf pools supported per device (1 per socket)
 * @note This value should be equal to RTE_MAX_NUMA_NODES
 */
#define WRS_AVP_MAX_MEMPOOLS (8)

/*
 * Defines address translation parameters for each support mbuf pool
 */
struct wrs_avp_mempool_info {
	void * addr;
	phys_addr_t phys_addr;
	uint64_t length;
};

/*
 * Struct used to create a AVP device. Passed to the kernel in IOCTL call or
 * via inter-VM shared memory when used in a guest.
 */
struct wrs_avp_device_info {
	uint32_t magic; /**< Memory validation marker */
	uint32_t version; /**< Data format version */

	char ifname[WRS_AVP_NAMESIZE];  /**< Network device name for AVP */

	phys_addr_t tx_phys;
	phys_addr_t rx_phys;
	phys_addr_t alloc_phys;
	phys_addr_t free_phys;

	uint32_t features;            /**< Supported feature bitmap */
	uint8_t min_rx_queues;        /**< Minimum supported receive/free queues */
	uint8_t num_rx_queues;        /**< Recommended number of receive/free queues */
	uint8_t max_rx_queues;        /**< Maximum supported receive/free queues */
	uint8_t min_tx_queues;        /**< Minimum supported transmit/alloc queues */
	uint8_t num_tx_queues;        /**< Recommended number of transmit/alloc queues */
	uint8_t max_tx_queues;        /**< Maximum supported transmit/alloc queues */

	uint32_t tx_size;            /**< Size of each transmit queue */
	uint32_t rx_size;            /**< Size of each receive queue */
	uint32_t alloc_size;         /**< Size of each alloc queue */
	uint32_t free_size;          /**< Size of each free queue */

	/* Used by Ethtool */
	phys_addr_t req_phys;
	phys_addr_t resp_phys;
	phys_addr_t sync_phys;
	void * sync_va;

	/* mbuf mempool (used when a single memory area is supported) */
	void * mbuf_va;
	phys_addr_t mbuf_phys;

	/* mbuf mempools */
	struct wrs_avp_mempool_info pool[WRS_AVP_MAX_MEMPOOLS];

#ifdef __KERNEL__
    /* Ethernet info */
    char ethaddr[ETH_ALEN];
#else
    char ethaddr[ETHER_ADDR_LEN];
#endif

	uint8_t mode;                /**< device mode, i.e guest, host, trace */

	/* mbuf size */
	unsigned mbuf_size;

    /*
     * unique id to differentiate between two instantiations of the same AVP
     * device (i.e., the guest needs to know if the device has been deleted
     * and recreated).
     */
    uint64_t device_id;

    uint32_t max_rx_pkt_len;      /**< Maximum receive unit size */
};

#define WRS_AVP_MAX_QUEUES (8) /**< Maximum number of queues per device */

/** Maximum number of chained mbufs in a packet */
#define WRS_AVP_MAX_MBUF_SEGMENTS (5)

#define WRS_AVP_DEVICE "avp"

#define WRS_AVP_IOCTL_TEST    _IOWR(0, 1, int)
#define WRS_AVP_IOCTL_CREATE  _IOWR(0, 2, struct wrs_avp_device_info)
#define WRS_AVP_IOCTL_RELEASE _IOWR(0, 3, struct wrs_avp_device_info)
#define WRS_AVP_IOCTL_QUERY   _IOWR(0, 4, struct wrs_avp_device_config)

#endif /* _WRS_AVP_COMMON_H_ */
