
#ifndef __DEV_H
#define	__DEV_H

#ifdef __cplusplus
extern "C"
{
#endif

	#include <system.h>
	#include <conio.h>
	#include <pcnet32.h>

	//////////////////////////////////////opt.h///////////////////////////////
	#define LITTLE_ENDIAN           1234
	#define BIG_ENDIAN              4321
	
	#define BYTEORDER               LITTLE_ENDIAN
	
	#define ARP_TABLE_SIZE          32
	#define ARP_XMIT_QUEUE_SIZE     4
	
	#define MTU                     1500             // Maximum transfer unit
	
	#define ICMP_TTL                255              // ICMP time to live
	#define UDP_TTL                 255              // UDP time to live
	#define TCP_TTL                 255              // TCP time to live
	#define RAW_TTL                 255              // RAW time to live
	
	//#define TCP_MIN_SEGLEN          512              // Minimum segment allocation size
	#define TCP_MIN_SEGLEN          (MTU - 40)       // Minimum segment allocation size
	
	#define TCP_MSS                 (MTU - 40)       // Maximum segment size
	#define TCP_WND                 (12 * TCP_MSS)    // TCP window size
	#define TCP_MAXRTX              12               // Maximum number of retransmissions
	#define TCP_SYNMAXRTX           6                // Maximum number of SYN retransmissions 
	#define TCP_MSL                 60000            // The maximum segment lifetime in milliseconds
	
	#define TCP_SND_BUF             (32 * 1024)      // TCP send buffer size
	
	//#define TCP_SND_QUEUELEN        (2 * TCP_SND_BUF / TCP_MSS)
	#define TCP_SND_QUEUELEN        (2 * TCP_SND_BUF / TCP_MIN_SEGLEN)
	
	#define MEM_ALIGNMENT           4
	#define PBUF_POOL_SIZE          128
	#define PBUF_POOL_BUFSIZE       128
	
	#define CHECK_IP_CHECKSUM
	#define CHECK_TCP_CHECKSUM
	#define CHECK_UDP_CHECKSUM
	
	//////////////////////////////////////inifile.h///////////////////////////////
	struct section *krnlcfg;

	struct property;

	struct section
	{
	  char *name;
	  struct section *next;
	  struct property *properties;
	};
	
	struct property
	{
	  char *name;
	  char *value;
	  struct property *next;
	};	

	struct section *find_section(struct section *sect, char *name);
	int get_section_size(struct section *sect);
	char *find_property(struct section *sect, char *name);
	char *get_property(struct section *sections, char *sectname, char *propname, char *defval);
	int get_numeric_property(struct section *sections, char *sectname, char *propname, int defval);
	void free_properties(struct section *sect);
	struct section *parse_properties(char *props);
	void list_properties(FILE* f, struct section *sect);
	struct section *read_properties(char *filename);
	/////////////////////////////////////////////////////////////////////////////////

	///////////////////////////////////////pbuf.h////////////////////////////////////
	#define PBUF_TRANSPORT_HLEN 20
	#define PBUF_IP_HLEN        20
	#define PBUF_LINK_HLEN      14
	
	#define PBUF_TRANSPORT      0
	#define PBUF_IP             1
	#define PBUF_LINK           2
	#define PBUF_RAW            3
	
	#define PBUF_RW             0
	#define PBUF_RO             1
	#define PBUF_POOL           2
	
	// Definitions for the pbuf flag field (these are not the flags that
	// are passed to pbuf_alloc()).
	
	#define PBUF_FLAG_RW    0x00    // Flags that pbuf data is read/write.
	#define PBUF_FLAG_RO    0x01    // Flags that pbuf data is read-only.
	#define PBUF_FLAG_POOL  0x02    // Flags that the pbuf comes from the pbuf pool.
	
	struct pbuf 
	{
	struct pbuf *next;
	
	unsigned short flags;
	unsigned short ref;
	void *payload;
	  
	int tot_len;                // Total length of buffer + additionally chained buffers.
	int len;                    // Length of this buffer.
	int size;                   // Allocated size of buffer
	};
	void pbuf_init();

	struct pbuf *pbuf_alloc(int layer, int size, int type);
	void pbuf_realloc(struct pbuf *p, int size); 
	int pbuf_header(struct pbuf *p, int header_size);
	int pbuf_clen(struct pbuf *p);
	int pbuf_spare(struct pbuf *p);
	void pbuf_ref(struct pbuf *p);
	int pbuf_free(struct pbuf *p);
	void pbuf_chain(struct pbuf *h, struct pbuf *t);
	struct pbuf *pbuf_dechain(struct pbuf *p);
	struct pbuf *pbuf_dup(int layer, struct pbuf *p);
	struct pbuf *pbuf_linearize(int layer, struct pbuf *p);
	struct pbuf *pbuf_cow(int layer, struct pbuf *p);

	/////////////////////////////////////////////////////////////////////////////////
	
	//////////////////////////////////////OS/////////////////////////////////////////
	#define S_IFMT         0170000         // File type mask
	#define S_IFPKT        0160000         // Packet device
	#define S_IFSOCK       0140000         // Socket
	#define S_IFLNK	       0120000         // Symbolic link
	#define S_IFREG        0100000         // Regular file
	#define S_IFBLK        0060000         // Block device
	#define S_IFDIR        0040000         // Directory
	#define S_IFCHR        0020000         // Character device
	#define S_IFIFO        0010000         // Pipe
	/////////////////////////////////////////////////////////////////////////////////

//////////////////////////////netif//////////////////////////////////////
#define NETIF_UP                      0x00000001   // Interface is up
#define NETIF_DHCP                    0x00000002   // Get IP address for interface via DHCP
#define NETIF_LOOPBACK                0x00000004   // Interface is loopback

#define NETIF_MULTICAST               0x00000010   // Supports multicast
#define NETIF_ALLMULTI                0x00000020   // Receive all multicast packets
#define NETIF_PROMISC                 0x00000040   // Receive all packets

#define NETIF_IP_TX_CHECKSUM_OFFLOAD  0x00010000
#define NETIF_IP_RX_CHECKSUM_OFFLOAD  0x00020000
#define NETIF_UDP_RX_CHECKSUM_OFFLOAD 0x00040000
#define NETIF_UDP_TX_CHECKSUM_OFFLOAD 0x00080000
#define NETIF_TCP_RX_CHECKSUM_OFFLOAD 0x00100000
#define NETIF_TCP_TX_CHECKSUM_OFFLOAD 0x00200000

typedef int port_t;
typedef int err_t;

struct ip_addr 
{
  unsigned long addr;
};

struct mclist
{
  struct mclist *next;
  struct ip_addr ipaddr;
  struct eth_addr hwaddr;
};

struct netif 
{
  struct netif *next;
  struct ip_addr ipaddr;
  struct ip_addr netmask;
  struct ip_addr gw;
  struct ip_addr broadcast;
  struct eth_addr hwaddr;
  int flags;
  char name[16];

  struct mclist *mclist;
  int mccount;

  err_t (*input)(struct pbuf *p, struct netif *inp);
  err_t (*output)(struct netif *netif, struct pbuf *p, struct ip_addr *ipaddr);
  
  void *state;
};
/////////////////////////////////////////////////////////////////////////////////

	//////////////////////////////Device Header//////////////////////////////////////
	#ifndef _DEV_T_DEFINED
	#define _DEV_T_DEFINED
	typedef unsigned int dev_t;
	#endif

	#ifndef _BLKNO_T_DEFINED
	#define _BLKNO_T_DEFINED
	typedef unsigned int blkno_t;
	#endif

	#ifndef _UID_T_DEFINED
	#define _UID_T_DEFINED
	typedef unsigned short uid_t;
	#endif

	#ifndef _GID_T_DEFINED
	#define _GID_T_DEFINED
	typedef unsigned short gid_t;
	#endif

	#ifndef _HMODULE_T_DEFINED
	#define _HMODULE_T_DEFINED
	typedef void *hmodule_t;
	#endif

	struct dev;
	struct bus;
	struct unit;
	
	#define NODEV (-1)
	
	#define DEVNAMELEN              32
	#define MAX_DEVS                64
	
	#define DEV_TYPE_STREAM		1
	#define DEV_TYPE_BLOCK		2
	#define DEV_TYPE_PACKET		3
	
	#define DEVFLAG_NBIO            1

	#define IOCTL_GETBLKSIZE        1
	#define IOCTL_GETDEVSIZE        2
	#define IOCTL_GETGEOMETRY       3
	#define IOCTL_REVALIDATE        4
	
	#define RESOURCE_IO	        1
	#define RESOURCE_MEM	        2
	#define RESOURCE_IRQ	        3
	#define RESOURCE_DMA	        4
	
	#define BUSTYPE_HOST            0
	#define BUSTYPE_PCI             1
	#define BUSTYPE_ISA             2
	
	#define BIND_BY_CLASSCODE       1
	#define BIND_BY_UNITCODE        2
	
	//
	// Bus
	//
	
	struct bus
	{
	  struct bus *next;
	  struct bus *sibling;
	  struct bus *parent;
	
	  struct unit *self;

	  unsigned long bustype;
	  unsigned long busno;
	
	  struct unit *units;
	  struct bus *bridges;
	};
	
	//
	// Unit
	//
	
	struct unit
	{
	  struct unit *next;
	  struct unit *sibling;
	  struct bus *bus;
	  struct dev *dev;
	
	  unsigned long classcode;
	  unsigned long unitcode;
	  unsigned long subunitcode;
	  unsigned long revision;
	  unsigned long unitno;
	
	  char *classname;
	  char *vendorname;
	  char *productname;
	
	  struct resource *resources;
	};
	
	//
	// Resource
	//
	
	struct resource
	{
	  struct resource *next;
	  unsigned short type;
	  unsigned short flags;
	  unsigned long start;
	  unsigned long len;
	};
	
	//
	// Binding
	//
	
	struct binding
	{
	  int bindtype;
	  unsigned long bustype;
	  unsigned long code;
	  unsigned long mask;
	  char *module;
	};

	//
	// Driver
	//
	
	struct driver
	{
	  char *name;
	  int type;
	
	  int (*ioctl)(struct dev *dev, int cmd, void *args, size_t size);
	  int (*read)(struct dev *dev, void *buffer, size_t count, blkno_t blkno, int flags);
	  int (*write)(struct dev *dev, void *buffer, size_t count, blkno_t blkno, int flags);
	
	  int (*attach)(struct dev *dev, struct eth_addr *hwaddr);
	  int (*detach)(struct dev *dev);
	  int (*transmit)(struct dev *dev, struct pbuf *p);
	  int (*set_rx_mode)(struct dev *dev);
	};
	
	//
	// Device
	//
	
	struct dev 
	{
	  char name[DEVNAMELEN];
	  struct driver *driver;
	  struct unit *unit;
	  void *privdata;
	  int refcnt;
	  uid_t uid;
	  gid_t gid;
	  int mode;
	  struct devfile *files;
	
	  struct netif *netif;
	  int (*receive)(struct netif *netif, struct pbuf *p);
	};
	
	//
	// Geometry
	//

	struct geometry
	{
	  int cyls;
	  int heads;
	  int spt;
	  int sectorsize;
	  int sectors;
	};
	
	//
	// Board info
	//
	
	struct board
	{
	  char *vendorname;
	  char *productname;
	  unsigned long bustype;
	  unsigned long unitcode;
	  unsigned long unitmask;
	  unsigned long subsystemcode;
	  unsigned long subsystemmask;
	  unsigned long revisioncode;
	  unsigned long revisionmask;
	  int flags;
	};
	
	extern struct dev *devtab[];
	extern unsigned int num_devs;
	
	extern struct unit *units;
	extern struct bus *buses;
	
	void enum_host_bus();
	void install_drivers();
	
	 struct bus *add_bus(struct unit *self, unsigned long bustype, unsigned long busno);
	 struct unit *add_unit(struct bus *bus, unsigned long classcode, unsigned long unitcode, unsigned long unitno);
	 struct resource *add_resource(struct unit *unit, unsigned short type, unsigned short flags, unsigned long start, unsigned long len);
	
	 struct resource *get_unit_resource(struct unit *unit, int type, int num);
	 int get_unit_irq(struct unit *unit);
	 int get_unit_iobase(struct unit *unit);
	 void *get_unit_membase(struct unit *unit);
	 char *get_unit_name(struct unit *unit);
	
	 struct unit *lookup_unit(struct unit *start, unsigned long unitcode, unsigned long unitmask);
	 struct unit *lookup_unit_by_class(struct unit *start, unsigned long classcode, unsigned long classmask);
	
	 struct board *lookup_board(struct board *board_tbl, struct unit *unit);
	
	 struct dev *device(dev_t devno);
	
	 dev_t dev_make(char *name, struct driver *driver, struct unit *unit, void *privdata);
	 dev_t dev_open(char *name);
	 int dev_close(dev_t devno);

	 int dev_ioctl(dev_t devno, int cmd, void *args, size_t size);
	 int dev_read(dev_t devno, void *buffer, size_t count, blkno_t blkno, int flags);
	 int dev_write(dev_t devno, void *buffer, size_t count, blkno_t blkno, int flags);
	
	 int dev_attach(dev_t dev, struct netif *netif, int (*receive)(struct netif *netif, struct pbuf *p));
	 int dev_detach(dev_t devno);
	 int dev_transmit(dev_t devno, struct pbuf *p);
	 int dev_receive(dev_t devno, struct pbuf *p);
	
	 int dev_setevt(dev_t devno, int events);
	 int dev_clrevt(dev_t devno, int events);

		
#ifdef __cplusplus
	}
#endif

#endif
