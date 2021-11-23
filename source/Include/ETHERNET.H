
#ifndef __ETHERNET_H
#define	__ETHERNET_H

#ifdef __cplusplus
extern "C"
{
#endif

	#include <fs.h>
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
	extern struct section *krnlcfg;

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

		
#ifdef __cplusplus
	}
#endif

#endif
