
#ifndef __PCNET32_H
#define	__PCNET32_H

#ifdef __cplusplus
extern "C"
{
#endif
	
	#include <dev.h>
	#include <system.h>
	

	#define WMWARE

////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////ether.h///////////////////////////////
	#define ETHER_HLEN 14
	
	#define ETHER_ADDR_LEN 6
	
	struct eth_addr 
	{
	  unsigned char addr[ETHER_ADDR_LEN];
	};
	  
	struct eth_hdr 
	{
	  struct eth_addr dest;
	  struct eth_addr src;
	  unsigned short type;
	};
/////////////////////////////////////////////////////////////////////////////////


// PCI IDs

#define PCI_DEVICE_PCNET32	0x2000
#define PCI_VENDOR_AMD		0x1022

// Offsets from base I/O address

#define PCNET32_WIO_RDP		0x10 // Register data port
#define PCNET32_WIO_RAP		0x12 // Register address port
#define PCNET32_WIO_RESET	0x14
#define PCNET32_WIO_BDP		0x16

#define PCNET32_DWIO_RDP	0x10
#define PCNET32_DWIO_RAP	0x14
#define PCNET32_DWIO_RESET	0x18
#define PCNET32_DWIO_BDP	0x1C

// Controller Status Registers

#define CSR                     0
#define INIT_BLOCK_ADDRESS_LOW  1
#define INIT_BLOCK_ADDRESS_HIGH 2
#define INTERRUPT_MASK          3
#define FEATURE_CONTROL         4
#define CHIP_ID_LOWER           88
#define CHIP_ID_UPPER           89

// Controller Status Register (CSR0) Bits

#define CSR_ERR                 0x8000
#define CSR_BABL                0x4000
#define CSR_CERR                0x2000
#define CSR_MISS                0x1000
#define CSR_MERR                0x0800
#define CSR_RINT                0x0400
#define CSR_TINT                0x0200
#define CSR_IDON                0x0100
#define CSR_INTR                0x0080
#define CSR_IENA                0x0040
#define CSR_RXON                0x0020
#define CSR_TXON                0x0010
#define CSR_TDMD                0x0008
#define CSR_STOP                0x0004
#define CSR_STRT                0x0002
#define CSR_INIT                0x0001

// Miscellaneous Configuration (BCR2)

#define MISCCFG                 2
#define MISCCFG_TMAULOOP        0x4000
#define MISCCFG_APROMWE         0x0100
#define MISCCFG_INTLEVEL        0x0080
#define MISCCFG_DXCVRCTL        0x0020
#define MISCCFG_DXCVRPOL        0x0010
#define MISCCFG_EADISEL         0x0008
#define MISCCFG_AWAKE           0x0004
#define MISCCFG_ASEL            0x0002
#define MISCCFG_XMAUSEL         0x0001
#define MISCCFG_                0x0000
#define MISCCFG_                0x0000

// Transmit Descriptor (TMD) Status

#define TMD_OWN                 0x8000
#define TMD_ERR                 0x4000
#define TMD_ADD_FCS             0x2000  // ADD_FCS and NO_FCS is controlled through the same bit
#define TMD_NO_FCS              0x2000
#define TMD_MORE                0x1000  // MORE and LTINT is controlled through the same bit
#define TMD_LTINT               0x1000
#define TMD_ONE                 0x0800
#define TMD_DEF                 0x0400
#define TMD_STP                 0x0200
#define TMD_ENP                 0x0100
#define TMD_BPE                 0x0080
#define TMD_RES                 0x007F

// Receive Descriptor (RMD) Status

#define RMD_OWN                 0x8000
#define RMD_ERR                 0x4000
#define RMD_FRAM                0x2000
#define RMD_OFLO                0x1000
#define RMD_CRC                 0x0800
#define RMD_BUFF                0x0400
#define RMD_STP                 0x0200
#define RMD_ENP                 0x0100
#define RMD_BPE                 0x0080
#define RMD_PAM                 0x0040
#define RMD_LAFM                0x0020
#define RMD_BAM                 0x0010

// Size of Tx and Rx rings

#ifndef PCNET32_LOG_TX_BUFFERS
#define PCNET32_LOG_TX_BUFFERS  4
#define PCNET32_LOG_RX_BUFFERS  5
#endif

#define TX_RING_SIZE		(1 << (PCNET32_LOG_TX_BUFFERS))
#define TX_RING_MOD_MASK	(TX_RING_SIZE - 1)
#define TX_RING_LEN_BITS	((PCNET32_LOG_TX_BUFFERS) << 12)

#define RX_RING_SIZE		(1 << (PCNET32_LOG_RX_BUFFERS))
#define RX_RING_MOD_MASK	(RX_RING_SIZE - 1)
#define RX_RING_LEN_BITS	((PCNET32_LOG_RX_BUFFERS) << 4)

#define ETHER_FRAME_LEN         1544
#define TX_TIMEOUT              5000

//#pragma pack(push)
//#pragma pack(1)

// Rx and Tx ring descriptors

struct pcnet32_rx_head
{
  unsigned long buffer;
  short length;
  unsigned short status;
  unsigned long msg_length;
  unsigned long reserved;
};
	
struct pcnet32_tx_head
{
  unsigned long buffer;
  short length;
  unsigned short status;
  unsigned long misc;
  unsigned long reserved;
};

// Initialization block

struct pcnet32_init_block
{
  unsigned short mode;
  unsigned short tlen_rlen;
  unsigned char phys_addr[6];
  unsigned short reserved;
  unsigned long filter[2];
  // Receive and transmit ring base, along with extra bits
  unsigned long rx_ring;
  unsigned long tx_ring;
};

//#pragma pack(pop)

// PCnet32 access functions

struct pcnet32_access
{
  unsigned short (*read_csr)(unsigned short, int);
  void (*write_csr)(unsigned short, int, unsigned short);
  unsigned short (*read_bcr)(unsigned short, int);
  void (*write_bcr)(unsigned short, int, unsigned short);
  unsigned short (*read_rap)(unsigned short);
  void (*write_rap)(unsigned short, unsigned short);
  void (*reset)(unsigned short);
};

struct pcnet32
{
  struct pcnet32_rx_head rx_ring[RX_RING_SIZE];
  struct pcnet32_tx_head tx_ring[TX_RING_SIZE];
  struct pcnet32_init_block init_block;
  struct pbuf *rx_buffer[RX_RING_SIZE];
  struct pbuf *tx_buffer[TX_RING_SIZE];

  unsigned long phys_addr;              // Physical address of this structure
  unsigned int devno;                          // Device number

  unsigned short iobase;		// Configured I/O base
  unsigned short irq;		        // Configured IRQ
  unsigned short membase;               // Configured memory base

  unsigned long next_rx;                // Next entry to receive

  //struct sem sem_tx;
  unsigned long size_tx;                // Number of outstanding transmit ring entries
  unsigned long curr_tx;                // First outstanding transmit ring entry
  unsigned long next_tx;                // Next transmit ring entry

  //struct interrupt intr;                // Interrupt object for driver
  //struct dpc dpc;                       // DPC for driver

  //struct event rdc;	                // Remote DMA completed event
  //struct event ptx;                     // Packet transmitted event

  struct eth_addr hwaddr;               // MAC address for NIC

  // Access functions
  unsigned short (*read_csr)(unsigned short, int);
  void (*write_csr)(unsigned short, int, unsigned short);
  unsigned short (*read_bcr)(unsigned short, int);
  void (*write_bcr)(unsigned short, int, unsigned short);
  unsigned short (*read_rap)(unsigned short);
  void (*write_rap)(unsigned short, unsigned short);
  void (*reset)(unsigned short);
};

//int install(struct unit *unit);
#ifndef offsetof
#define offsetof(s, m) ((size_t)&(((s *) 0)->m))
#endif


#ifdef __cplusplus
	}
#endif

#endif
