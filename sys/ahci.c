#include<sys/defs.h>
#include<sys/ahci.h>
#include<sys/kprintf.h>
#include<sys/pci.h>
#include<sys/virtualMemory.h>
#include<sys/phyMemMapper.h>

static uint64_t PORT_BASE;
static uint64_t kernbase;
// Start command engine
//uint64_t adr = 0x3EFFF000;
//uint64_t adr2 = 0x3AFFF000;
//uint8_t *buf2 = (uint8_t *)0x3EFFF00;
//uint8_t *buf = (uint8_t *)0x3AFFF00;
uint8_t *buf2 = (uint8_t *)0xFFFFFFFF85000000;
uint8_t *buf = (uint8_t *)0xFFFFFFFF84000000;

void start_cmd(hba_port_t *port)
{
	// Wait until CR (bit15) is cleared
 int j;
	while (port->cmd & HBA_PxCMD_CR){
    j=1;
  }
  if (j==1){
    kprintf("..\n");
  }
  int i;
  while (port->cmd & HBA_PxCMD_CLO){
    i=1;
  }
  if (i==1){
    kprintf("..\n");
  }
	// Set FRE (bit4) and ST (bit0)
  //port->cmd |= HBA_PxCMD_CLO;
  //port->sact |= HBA_PXSACT_DS;
  port->cmd |= HBA_PxCMD_FRE;
	port->cmd |= HBA_PxCMD_ST;
  
}
 
// Stop command engine
void stop_cmd(hba_port_t *port)
{
	port->cmd &= ~HBA_PxCMD_ST;
  while(1)
	{
		if (port->cmd & HBA_PxCMD_FR)
			continue;
		if (port->cmd & HBA_PxCMD_CR)
			continue;
		break;
	}
  int i=0;
  while(i<100000000){
    i++;
  }
	port->cmd &= ~HBA_PxCMD_FRE;
  while(1){
  if (port->cmd & HBA_PxCMD_FR){
    continue;
  }
  break;
  }
}

void memset1(void *str, int c, size_t n){
  unsigned char *s = str;
  while(n > 0){
      *s = c;
      s++;
      n--;
    }
}

void port_rebase(hba_port_t *port, int portno)
{	// Stop command engine
 
	// Command list offset: 1K*portno
	// Command list entry size = 32
	// Command list entry maxim count = 32
	// Command list maxim size = 32*32 = 1K per port
  
	port->clb = PORT_BASE + (portno<<10);
	memset1((void*)((port->clb)+kernbase), 0, 1024);
 
	// FIS offset: 32K+256*portno
	// FIS entry size = 256 bytes per port
	port->fb = PORT_BASE + (32<<10) + (portno<<8);
	//port->fbu = 0;
	memset1((void*)((port->fb)+kernbase), 0, 256);
 
	// Command table offset: 40K + 8K*portno
	// Command table size = 256*32 = 8K per port
	hba_cmd_header_t *cmdheader = (hba_cmd_header_t*)(port->clb+kernbase);
	for (int i=0; i<32; i++)
	{
		cmdheader[i].prdtl = 8;	// 8 prdt entries per command table
					// 256 bytes per command table, 64+16+48+16*8
		// Command table offset: 40K + 8K*portno + cmdheader_index*256
		cmdheader[i].ctba = PORT_BASE + (32<<10) + (portno<<13) + (i<<8);
		//cmdheader[i].ctbau = 0;
		memset1((void*)((cmdheader[i].ctba)+kernbase), 0, 256);
	}
  //For each implemented port, clear the PxSERR register, by writing 1 to each implemented location
  //port->is_rwc = 0; //
  //port->ie = 1;
	//start_cmd(port);	// Start command engine
  port->sctl = 0x001;
  int j=0;
  while(j<5){
  int i=0;
  while(i<100000000){
    i++;
  }
  j++;
  }
  //uint32_t ssts1 = port->ssts;
  //kprintf("ssts New1: %p \n",ssts1);
  port->sctl = 0x000;
  while((port->ssts&0xF) != 3){
    int i=0;
    while(i<100000000){
      i++;
    }
    //uint32_t ssts = port->ssts;
    //kprintf("ssts New: %p \n",ssts);
  }
  port->serr_rwc = 0xFFFFFFFF;
  port->is_rwc = 0xFFFFFFFF;
  j=0;
  while(j<5){
  int i=0;
  while(i<100000000){
    i++;
  }
  j++;
  }
  port->cmd |= HBA_PxICC;
  port->cmd |= HBA_PxCMD_POD;
  port->cmd |= HBA_PxCMD_SUD;
  j=0;
  while(j<5){
  int i=0;
  while(i<100000000){
    i++;
  }
  j++;
  }
  //port->cmd |= HBA_PxCMD_FRE;
	//port->cmd |= HBA_PxCMD_ST;
}

// Find a free command list slot
int find_cmdslot(hba_port_t *port)
{
	// If not set in SACT and CI, the slot is free
	uint32_t slots = (port->sact | port->ci);//m_port was used instead of port
	for (int i=0; i<MAX_CMD_SLOT_CNT; i++)
	{
		if ((slots&1) == 0)
			return i;
		slots >>= 1;
	}
	kprintf("Cannot find free command list entry\n");
	return -1;
}

int read(hba_port_t *port, uint32_t startl, uint32_t starth, uint32_t count, uint8_t *buf)
{
	port->is_rwc = (uint32_t)-1;		// Clear pending interrupt bits
	int spin = 0; // Spin lock timeout counter
	int slot = find_cmdslot(port);
	if (slot == -1){
		return 0;
  }
	hba_cmd_header_t *cmdheader = (hba_cmd_header_t*)port->clb;
	cmdheader += slot;
	cmdheader->cfl = sizeof(fis_reg_h2d_t)/sizeof(uint32_t);	// Command FIS size
	cmdheader->w = 0;		// Read from device
  //cmdheader->c =1;
  //cmdheader->p =1;
	cmdheader->prdtl = (uint16_t)((count-1)>>4) + 1;	// PRDT entries count
 
	hba_cmd_tbl_t *cmdtbl = (hba_cmd_tbl_t*)(cmdheader->ctba);
  memset1(cmdtbl, 0, sizeof(hba_cmd_tbl_t) + (cmdheader->prdtl-1)*sizeof(hba_prdt_entry_t));
 
	// 8K bytes (16 sectors) per PRDT
  int i=0;
	for (i=0; i<cmdheader->prdtl-1; i++)
	{
		cmdtbl->prdt_entry[i].dba = (uint64_t)buf;
		cmdtbl->prdt_entry[i].dbc = 8*1024;	// 8K bytes(should I subtract -1 ***************??
		cmdtbl->prdt_entry[i].i = 1;
		buf += 4*1024;	// 4K words
		count -= 16;	// 16 sectors
	}
	// Last entry
	cmdtbl->prdt_entry[i].dba = (uint64_t)buf;
	cmdtbl->prdt_entry[i].dbc = ((count<<9));	// 512 bytes per sector // should I subtract -1 ************??
	cmdtbl->prdt_entry[i].i = 1;
 
	// Setup command
	fis_reg_h2d_t *cmdfis = (fis_reg_h2d_t*)(&cmdtbl->cfis);
	cmdfis->fis_type = FIS_TYPE_REG_H2D;
	cmdfis->c = 1;	// Command
	cmdfis->command = ATA_CMD_READ_DMA_EX;
 
	cmdfis->lba0 = (uint8_t)startl;
	cmdfis->lba1 = (uint8_t)(startl>>8);
	cmdfis->lba2 = (uint8_t)(startl>>16);
	cmdfis->device = 1<<6;	// LBA mode
 
	cmdfis->lba3 = (uint8_t)(startl>>24);
	cmdfis->lba4 = (uint8_t)starth;
	cmdfis->lba5 = (uint8_t)(starth>>8);
 
	cmdfis->count = count;
	//cmdfis->counth = HIBYTE(count);
  //cmdfis->countl = LOBYTE(count);
 
	// The below loop waits until the port is no longer busy before issuing a new command
	while ((port->tfd & (ATA_STATUS_BSY | ATA_STATUS_DRQ)) && spin < 1000000)
	{
		spin++;
	}
	if (spin == 1000000)
	{
		kprintf("Read Port is hung\n");
		return 0;
	}
 
	port->ci = 1<<slot;	// Issue command
 
	// Wait for completion
	while (1)
	{
		// In some longer duration reads, it may be helpful to spin on the DPS bit 
		// in the PxIS port field as well (1 << 5)
		if ((port->ci & (1<<slot)) == 0) 
			break;
		if (port->is_rwc & HBA_PxIS_TFES)	// Task file error
		{
			kprintf("Read disk error\n");
			return 0;
		}
	}
 
	// Check again
	if (port->is_rwc & HBA_PxIS_TFES)
	{
		kprintf("Read disk error\n");
		return 0;
	}
 
	return 1;
}

int write(hba_port_t *port, uint32_t startl, uint32_t starth, uint32_t count, uint8_t *buf)
{
	port->is_rwc = (uint32_t)-1;		// Clear pending interrupt bits
	int spin = 0; // Spin lock timeout counter
	int slot = find_cmdslot(port);
	if (slot == -1){
    kprintf("Slot not working");
		return 0;
  }
  //kprintf("Here1");
	hba_cmd_header_t *cmdheader = (hba_cmd_header_t*)(port->clb+kernbase);
	cmdheader += slot;
	cmdheader->cfl = sizeof(fis_reg_h2d_t)/sizeof(uint32_t);	// Command FIS size
	cmdheader->w = 1;		// write from device
  //cmdheader->c =1;
  //cmdheader->p =1;
	cmdheader->prdtl = (uint16_t)((count-1)>>4) + 1;	// PRDT entries count
  //kprintf("Here2");
	hba_cmd_tbl_t *cmdtbl = (hba_cmd_tbl_t*)(cmdheader->ctba+kernbase);
	memset1(cmdtbl, 0, sizeof(hba_cmd_tbl_t) + (cmdheader->prdtl-1)*sizeof(hba_prdt_entry_t));
  //kprintf("Here3");
	// 8K bytes (16 sectors) per PRDT
  int i=0;
  //LOOK HERE, cmdtbl->prdt_entry[i] will be physical adress, but you need to acces physical.. so make changes accordingly.
	for (i=0; i<cmdheader->prdtl-1; i++)
	{
		cmdtbl->prdt_entry[i].dba = (uint64_t)buf;
		cmdtbl->prdt_entry[i].dbc = 8*1024;	// 8K bytes
		cmdtbl->prdt_entry[i].i = 1;
		buf += 4*1024;	// 4K words
		count -= 16;	// 16 sectors
	}
	// Last entry
  //kprintf("Here4");
	cmdtbl->prdt_entry[i].dba = (uint64_t)buf;
	cmdtbl->prdt_entry[i].dbc = ((count<<9));	// 512 bytes per sector
	cmdtbl->prdt_entry[i].i = 1;
 
	// Setup command
  //kprintf("Here5");
	fis_reg_h2d_t *cmdfis = (fis_reg_h2d_t*)(&cmdtbl->cfis);
	cmdfis->fis_type = FIS_TYPE_REG_H2D;
	cmdfis->c = 1;	// Command
	cmdfis->command = ATA_CMD_WRITE_DMA_EX;
  //kprintf("Here6");
	cmdfis->lba0 = (uint8_t)startl;
	cmdfis->lba1 = (uint8_t)(startl>>8);
	cmdfis->lba2 = (uint8_t)(startl>>16);
	cmdfis->device = 1<<6;	// LBA mode
  //kprintf("Here7");
	cmdfis->lba3 = (uint8_t)(startl>>24);
	cmdfis->lba4 = (uint8_t)starth;
	cmdfis->lba5 = (uint8_t)(starth>>8);
  //kprintf("Here8");
	cmdfis->count = count;
	//cmdfis->counth = HIBYTE(count);
  //cmdfis->countl = LOBYTE(count);
  
	// The below loop waits until the port is no longer busy before issuing a new command
	while ((port->tfd & (ATA_STATUS_BSY | ATA_STATUS_DRQ)) && spin < 1000000)
	{
		spin++;
	}
	if (spin == 1000000)
	{
		kprintf("Write Port is hung\n");
		return 0;
	}
  //kprintf("Here9");
	port->ci = 1<<slot;	// Issue command
  //kprintf("Here10");
	// Wait for completion
	while (1)
	{
		// In some longer duration reads, it may be helpful to spin on the DPS bit 
		// in the PxIS port field as well (1 << 5)
		if ((port->ci & (1<<slot)) == 0) 
			break;
		if (port->is_rwc & HBA_PxIS_TFES)	// Task file error
		{
			kprintf("Write disk error\n");
			return 0;
		}
	}
 
	// Check again
	if (port->is_rwc & HBA_PxIS_TFES)
	{
		kprintf("Write disk error\n");
		return 0;
	}
 
	return 1;
}

void write_read(hba_port_t *port, uint32_t startl, uint32_t starth, uint32_t count, uint8_t *inbuf,uint8_t *outbuf){
  kprintf("Writing and Reading into the memory...  ");
  int k=0;
  uint32_t writel = startl;
  uint32_t writeh = starth;
  uint8_t *outbuf2 = outbuf;
  //uint32_t readl = startl;
  //uint32_t readh = starth;
  while(k<100){
    int i=0;
    uint8_t *temp_l = inbuf;
    while(i<4096){
      *(temp_l+i) = k;
      i++; 
    }
    write(port,writel,writeh,8,inbuf);
    read(port,writel,writeh,8,outbuf2);
    kprintf("%d ",*(outbuf2+k));
    inbuf=inbuf+4096;
    writel=writel+8;
    outbuf2=outbuf2+4096;
    k++;
  }
  //kprintf("Reading into first block\n");
  /*while(k<100){
    read(port,readl,readh,8,outbuf2);
    kprintf("%d ",*(outbuf2+k));
    outbuf2=outbuf2+4096;
    readl=readl+8;
  }*/
  kprintf("\nRead and Write Succesfull..\n");
}

void port_probe(hba_mem_t *abar){
  uint32_t port_implem_reg = abar->pi;
  //kprintf("Inside AHCI: %p \n",port_implem_reg);
  int firstport = 0;
  PORT_BASE = get_AHCI_PHYS();
  kernbase = get_kernbase();
  for(int i=0;i<32;i++){
    if(port_implem_reg & 1){
      //int type = port_type(&abar->ports[i]);
      hba_port_t* port = &abar->ports[i];
      uint32_t type = port->sig;
      //kprintf("Type value: %x",type);
			if (type == AHCI_DEV_SATA){
        kprintf("    Port %d is linked to SATA Drive\n", i);
        /*uint32_t ssts = port->ssts;
        kprintf("SSTS: %p \n",ssts);
        uint32_t sctl = port->sctl;
        kprintf("SCTL: %p \n",sctl);
        uint32_t cmd = port->cmd;
        kprintf("CMD: %p \n",cmd);
        kprintf("is_rwc: %p \n",port->is_rwc);
        kprintf("tfd: %p \n",port->tfd);
        kprintf("serr_rwc: %p \n",port->serr_rwc);
        kprintf("sact: %p \n",port->sact);
        kprintf("ci: %p \n",port->ci);
        kprintf("ie: %p \n",port->ie);*/
        
        if(firstport == 0){
          //port->cmd &= ~HBA_PxCMD_ST;
          //port->cmd &= ~HBA_PxCMD_FRE;
          //port_rebase(port,i);
          //port->cmd |= HBA_PxCMD_FRE;
	        //port->cmd |= HBA_PxCMD_ST;
          //*buf2 = "Hello MF." ;
          /*int z=0;
          while(z<100){
            int y=0;
            while(y<4096){
              *(buf+z)=z;
              y++;
            }
            z++;
          }*/
          /*int z=0;
          while(z<4096){
            *(buf+z) = z;
            z++;
          }*/
          //kprintf("Writing\n");
          //write(port,0,0,8,buf);
          //write(port,8,0,8,buf);
          //kprintf("Reading\n");
          //read(port,8,0,8,buf2);
          //write_read(port,0,0,8,buf,buf2);
          firstport = i;
          //kprintf("Read data from disk: %d \n",*(buf2+300));
        }
        //rw_port = port;
			}
			else if (type == AHCI_DEV_SATAPI){
				kprintf("      Port %d is linked to SATAAPI Drive\n", i);
        if(firstport ==0){
          firstport =i;
        }
        //rw_port = port;
			}
			else if (type == AHCI_DEV_SEMB){
				kprintf("      Port %d is linked to SEMB drive\n", i);
        if(firstport ==0){
          firstport =i;
        }
        //rw_port = port;
			}
			else if (type == AHCI_DEV_PM){
				kprintf("      Port %d is linked to PM drive%d\n", i);
        if(firstport ==0){
          firstport =i;
        }
        //rw_port = port;
			}
			else{
				//kprintf("Drive not found at port %d\n", i);
			}
      //kprintf("Rebasing");
      //port_rebase(port,i);
    }
    else{
      port_implem_reg = port_implem_reg>>1;
    }
  }
  hba_port_t* rw_port = &abar->ports[firstport];
  abar->ghc = 0x80000001;
  abar->ghc |= (uint32_t) 0x2;
  kprintf("Port Rebasing..Please Wait..\n");
  stop_cmd(rw_port);
  port_rebase(rw_port,firstport);
  start_cmd(rw_port);
  //write_read(rw_port,0,0,8,buf,buf2);
  /*TODO: Need to change write from middle and read from first*/
  /*uint32_t ssts = rw_port->ssts;
        kprintf("SSTS: %p \n",ssts);
        uint32_t sctl = rw_port->sctl;
        kprintf("SCTL: %p \n",sctl);
        uint32_t cmd = rw_port->cmd;
        kprintf("CMD: %p \n",cmd);
        kprintf("is_rwc: %p \n",rw_port->is_rwc);
        kprintf("tfd: %p \n",rw_port->tfd);
        kprintf("serr_rwc: %p \n",rw_port->serr_rwc);
        kprintf("sact: %p \n",rw_port->sact);
        kprintf("ci: %p \n",rw_port->ci);
        kprintf("ie: %p \n",rw_port->ie);*/
}
