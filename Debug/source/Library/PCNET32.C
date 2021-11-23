#include <pcnet32.h>
#include <dev.h>

void dump_csr(unsigned short csr)
{
  kprintf("CRS: ");
  if (csr & CSR_ERR) kprintf(" ERR");
  if (csr & CSR_BABL) kprintf(" BABL");
  if (csr & CSR_CERR) kprintf(" CERR");
  if (csr & CSR_MISS) kprintf(" MISS");
  if (csr & CSR_MERR) kprintf(" MERR");
  if (csr & CSR_RINT) kprintf(" RINT");
  if (csr & CSR_TINT) kprintf(" TINT");
  if (csr & CSR_IDON) kprintf(" IDON");
  if (csr & CSR_INTR) kprintf(" INTR");
  if (csr & CSR_IENA) kprintf(" IENA");
  if (csr & CSR_RXON) kprintf(" RXON");
  if (csr & CSR_TXON) kprintf(" TXON");
  if (csr & CSR_TDMD) kprintf(" TDMD");
  if (csr & CSR_STOP) kprintf(" STOP");
  if (csr & CSR_STRT) kprintf(" STRT");
  if (csr & CSR_INIT) kprintf(" INIT");
  kprintf("\n");
}

static unsigned short pcnet32_wio_read_csr(unsigned short addr, int index)
{
  outp16((unsigned short) (addr + PCNET32_WIO_RAP), (unsigned short) index);
  return inp16((unsigned short) (addr + PCNET32_WIO_RDP));
}

static void pcnet32_wio_write_csr(unsigned short addr, int index, unsigned short val)
{	
  outp16((unsigned short) (addr + PCNET32_WIO_RDP), val);  
  outp16((unsigned short) (addr + PCNET32_WIO_RAP), (unsigned short) index);  
}

static unsigned short pcnet32_wio_read_bcr(unsigned short addr, int index)
{
  outp16((unsigned short) (addr + PCNET32_WIO_RAP), (unsigned short) index);
  return inp16((unsigned short) (addr + PCNET32_WIO_BDP));
}

static void pcnet32_wio_write_bcr(unsigned short addr, int index, unsigned short val)
{
  outp16((unsigned short) (addr + PCNET32_WIO_RAP), (unsigned short) index);
  outp16((unsigned short) (addr + PCNET32_WIO_BDP), val);
}

static unsigned short pcnet32_wio_read_rap(unsigned short addr)
{
  return inp16((unsigned short) (addr + PCNET32_WIO_RAP));
}

static void pcnet32_wio_write_rap(unsigned short addr, unsigned short val)
{
  outp16((unsigned short) (addr + PCNET32_WIO_RAP), val);
}

static void pcnet32_wio_reset(unsigned short addr)
{
  inp16((unsigned short) (addr + PCNET32_WIO_RESET));
}

static int pcnet32_wio_check(unsigned short addr)
{
  outp16((unsigned short) (addr + PCNET32_WIO_RAP), 88);
  return inp16((unsigned short) (addr + PCNET32_WIO_RAP)) == 88;
}

static struct pcnet32_access pcnet32_wio =
{
  pcnet32_wio_read_csr,
  pcnet32_wio_write_csr,
  pcnet32_wio_read_bcr,
  pcnet32_wio_write_bcr,
  pcnet32_wio_read_rap,
  pcnet32_wio_write_rap,
  pcnet32_wio_reset
};

static unsigned short pcnet32_dwio_read_csr(unsigned short addr, int index)
{
  outp32((unsigned short) (addr + PCNET32_DWIO_RAP), index);
  return (unsigned short) (inp32((unsigned short) (addr + PCNET32_DWIO_RDP)) & 0xffff);
}

static void pcnet32_dwio_write_csr (unsigned short addr, int index, unsigned short val)
{
  outp32((unsigned short) (addr + PCNET32_DWIO_RAP), index);
  outp32((unsigned short) (addr + PCNET32_DWIO_RDP), val);
}

static unsigned short pcnet32_dwio_read_bcr(unsigned short addr, int index)
{
  outp32((unsigned short) (addr + PCNET32_DWIO_RAP), index);
  return (unsigned short) inp32((unsigned short) (addr + PCNET32_DWIO_BDP)) & 0xffff;
}

static void pcnet32_dwio_write_bcr(unsigned short addr, int index, unsigned short val)
{
  outp32((unsigned short) (addr + PCNET32_DWIO_RAP), index);
  outp32((unsigned short) (addr + PCNET32_DWIO_BDP), val);
}

static unsigned short pcnet32_dwio_read_rap(unsigned short addr)
{
  return (unsigned short) inp32((unsigned short) (addr + PCNET32_DWIO_RAP)) & 0xffff;
}

static void pcnet32_dwio_write_rap(unsigned short addr, unsigned short val)
{
  outp32((unsigned short) (addr + PCNET32_DWIO_RAP), val);
}

static void pcnet32_dwio_reset(unsigned short addr)
{
  inp32((unsigned short) (addr + PCNET32_DWIO_RESET));
}

static int pcnet32_dwio_check(unsigned short addr)
{
  outp32((unsigned short) (addr + PCNET32_DWIO_RAP), 88);
  return inp32((unsigned short) (addr + PCNET32_DWIO_RAP)) == 88;
}

static struct pcnet32_access pcnet32_dwio =
{
  pcnet32_dwio_read_csr,
  pcnet32_dwio_write_csr,
  pcnet32_dwio_read_bcr,
  pcnet32_dwio_write_bcr,
  pcnet32_dwio_read_rap,
  pcnet32_dwio_write_rap,
  pcnet32_dwio_reset
};

int pcnet32_transmit(struct dev *dev, struct pbuf *p)
{
  struct pcnet32 *pcnet32 = dev->privdata;
  int len;
  int left;
  struct pbuf *q;
  int entry = pcnet32->next_tx;
  unsigned short status;

  //kprintf("pcnet32: transmit packet len=%d\n", p->tot_len);

  if (!p || p->len == 0) return 0;//-EINVAL;

//#ifdef WMWARE
//  if (wait_for_object(&pcnet32->sem_tx, TX_TIMEOUT) < 0)
//  {
//    kprintf("pcnet32: transmit timeout, drop packet\n");
//    stats.link.drop++;
//    return -ETIMEOUT;
//  }
//
//  p = pbuf_linearize(PBUF_RAW, p);
//#else
//  // Wait for free entries in transmit ring
//  for (q = p; q != NULL; q = q->next)
//  {
//    if (q->len > 0)
//    {
//      if (wait_for_object(&pcnet32->sem_tx, TX_TIMEOUT) < 0)
//      {
//	kprintf("pcnet32: transmit timeout, drop packet\n");
//	stats.link.drop++;
//	return -ETIMEOUT;
//      }
//    }
//  }
//#endif

  // Set Start of Packet (STP) in first buffer
  status = TMD_OWN | TMD_STP;

  // Place each part of the packet in the transmit ring
  left = p->tot_len;
  for (q = p; q != NULL; q = q->next) 
  {
    len = q->len;
    left -= len;
    if (len > 0)
    {
      //kprintf("pcnet32_transmit: sending %d (entry = %d)\n", len, entry);

      // Set End of Packet if this is the last part
      if (left == 0) status |= TMD_ENP;
      pcnet32->tx_buffer[entry] = p == q ? p : NULL;
      pcnet32->tx_ring[entry].buffer = (unsigned long)/*virt2phys*/(q->payload);
      pcnet32->tx_ring[entry].length = -len;
      pcnet32->tx_ring[entry].misc = 0x00000000;
      pcnet32->tx_ring[entry].status = status;

      // Move to next entry
      entry = (++entry) & TX_RING_MOD_MASK;
      pcnet32->next_tx = entry;
      pcnet32->size_tx++;

      // This is not first buffer so Start of Packet (STP) is not set
      status = TMD_OWN;
    }

    // Trigger an immediate send poll
    pcnet32->write_csr(pcnet32->iobase, CSR, CSR_IENA | CSR_TDMD);
  }

  return 0;
}

//void pcnet32_receive(struct pcnet32 *pcnet32)
//{
//  int entry = pcnet32->next_rx;
//  struct pbuf *p;
//
//  //kprintf("pcnet32: receive entry %d, 0x%04X\n", entry, pcnet32->rx_ring[entry].status);
//
//  while (!(pcnet32->rx_ring[entry].status & RMD_OWN))
//  {
//    int status = pcnet32->rx_ring[entry].status >> 8;
//
//    //kprintf("receive entry %d, %d\n", entry, status);
//
//    if (status != 0x03)
//    {
//      // Error
//      kprintf("pcnet32: rx error\n");
//      stats.link.err++;
//    }
//    else
//    {
//      short len = (short) (pcnet32->rx_ring[entry].msg_length & 0xfff) - 4;
//      //kprintf("length %d\n", pkt_len);
//      
//      // Get packet
//      p = pcnet32->rx_buffer[entry];
//
//      // Allocate new packet for receive ring
//      pcnet32->rx_buffer[entry] = pbuf_alloc(PBUF_RAW, ETHER_FRAME_LEN, PBUF_RW);
//      if (pcnet32->rx_buffer[entry])
//      {
//	// Give ownership back to card
//	pcnet32->rx_ring[entry].buffer = /*virt2phys*/(pcnet32->rx_buffer[entry]->payload);
//	pcnet32->rx_ring[entry].length = -ETHER_FRAME_LEN; // Note 1
//	pcnet32->rx_ring[entry].status |= RMD_OWN;
//      }
//      else
//      {
//	kprintf(KERN_WARNING "pcnet32: unable to allocate packet for receive ring\n");
//	stats.link.memerr++;
//      }
//
//      if (p)
//      {
//	// Resize packet buffer
//	pbuf_realloc(p, len);
//
//        // Send packet to upper layer
//	if (dev_receive(pcnet32->devno, p) < 0) pbuf_free(p);
//      }
//
//      // Move to next entry
//      entry = (++entry) & RX_RING_MOD_MASK;
//      pcnet32->next_rx = entry;
//    }
//  }
//}
//
//void pcnet32_transmitted(struct pcnet32 *pcnet32)
//{
//  int entry = pcnet32->curr_tx;
//  int freed = 0;
//
//  while (pcnet32->size_tx > 0 && (pcnet32->rx_ring[entry].status & TMD_OWN) != 0)
//  {
//    if (pcnet32->tx_buffer[entry])
//    {
//      pbuf_free(pcnet32->tx_buffer[entry]);
//      pcnet32->tx_buffer[entry] = NULL;
//    }
//    freed++;
//
//    // Move to next entry
//    entry = (++entry) & TX_RING_MOD_MASK;
//    pcnet32->curr_tx = entry;
//    pcnet32->size_tx--;
//  }
//
//  //kprintf("pcnet32: release %d tx entries\n", freed);
//
//  release_sem(&pcnet32->sem_tx, freed);
//}
//
//void pcnet32_dpc(void *arg)
//{
//  struct pcnet32 *pcnet32 = (struct pcnet32 *) arg;
//  unsigned short iobase = pcnet32->iobase;
//  unsigned short csr;
//
//  while ((csr = pcnet32->read_csr(iobase, CSR)) & (CSR_ERR | CSR_RINT | CSR_TINT))
//  {
//    // Acknowledge all of the current interrupt sources
//    pcnet32->write_csr(iobase, CSR, (unsigned short) (csr & ~(CSR_IENA | CSR_TDMD | CSR_STOP | CSR_STRT | CSR_INIT)));
//    //dump_csr(csr);
//
//    if (csr & CSR_RINT) pcnet32_receive(pcnet32);
//    if (csr & CSR_TINT) pcnet32_transmitted(pcnet32);
//  }
//
//  //dump_csr(csr);
//
//  //kprintf("pcnet32: intr (csr0 = %08x)\n", pcnet32->read_csr(pcnet32->iobase, 0));
//  //dump_csr(pcnet32->read_csr(pcnet32->iobase, 0));
//
//  
//  pcnet32->write_csr(iobase, CSR, CSR_BABL | CSR_CERR | CSR_MISS | CSR_MERR | CSR_IDON | CSR_IENA);
//
//  eoi(pcnet32->irq);
//}
//
int pcnet32_handler(struct regs *ctxt, void *arg)
{
  //struct pcnet32 *pcnet32 = (struct pcnet32 *) arg;

  //// Queue DPC to service interrupt
  //queue_irq_dpc(&pcnet32->dpc, pcnet32_dpc, pcnet32);
	printf("pcnet32_handler");
  return 0;
}

int pcnet32_ioctl(struct dev *dev, int cmd, void *args, size_t size)
{
  return 0;//-ENOSYS;
}

int pcnet32_attach(struct dev *dev, struct eth_addr *hwaddr)
{
  struct pcnet32 *pcnet32 = dev->privdata;
  *hwaddr = pcnet32->hwaddr;

  return 0;
}

int pcnet32_detach(struct dev *dev)
{
  return 0;
}

struct driver pcnet32_driver =
{
  "pcnet32",
  DEV_TYPE_PACKET,
  pcnet32_ioctl,
  NULL,
  NULL,
  pcnet32_attach,
  pcnet32_detach,
  pcnet32_transmit
};

int install(struct unit *unit)
{
  struct pcnet32 *pcnet32;
  int version;
  int i;
  char str[20];
  unsigned short val;
  unsigned long init_block;

  kprintf("install");getch();
  // Check for PCI device
  if (unit->bus->bustype != BUSTYPE_PCI) return 0;//-EINVAL;

  // Allocate device structure
  pcnet32 = (struct pcnet32 *) malloc(sizeof(struct pcnet32));
  if (!pcnet32) return 0;//-ENOMEM;
  memset(pcnet32, 0, sizeof(struct pcnet32));
  pcnet32->phys_addr = (unsigned long)/*virt2phys*/(pcnet32);
  kprintf("install phys_addr:%8X",pcnet32->phys_addr);getch();
  // Setup NIC configuration
  pcnet32->iobase = (unsigned short) get_unit_iobase(unit);
  kprintf("install iobase:%d",pcnet32->iobase);getch();
  pcnet32->irq = (unsigned short) get_unit_irq(unit);
  kprintf("install irq:%d",pcnet32->irq);getch();
  pcnet32->membase = (unsigned short) get_unit_membase(unit);
  kprintf("install membase:%d",pcnet32->membase);getch();
  
  // Enable bus mastering
  pci_enable_busmastering(unit);
   
  // Reset the chip
  pcnet32_dwio_reset(pcnet32->iobase);
  pcnet32_wio_reset(pcnet32->iobase);

  
  // Setup access functions
  if (pcnet32_wio_read_csr(pcnet32->iobase, 0) == 4 && pcnet32_wio_check(pcnet32->iobase))
  {
	  kprintf("install IF");getch();
    pcnet32->read_csr = pcnet32_wio.read_csr;
    pcnet32->write_csr = pcnet32_wio.write_csr;
    pcnet32->read_bcr = pcnet32_wio.read_bcr;
    pcnet32->write_bcr = pcnet32_wio.write_bcr;
    pcnet32->read_rap = pcnet32_wio.read_rap;
    pcnet32->write_rap = pcnet32_wio.write_rap;
    pcnet32->reset = pcnet32_wio.reset;
  }
  else
  {
	  kprintf("install Not IF");getch();
    if (pcnet32_dwio_read_csr(pcnet32->iobase, 0) == 4 && pcnet32_dwio_check(pcnet32->iobase))
    {
      pcnet32->read_csr = pcnet32_dwio.read_csr;
      pcnet32->write_csr = pcnet32_dwio.write_csr;
      pcnet32->read_bcr = pcnet32_dwio.read_bcr;
      pcnet32->write_bcr = pcnet32_dwio.write_bcr;
      pcnet32->read_rap = pcnet32_dwio.read_rap;
      pcnet32->write_rap = pcnet32_dwio.write_rap;
      pcnet32->reset = pcnet32_dwio.reset;
    }
    else
      return 0;//-EIO;
  }

  version = pcnet32->read_csr(pcnet32->iobase, CHIP_ID_LOWER) | (pcnet32_wio_read_csr(pcnet32->iobase, CHIP_ID_UPPER) << 16);
  
  if ((version & 0xfff) != 0x003) return 0;
  version = (version >> 12) & 0xffff;
  kprintf("PCnet chip version is %#x.\n", version);

  switch (version)
  {
    case 0x2420:
      unit->vendorname = "AMD";
      unit->productname = "AMD PCI 79C970";
      break;

    case 0x2430:
      unit->vendorname = "AMD";
      unit->productname = "AMD PCI 79C970";
      break;

    case 0x2621:		
      unit->vendorname = "AMD";
      unit->productname = "AMD PCI II 79C970A";
	  kprintf("%s:%s",unit->vendorname,unit->productname, version);getch();
      //fdx = 1;
      break;

    case 0x2623:
      unit->vendorname = "AMD";
      unit->productname = "AMD FAST 79C971";
      //fdx = 1; mii = 1; fset = 1;
      //ltint = 1;
      break;

    case 0x2624:
      unit->vendorname = "AMD";
      unit->productname = "AMD FAST+ 79C972";
      //fdx = 1; mii = 1; fset = 1;
      break;

    case 0x2625:
      unit->vendorname = "AMD";
      unit->productname = "AMD FAST III 79C973";
      //fdx = 1; mii = 1;
      break;

    case 0x2626:
      unit->vendorname = "AMD";
      unit->productname = "AMD Home 79C978";
      //fdx = 1;
      break;

    case 0x2627:
      unit->vendorname = "AMD";
      unit->productname = "AMD FAST III 79C975";
      //fdx = 1; mii = 1;
      break;

    default:
      kprintf("pcnet32: PCnet version %#x, no PCnet32 chip.\n", version);
      return 0;
  }

  // Install interrupt handler
  //init_dpc(&pcnet32->dpc);
  //register_interrupt(&pcnet32->intr, IRQ2INTR(pcnet32->irq), pcnet32_handler, pcnet32);
  irq_install_handler(IRQ2INTR(pcnet32->irq),(void*) pcnet32_handler);//automatic Enable IRQ
  kprintf("ssirq_install_handler");getch();

  //enable_irq(pcnet32->irq);

  // Read MAC address from PROM
  for (i = 0; i < ETHER_ADDR_LEN; i++)
  {
	  pcnet32->hwaddr.addr[i] = (unsigned char) inp8((unsigned short) (pcnet32->iobase + i));
	  //kprintf("Read MAC %d:%d",i,pcnet32->hwaddr.addr[i]);getch();
  }
  

  // Setup the init block
  //pcnet32->init_block.mode = 0x0003;
  //pcnet32->init_block.mode = 0x8000;
  //pcnet32->init_block.mode = 0x0000;
  pcnet32->init_block.mode = 0x0080; // ASEL
  pcnet32->init_block.tlen_rlen = TX_RING_LEN_BITS | RX_RING_LEN_BITS;
  for (i = 0; i < 6; i++) pcnet32->init_block.phys_addr[i] = pcnet32->hwaddr.addr[i];
  pcnet32->init_block.filter[0] = 0x00000000;
  pcnet32->init_block.filter[1] = 0x00000000;
  pcnet32->init_block.rx_ring = pcnet32->phys_addr + offsetof(struct pcnet32, rx_ring);
  pcnet32->init_block.tx_ring = pcnet32->phys_addr + offsetof(struct pcnet32, tx_ring);

  //kprintf("init_block %8X:%8X",pcnet32->init_block.rx_ring ,pcnet32->init_block.tx_ring);getch();
  // Allocate receive ring
  for (i = 0; i < RX_RING_SIZE; i++)
  {
    pcnet32->rx_buffer[i] = (struct pbuf *)malloc(ETHER_FRAME_LEN) ;//pbuf_alloc(PBUF_RAW, ETHER_FRAME_LEN, PBUF_RW);
    pcnet32->rx_ring[i].buffer =(unsigned long) /*virt2phys*/(pcnet32->rx_buffer[i]->payload);
    pcnet32->rx_ring[i].length = (short) -ETHER_FRAME_LEN;
    pcnet32->rx_ring[i].status = RMD_OWN;
  }
  pcnet32->next_rx = 0;

  // Initialize transmit ring
  for (i = 0; i < TX_RING_SIZE; i++)
  {
    pcnet32->tx_buffer[i] = NULL;
    pcnet32->tx_ring[i].buffer = 0;
    pcnet32->tx_ring[i].status = 0;
  }
  //kprintf("transmit");getch();
  //init_sem(&pcnet32->sem_tx, TX_RING_SIZE);
  pcnet32->size_tx = 0;
  pcnet32->curr_tx = 0;
  pcnet32->next_tx = 0;

  // Reset pcnet32
  pcnet32->reset(pcnet32->iobase);
  //kprintf("reset");getch();
  /// Switch pcnet32 to 32bit mode
  pcnet32->write_bcr(pcnet32->iobase, 20, 2);

  //kprintf("Switch pcnet32 to 32bit mode");getch();
  // Set autoselect bit
  //val = pcnet32->read_bcr(pcnet32->iobase, MISCCFG) & ~MISCCFG_ASEL;
  //val |= MISCCFG_ASEL;
  //pcnet32->write_bcr(pcnet32->iobase, MISCCFG, val);

  // Set full duplex
  val = pcnet32->read_bcr(pcnet32->iobase, 9) & ~3;
  val |= 1;
  pcnet32->write_bcr(pcnet32->iobase, 9, val);
	
  //kprintf("Set full duplex");getch();

  init_block = pcnet32->phys_addr + offsetof(struct pcnet32, init_block);
  pcnet32->write_csr(pcnet32->iobase, 1, (unsigned short) (init_block & 0xffff));
  pcnet32->write_csr(pcnet32->iobase, 2, (unsigned short) (init_block >> 16));

  //kprintf("pcnet32->write_csr%8X",pcnet32->write_csr);getch();

  pcnet32->write_csr(pcnet32->iobase, 4, 0x0915);
  pcnet32->write_csr(pcnet32->iobase, 0, CSR_INIT);

  //kprintf("write_csr");getch();
  i = 0;
  while (i++ < 100)
  {
    if (pcnet32->read_csr(pcnet32->iobase, 0) & CSR_IDON) break;
  }
  //kprintf("0..100");getch();

  //kprintf("pcnet32->iobase=%d pcnet32->write_csr=%8X",pcnet32->iobase,pcnet32->write_csr);
  pcnet32->write_csr(pcnet32->iobase, 0, CSR_IENA | CSR_STRT); // note 2

  //kprintf("write_csr");getch();

  //pcnet32->devno = dev_make("eth#", &pcnet32_driver, unit, pcnet32);

  kprintf("%s: %s iobase 0x%x irq %d hwaddr %s\n", device(pcnet32->devno)->name, unit->productname, pcnet32->iobase, pcnet32->irq, ether2str(&pcnet32->hwaddr, str));

  return 0;
}

//int __stdcall start(hmodule_t hmod, int reason, void *reserved2)
//{
//  return 1;
//}



