#include <dev.h>
#include <pci.h>
#include <pnp.h>
#include <pcnet32.h>


///////////////////////////////////////pbuf.h////////////////////////////////////
// pbuf.c
//
// Packet buffer management
//
// Copyright (C) 2002 Michael Ringgaard. All rights reserved.
// Portions Copyright (C) 2001, Swedish Institute of Computer Science.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
// 
// 1. Redistributions of source code must retain the above copyright 
//    notice, this list of conditions and the following disclaimer.  
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.  
// 3. Neither the name of the project nor the names of its contributors
//    may be used to endorse or promote products derived from this software
//    without specific prior written permission. 
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
// OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
// HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
// LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
// OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF 
// SUCH DAMAGE.
// 

 struct pbuf *pbuf_pool = NULL;
 struct pbuf *pbuf_pool_alloc_cache = NULL;
 struct pbuf *pbuf_pool_free_cache = NULL;

 int pbuf_pool_free_lock, pbuf_pool_alloc_lock;

//
// pbuf_init
//
// Initializes the pbuf module. A large part of memory is allocated
// for holding the pool of pbufs. The size of the individual pbufs in
// the pool is given by the size parameter, and the number of pbufs in
// the pool by the num parameter.
//
// After the memory has been allocated, the pbufs are set up. The
// next pointer in each pbuf is set up to point to the next pbuf in
// the pool.
//

void pbuf_init()
{
  struct pbuf *p, *q;
  int i;

  // Allocate buffer pool
  pbuf_pool = (struct pbuf *) malloc(PBUF_POOL_SIZE * (PBUF_POOL_BUFSIZE + sizeof(struct pbuf)));
  ///stats.pbuf.avail = PBUF_POOL_SIZE;
  
  // Set up next pointers to link the pbufs of the pool together
  p = pbuf_pool;
  
  for (i = 0; i < PBUF_POOL_SIZE; i++)
  {
    p->next = (struct pbuf *) ((char *) p + PBUF_POOL_BUFSIZE + sizeof(struct pbuf));
    p->len = p->tot_len = p->size = PBUF_POOL_BUFSIZE;
    p->payload = (void *) ((char *) p + sizeof(struct pbuf));
    q = p;
    p = p->next;
  }
  
  // The next pointer of last pbuf is NULL to indicate that there are no more pbufs in the pool
  q->next = NULL;

  pbuf_pool_alloc_lock = 0;
  pbuf_pool_free_lock = 0;
}

//static struct pbuf *pbuf_pool_alloc()
//{
//  struct pbuf *p = NULL;
//
//  // First, see if there are pbufs in the cache
//  if (pbuf_pool_alloc_cache) 
//  {
//    p = pbuf_pool_alloc_cache;
//    if (p) pbuf_pool_alloc_cache = p->next; 
//  } 
//  else 
//  {
//    // Next, check the actual pbuf pool, but if the pool is locked, we
//    // pretend to be out of buffers and return NULL
//    if (pbuf_pool_free_lock) 
//    {
//      stats.pbuf.alloc_locked++;
//      return NULL;
//    }
//    
//    pbuf_pool_alloc_lock++;
//    if (!pbuf_pool_free_lock)
//    {
//      p = pbuf_pool;
//      if(p) pbuf_pool = p->next; 
//    } 
//    else
//      stats.pbuf.alloc_locked++;
//
//    pbuf_pool_alloc_lock--;
//  }
//
//  if (p != NULL) 
//  {
//    stats.pbuf.used++;
//    if (stats.pbuf.used > stats.pbuf.max) stats.pbuf.max = stats.pbuf.used;
//  }
//
//  return p;   
//}

/////////////////////////////////////////////////////////////////////////////////
//
// ether2str
//
// This function converts an ethernet address to string format.
//

char *ether2str(struct eth_addr *hwaddr, char *s)
{
  sprintf(s, "%02x:%02x:%02x:%02x:%02x:%02x",  
          hwaddr->addr[0], hwaddr->addr[1], hwaddr->addr[2], 
	  hwaddr->addr[3], hwaddr->addr[4], hwaddr->addr[5]);
  return s;
}

/////////////////////////////////////////////inifile/////////////////////////////

static char *trimstr(char *s, char *end)
{
  char *str;
  char *t;
  int ch;
  int i;
  
  while (end > s)
  {
    if (*(end - 1) == ' ' || *(end - 1) == '\t') 
      end--;
    else
      break;
  }
  if (end == s) return NULL;

  t = str = (char *) malloc(end - s + 1);
  while (s < end)
  {
    if (*s == '^')
    {
      s++;
      ch = 0;
      for (i = 0; i < 2; i++)
      {
        if (s == end) break;

	if (*s >= '0' && *s <= '9')
	  ch = (ch << 4) + *s - '0';
	else if (*s >= 'A' && *s <= 'F')
	  ch = (ch << 4) + *s + 10 - 'A';
	else if (*s >= 'a' && *s <= 'f')
	  ch = (ch << 4) + *s + 10 - 'a';
	else
          break;

	s++;
      }
      *t++ = ch;
    }
    else
      *t++ = *s++;
  }

  *t = 0;
  return str;
}

struct section *find_section(struct section *sect, char *name)
{
  while (sect)
  {
    if (Strcmp(sect->name, name) == 0) return sect;
    sect = sect->next;
  }

  return NULL;
}

int get_section_size(struct section *sect)
{
  struct property *prop;
  int n;
  
  if (!sect) return 0;
  prop = sect->properties;

  n = 0;
  while (prop)
  {
    n++;
    prop = prop->next;
  }

  return n;
}

char *find_property(struct section *sect, char *name)
{
  struct property *prop;
  
  if (!sect) return NULL;
  prop = sect->properties;

  while (prop)
  {
    if (Strcmp(prop->name, name) == 0) return prop->value ? prop->value : "";
    prop = prop->next;
  }

  return NULL;
}


char *get_property(struct section *sections, char *sectname, char *propname, char *defval)
{
  struct section *sect;
  char *val;

  sect = find_section(sections, sectname);
  if (!sect) return defval;

  val = find_property(sect, propname);
  return val ? val : defval;
}

int get_numeric_property(struct section *sections, char *sectname, char *propname, int defval)
{
  char *val;

  val = get_property(sections, sectname, propname, NULL);
  return val ? atoi(val) : defval;
}

void free_properties(struct section *sect)
{
  struct section *nextsect;
  struct property *prop;
  struct property *nextprop;

  while (sect)
  {
    if (sect->name) free(sect->name);
    
    prop = sect->properties;
    while (prop)
    {
      if (prop->name) free(prop->name);
      if (prop->value) free(prop->value);
      
      nextprop = prop->next;
      free(prop);
      prop = nextprop;
    }

    nextsect = sect->next;
    free(sect);
    sect = nextsect;
  }
}

struct section *parse_properties(char *props)
{
  struct section *secthead = NULL;
  struct section *sect = NULL;
  struct property *prop = NULL;
  char *p;
  char *end;
  char *split;

  p = props;
  while (*p)
  {
    // Skip white at start of line
    while (*p == ' ' || *p == '\t') p++;
    
    // Skip comments
    if (*p == '#' || *p == ';')
    {
      while (*p && *p != '\r' && *p != '\n') p++;
      if (*p == '\r') p++;
      if (*p == '\n') p++;
      continue;
    }

    // Skip blank lines
    if (*p == 0 || *p == '\r' || *p == '\n')
    {
      if (*p == '\r') p++;
      if (*p == '\n') p++;
      continue;
    }

    // Check for section or property
    if (*p == '[')
    {
      struct section *newsect;

      p++;
      end = p;
      while (*end && *end != ']') end++;

      newsect = (struct section *) malloc(sizeof(struct section));
      if (!newsect) return NULL;

      newsect->name = trimstr(p, end);
      newsect->next = NULL;
      newsect->properties = NULL;
      if (!secthead) secthead = newsect;
      if (sect) sect->next = newsect;
      sect = newsect;
      prop = NULL;

      p = end;
      if (*p == ']') p++;
    }
    else
    {
      struct property *newprop;

      end = p;
      split = NULL;
      while (*end && *end != '\r' && *end != '\n') 
      {
	if (!split && (*end == '=' || *end == ':')) split = end;
	end++;
      }

      if (sect)
      {
	newprop = (struct property *) malloc(sizeof(struct property));
	if (!newprop) return NULL;

	if (split)
	{
	  newprop->name = trimstr(p, split);
	  split++;
	  while (*split == ' ' || *split == '\t') split++;
	  newprop->value = trimstr(split, end);
	}
	else
	{
	  newprop->name = trimstr(p, end);
	  newprop->value = NULL;
	}

	newprop->next = NULL;
	if (prop)
	  prop->next = newprop;
	else
	  sect->properties = newprop;

	prop = newprop;
      }

      p = end;
      if (*p == '\r') p++;
      if (*p == '\n') p++;
    }
  }

  return secthead;
}

void list_properties(FILE* f, struct section *sect)
{
  struct property *prop;

  while (sect)
  {
    fwrite("[" ,1 ,1,f );
    fwrite(sect->name, strlen(sect->name),1,f);
    fwrite("]\r\n", 3,1,f);

    prop = sect->properties;
    while (prop)
    {
      fwrite(prop->name, strlen(prop->name),1,f);
      if (prop->value)
      {
		fwrite("=", 1,1,f);
        fwrite(prop->value, strlen(prop->value),1,f);
      }
      fwrite("\r\n", 2,1,f);
      prop = prop->next;
    }
    
    if (sect->next) fwrite("\r\n", 2,1,f);
    sect = sect->next;
  }
}

struct section *read_properties(char *filename)
{
  //int f;
  //int size;
  //struct stat64 buffer;
  //char *props;
  //struct section *sect;

  //f = fopen(filename, FOpenModeRDWRBinary);
  //if (f < 0) return NULL;

  ////fstat64(f, &buffer);
  //size = (int) buffer.st_size;

  //props = (char *) malloc(size + 1);
  //if (!props)
  //{
  //  close(f);
  //  return NULL;
  //}

  //if (read(f, props, size) != size)
  //{
  //  free(props);
  //  close(f);
  //  return NULL;
  //}

  //close(f);

  //props[size] = 0;

  //sect = parse_properties(props);
  //free(props);

  //return sect;
	return NULL;
}

/////////////////////////////////////////////////////////////////////////////////

struct unit *units;
struct bus *buses;

struct binding *bindtab;
int num_bindings;

struct dev *devtab[MAX_DEVS];
unsigned int num_devs = 0;

//static int units_proc(struct proc_file *pf, void *arg);
//static int devices_proc(struct proc_file *pf, void *arg);

char *busnames[] = {"HOST", "PCI", "ISA", "?", "?"};
char *devtypenames[] = {"?", "stream", "block", "packet"};



struct bus *add_bus(struct unit *self, unsigned long bustype, unsigned long busno)
{
  struct bus *bus;
  struct bus *b;

  // Create new bus
  bus = (struct bus *) malloc(sizeof(struct bus));
  if (!bus) return NULL;
  memset(bus, 0, sizeof(struct bus));

  bus->self = self;
  if (self) bus->parent = self->bus;
  bus->bustype = bustype;
  bus->busno = busno;

  // Add bus as a bridge on the parent bus
  if (bus->parent)
  {
    if (bus->parent->bridges)
    {
      b = bus->parent->bridges;
      while (b->sibling) b = b->sibling;
      b->sibling = bus;
    }
    else
      bus->parent->bridges = bus;
  }

  // Add bus to bus list
  if (buses)
  {
    b = buses;
    while (b->next) b = b->next;
    b->next = bus;
  }
  else
    buses = bus;

  return bus;
}

struct unit *add_unit(struct bus *bus, unsigned long classcode, unsigned long unitcode, unsigned long unitno)
{
  struct unit *unit;
  struct unit *u;

  // Create new unit
  unit = (struct unit *) malloc(sizeof(struct unit));
  if (!unit) return NULL;
  memset(unit, 0, sizeof(struct unit));

  unit->bus = bus;
  unit->classcode = classcode;
  unit->unitcode = unitcode;
  unit->unitno = unitno;

  unit->classname = "";
  unit->vendorname = "";
  unit->productname = "";

  // Add unit to bus
  if (bus)
  {
    if (bus->units)
    {
      u = bus->units;
      while (u->sibling) u = u->sibling;
      u->sibling = unit;
    }
    else
      bus->units = unit;
  }

  // Add unit to unit list
  if (units)
  {
    u = units;
    while (u->next) u = u->next;
    u->next = unit;
  }
  else
    units = unit;

  return unit;
}

struct resource *add_resource(struct unit *unit, unsigned short type, unsigned short flags, unsigned long start, unsigned long len)
{
  struct resource *res;
  struct resource *r;

  // Create new resource
  res = (struct resource *) malloc(sizeof(struct resource));
  if (!res) return NULL;
  memset(res, 0, sizeof(struct resource));

  res->type = type;
  res->flags = flags;
  res->start = start;
  res->len = len;

  // Add resource to unit resource list
  if (unit->resources)
  {
    r = unit->resources;
    while (r->next) r = r->next;
    r->next = res;
  }
  else
    unit->resources = res;

  return res;
}

struct resource *get_unit_resource(struct unit *unit, int type, int num)
{
  struct resource *res = unit->resources;

  while (res)
  {
    if (res->type == type)
    {
      if (num == 0)
	return res;
      else
	num--;
    }

    res = res->next;
  }

  return NULL;
}

int get_unit_irq(struct unit *unit)
{
  struct resource *res = get_unit_resource(unit, RESOURCE_IRQ, 0);
  
  if (res) return res->start;
  return -1;
}

int get_unit_iobase(struct unit *unit)
{
  struct resource *res = get_unit_resource(unit, RESOURCE_IO, 0);
  
  if (res) return res->start;
  return -1;
}

void *get_unit_membase(struct unit *unit)
{
  struct resource *res = get_unit_resource(unit, RESOURCE_MEM, 0);
  
  if (res) return (void *) (res->start);
  return NULL;
}

char *get_unit_name(struct unit *unit)
{
  if (unit->productname && *unit->productname) return unit->productname;
  if (unit->classname && *unit->classname) return unit->classname;
  return "unknown";
}

struct unit *lookup_unit(struct unit *start, unsigned long unitcode, unsigned long unitmask)
{
  struct unit *unit;
  
  if (start)
    unit = start->next;
  else
    unit = units;

  while (unit)
  {
    if ((unit->unitcode & unitmask) == unitcode) return unit;
    unit = unit->next;
  }

  return NULL;
}

struct unit *lookup_unit_by_class(struct unit *start, unsigned long classcode, unsigned long classmask)
{
  struct unit *unit;
  
  if (start)
    unit = start->next;
  else
    unit = units;

  while (unit)
  {
    if ((unit->classcode & classmask) == classcode) return unit;
    unit = unit->next;
  }

  return NULL;
}

struct board *lookup_board(struct board *board_tbl, struct unit *unit)
{
  int i = 0;

  while (board_tbl[i].vendorname != NULL)
  {
    if (unit->bus->bustype == board_tbl[i].bustype &&
        (unit->unitcode & board_tbl[i].unitmask) == board_tbl[i].unitcode &&
        (unit->subunitcode & board_tbl[i].subsystemmask) == board_tbl[i].subsystemcode &&
        (unit->revision & board_tbl[i].revisionmask) == board_tbl[i].revisioncode)
      break;

    i++;
  }

  if (board_tbl[i].vendorname == NULL) return NULL;
  return &board_tbl[i];
}

void enum_host_bus()
{
  struct bus *host_bus;
  struct unit *pci_host_bridge;
  unsigned long unitcode;
  struct bus *pci_root_bus;
  struct unit *isa_bridge;
  struct bus *isa_bus;

  // Create host bus
  host_bus = add_bus(NULL, BUSTYPE_HOST, 0);

  unitcode = get_pci_hostbus_unitcode();
  kprintf("SSS:enum_host_busunitcode=%d",unitcode);
  if (unitcode)
  {
    // Enumerate PCI buses
    pci_host_bridge = add_unit(host_bus, PCI_HOST_BRIDGE, unitcode, 0);
    pci_root_bus = add_bus(pci_host_bridge, BUSTYPE_PCI, 0);

    enum_pci_bus(pci_root_bus);
  }
  else
  {
    // Enumerate ISA bus using PnP
    isa_bridge = add_unit(host_bus, PCI_ISA_BRIDGE, 0, 0);
    isa_bus = add_bus(isa_bridge, BUSTYPE_ISA, 0);

    enum_isapnp(isa_bus);
  }   
}



static void parse_bindings()
{
  struct section *sect;
  struct property *prop;
  struct binding *bind;
  char buf[128];
  char *p;
  char *q;
  int n;

  // Parse driver bindings
  kprintf("\nkrnlcfg->name=%s",krnlcfg->name ); getch();
  sect = find_section(krnlcfg, "bindings");
  if (!sect) return;
  num_bindings = get_section_size(sect);
  if (!num_bindings) return;

  bindtab = (struct binding *) malloc(num_bindings * sizeof(struct binding));
  memset(bindtab, 0, num_bindings * sizeof(struct binding));

  n = 0;
  prop = sect->properties;
  while (prop)
  {
    bind = &bindtab[n];

    Strcpy(buf, prop->name);
	kprintf("name=%s value=%s  ",prop->name ,prop->value);
	getch();
    
    p = q = buf;
    while (*q && *q != ' ') q++;
    if (!*q) continue;
    *q++ = 0;

    if (Strcmp(p, "pci") == 0)
      bind->bustype = BUSTYPE_PCI;
    else if (Strcmp(p, "isa") == 0)
      bind->bustype = BUSTYPE_ISA;

    while (*q == ' ') q++;
    p = q;
    while (*q && *q != ' ') q++;
    if (!*q) continue;
    *q++ = 0;

    if (Strcmp(p, "class") == 0)
      bind->bindtype = BIND_BY_CLASSCODE;
    else if (Strcmp(p, "unit") == 0)
      bind->bindtype = BIND_BY_UNITCODE;

    while (*q == ' ') q++;

    while (*q)
    {
      unsigned long digit;
      unsigned long mask;

      mask = 0xF;
      if (*q >= '0' && *q <= '9')
	digit = *q - '0';
      else if (*q >= 'A' && *q <= 'F')
	digit = *q - 'A' + 10;
      else if (*q >= 'a' && *q <= 'f')
	digit = *q - 'a' + 10;
      else
      {
	digit = 0;
	mask = 0;
      }

      bind->code = (bind->code << 4) | digit;
      bind->mask = (bind->mask << 4) | mask;
      q++;
    }

    bind->module = prop->value;

    //kprintf("binding %c %08X %08X %s\n", bind->type, bind->code, bind->mask, bind->module);
    prop = prop->next;
    n++;
  }
  
}

static struct binding *find_binding(struct unit *unit)
{
  int n;

  for (n = 0; n < num_bindings; n++)
  {
    struct binding *bind = &bindtab[n];

    if (bind->bustype == unit->bus->bustype)
    {
      if (bind->bindtype == BIND_BY_CLASSCODE)
      {
	if ((unit->classcode & bind->mask) == bind->code) return bind;
      }

      if (bind->bindtype == BIND_BY_UNITCODE)
      {
	if ((unit->unitcode & bind->mask) == bind->code) return bind;
      }
    }
  }

  return NULL;
}

extern int install(struct unit *unit);
static int initialize_driver(struct unit *unit, char *driverstr)
{
	kprintf("\ninitialize_driver:%s:%s\n",unit->classname,unit->productname);getch();
	install(unit);
 // char *buf;
 // char *p;
 // char *modname;
 // char *entryname;
 // char *opts;
 // hmodule_t hmod;
 // int rc;
 // int (*entry)(struct unit *unit, char *opts);

 // 
 // p = buf =(char *)malloc(strlen(driverstr) + 1);
 // if (!buf) return 0;//-ENOMEM;
 // memcpy(buf, driverstr, strlen(driverstr) + 1);

 // modname = p;
 // entryname = strchr(p, '!');
 // if (entryname)
 // {
 //   *entryname++ = 0;
 //   p = entryname;
 // }
 // else 
 //   entryname = "install";

 // opts = strchr(p, ':');
 // if (opts)
 //   *opts++ = 0;
 // else 
 //   opts = NULL;

 // //hmod = load(modname, 0);
 // if (!hmod)
 // {
 //   kprintf("dev: unable to load module %s\n", modname);
 //   free(buf,strlen(driverstr) + 1);
 //   return 0;//-ENOEXEC;
 // }

 // //entry = resolve(hmod, entryname);

 // if (!entry)
 // {
 //   kprintf("dev: unable to find entry %s in module %s\n", entryname, modname);
 //   //unload(hmod);
 //   free(buf,strlen(driverstr) + 1);
 //   return 0;//-ENOEXEC;
 // }
 // kprintf("\ninitialize_driver:%s:%s\n",unit->classname,unit->productname);getch();
	//install(unit, opts);
 // //rc = entry(unit, opts);
 // if (rc < 0)
 // {
 //   kprintf("dev: initialization of %s!%s failed with error %d\n", modname, entryname, rc);
 //   //unload(hmod);
 //   free(buf,strlen(driverstr) + 1);
 //   return rc;
 // }

 // free(buf,strlen(driverstr) + 1);
  return 0;
}

static void install_driver(struct unit *unit, struct binding *bind)
{
  int rc;

  rc = initialize_driver(unit, bind->module);
  if (rc < 0)
  {
    kprintf("dev: driver '%s' failed with error %d for unit %08X '%s'\n", bind->module, rc, unit->unitcode, get_unit_name(unit));
  }
}

static void bind_units()
{
  struct unit *unit = units;
	kprintf("units=%s",units->classname);getch();
  while (unit)
  {
    struct binding *bind = find_binding(unit);
    if (bind)
	{
		kprintf("module=%s unit=%s",bind->module ,unit->productname);getch();
		install_driver(unit, bind);
	}
    unit = unit->next;
  }
  kprintf("Finish bind_units");getch();
}

static void install_legacy_drivers()
{
  struct section *sect;
  struct property *prop;
  int buflen;
  char *buf;
  int rc;

  sect = find_section(krnlcfg, "drivers");
  if (!sect) return;

  prop = sect->properties;
  while (prop)
  {
    buflen = strlen(prop->name) + 1;
    if (prop->value) buflen += strlen(prop->value) + 1;
    buf = (char *)malloc(buflen);
    
    if (buf)
    {
      Strcpy(buf, prop->name);
      if (prop->value)
      {
	strcat(buf, ":");
	strcat(buf, prop->value);
      }

      rc = initialize_driver(NULL, buf);
      if (rc < 0)
      {
	kprintf("dev: error %d initializing driver %s\n", rc, prop->name);
      }

      free(buf);
    }

    prop = prop->next;
  }
}


void install_drivers()
{
  // Register /proc/units
 // register_proc_inode("units", units_proc, NULL);
 // register_proc_inode("devices", devices_proc, NULL);

  // Parse driver binding database
  parse_bindings();
	kprintf("Finish Binding");getch();
  // Match bindings to units
  bind_units();

  // Install legacy drivers
  install_legacy_drivers();
}

struct dev *device(dev_t devno)
{
  if (devno < 0 || devno >= num_devs) return NULL;
  return devtab[devno];
}

dev_t dev_make(char *name, struct driver *driver, struct unit *unit, void *privdata)
{
  struct dev *dev;
  dev_t devno;
  char *p;
  unsigned int n, m;
  int exists;

  if (num_devs == MAX_DEVS) printf("too many devices");

  dev = (struct dev *) malloc(sizeof(struct dev));
  if (!dev) return 0;//NODEV;
  memset(dev, 0, sizeof(struct dev));

  Strcpy(dev->name, name);
  
  p = dev->name;
  while (p[0] && p[1]) p++;
  if (*p == '#')
  {
    n = 0;
    while (1)
    {
      sprintf(p, "%d", n);
      exists = 0;
      for (m = 0; m < num_devs; m++) 
      {
	if (Strcmp(devtab[m]->name, dev->name) == 0) 
	{
	  exists = 1;
	  break;
	}
      }

      if (!exists) break;
      n++;
    }
  }

  dev->driver = driver;
  dev->unit = unit;
  dev->privdata = privdata;
  dev->refcnt = 0;
  dev->mode = 0600;

  switch (dev->driver->type)
  {
    case DEV_TYPE_STREAM: dev->mode |= S_IFCHR; break;
    case DEV_TYPE_BLOCK: dev->mode |= S_IFBLK; break;
    case DEV_TYPE_PACKET: dev->mode |= S_IFPKT; break;
  }

  if (unit) unit->dev = dev;

  devno = num_devs++;
  devtab[devno] = dev;
  
  return devno;
}

dev_t dev_open(char *name)
{
  dev_t devno;

  for (devno = 0; devno < num_devs; devno++)
  {
    if (Strcmp(devtab[devno]->name, name) == 0)
    {
      devtab[devno]->refcnt++;
      return devno;
    }
  }

  return NODEV;
}

int dev_close(dev_t devno)
{
  if (devno < 0 || devno >= num_devs) return 0;//-ENODEV;
  if (devtab[devno]->refcnt == 0) return 0;//-EPERM;
  devtab[devno]->refcnt--;
  return 0;
}


int dev_ioctl(dev_t devno, int cmd, void *args, size_t size)
{
  struct dev *dev;

  if (devno < 0 || devno >= num_devs) return 0;//-ENODEV;
  dev = devtab[devno];
  if (!dev->driver->ioctl) return 0;//-ENOSYS;

  return dev->driver->ioctl(dev, cmd, args, size);
}

int dev_read(dev_t devno, void *buffer, size_t count, blkno_t blkno, int flags)
{
  struct dev *dev;

  if (devno < 0 || devno >= num_devs) return 0;//-ENODEV;
  dev = devtab[devno];
  if (!dev->driver->read) return 0;//-ENOSYS;

  return dev->driver->read(dev, buffer, count, blkno, flags);
}

int dev_write(dev_t devno, void *buffer, size_t count, blkno_t blkno, int flags)
{
  struct dev *dev;

  if (devno < 0 || devno >= num_devs) return 0;//-ENODEV;
  dev = devtab[devno];
  if (!dev->driver->read) return 0;//-ENOSYS;

  return dev->driver->write(dev, buffer, count, blkno, flags);
}


int dev_attach(dev_t devno, struct netif *netif, int (*receive)(struct netif *netif, struct pbuf *p))
{
  struct dev *dev;

  if (devno < 0 || devno >= num_devs) return 0;//-ENODEV;
  dev = devtab[devno];
  if (!dev->driver->attach) return 0;//-ENOSYS;

  dev->netif = netif;
  dev->receive = receive;
  return dev->driver->attach(dev, &netif->hwaddr);
}

int dev_detach(dev_t devno)
{
  struct dev *dev;
  int rc;

  if (devno < 0 || devno >= num_devs) return 0;//-ENODEV;
  dev = devtab[devno];

  if (dev->driver->detach) 
    rc = dev->driver->detach(dev);
  else
    rc = 0;

  dev->netif = NULL;
  dev->receive = NULL;

  return rc;
}

int dev_transmit(dev_t devno, struct pbuf *p)
{
  struct dev *dev;

  if (devno < 0 || devno >= num_devs) return 0;//-ENODEV;
  dev = devtab[devno];
  if (!dev->driver->transmit) return 0;//-ENOSYS;

  return dev->driver->transmit(dev, p);
}

int dev_receive(dev_t devno, struct pbuf *p)
{
  struct dev *dev;

  if (devno < 0 || devno >= num_devs) return 0;//-ENODEV;
  dev = devtab[devno];
  if (!dev->receive) return 0;//-ENOSYS;

  return dev->receive(dev->netif, p);
}

int dev_setevt(dev_t devno, int events)
{
  struct dev *dev;

  if (devno < 0 || devno >= num_devs) return 0;//-ENODEV;
  dev = devtab[devno];
  //devfs_setevt(dev, events);
  return 0;
}

int dev_clrevt(dev_t devno, int events)
{
  struct dev *dev;

  if (devno < 0 || devno >= num_devs) return 0;//-ENODEV;
  dev = devtab[devno];
  //devfs_clrevt(dev, events);
  return 0;
}

int units_proc(/*struct proc_file *pf,*/ void *arg)
{
  struct unit *unit;
  struct resource *res;
  int bustype;
  int busno;

  unit = units;
  while (unit)
  {
    if (unit->bus)
    {
      bustype = unit->bus->bustype;
      busno = unit->bus->busno;
    }
    else
    {
      bustype = BUSTYPE_HOST;
      busno = 0;
    }
    
    printf("%s unit %d.%d class %08X code %08X %s:\n", busnames[bustype], busno, unit->unitno,unit->classcode, unit->unitcode, get_unit_name(unit));

    if (unit->subunitcode != 0 || unit ->revision != 0)
    {
      printf("  subunitcode: %08X revision %d\n", unit->subunitcode, unit->revision);
    }

    res = unit->resources;
    while (res)
    {
      switch (res->type)
      {
	case RESOURCE_IO: 
	  if (res->len == 1) 
	    printf("  io: 0x%03x", res->start);
	  else
	    printf("  io: 0x%03x-0x%03x", res->start, res->start + res->len - 1);
	  break;

	case RESOURCE_MEM:
	  if (res->len == 1) 
	    printf("  mem: 0x%08x", res->start);
	  else
	    printf("  mem: 0x%08x-0x%08x", res->start, res->start + res->len - 1);
	  break;

	case RESOURCE_IRQ:
	  if (res->len == 1) 
	    printf("  irq: %d", res->start);
	  else
	    printf("  irq: %d-%d", res->start, res->start + res->len - 1);
	  break;

	case RESOURCE_DMA:
	  if (res->len == 1) 
	    printf("  dma: %d", res->start);
	  else
	    printf("  dma: %d-%d", res->start, res->start + res->len - 1);
	  break;
      }

      printf("\n");

      res = res->next;
    }

    unit = unit->next;
  }

  return 0;
}

int devices_proc(/*struct proc_file *pf,*/ /*void *arg*/)
{
  dev_t devno;
  struct dev *dev;

  printf("devno name     driver           type   unit\n");
  printf("----- -------- ---------------- ------ ------------------------------\n");

  for (devno = 0; devno < num_devs; devno++)
  {
    dev = devtab[devno];
    printf("%5d %-8s %-16s %-6s ", devno, dev->name, dev->driver->name, devtypenames[dev->driver->type]);
    if (dev->unit)
      printf("%s unit %d.%d\n", busnames[dev->unit->bus->bustype], dev->unit->bus->busno, dev->unit->unitno);
    else
      printf("<none>\n");
  }

  return 0;

}
