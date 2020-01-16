#include <stdio.h>
#include "NuMicro.h"
#include "pragma.h"
#include "FreeRTOS.h"
#include "string.h"
#include "crc32.h"

unsigned char *const strworkmode[] = {"MJCloud", "TCPServer", "TCPClient", "UDP", "HTTPClient", "VSPD"};
unsigned char *const strflowctrl[] = {"None", "Hardware", "XonXoff"};
unsigned char *const strparity[] = {"None", "Odd", "Even", "Mark", "Space"};
unsigned char *const strstopbit[] = {"1", "1.5", "2"};

#pragma pack(4)
st_Work_Pragma stWork_Pragma;
st_Device_Info stDevice_Info;
#pragma pack()

st_Module_Info stModule_Info;

void flash_driver_init(void);
void flash_erase(uint32_t u32StartAddr, uint32_t u32EndAddr);
void flash_write(uint32_t u32StartAddr, uint32_t *data, uint32_t len);
void flash_read(uint32_t u32StartAddr, uint32_t *data, uint32_t len);

static int  set_data_flash_base(uint32_t u32DFBA)
{
    uint32_t   au32Config[2];          /* User Configuration */

    /* Read User Configuration 0 & 1 */
    if (FMC_ReadConfig(au32Config, 2) < 0)
    {
        printf("\nRead User Config failed!\n");       /* Error message */
        return -1;                     /* failed to read User Configuration */
    }

    /* Check if Data Flash is enabled and is expected address. */
    if ((!(au32Config[0] & 0x1)) && (au32Config[1] == u32DFBA))
        return 0;                      /* no need to modify User Configuration */

    FMC_ENABLE_CFG_UPDATE();           /* Enable User Configuration update. */

    au32Config[0] &= ~0x1;             /* Clear CONFIG0 bit 0 to enable Data Flash */
    au32Config[1] = u32DFBA;           /* Give Data Flash base address  */

    /* Update User Configuration settings. */
    if (FMC_WriteConfig(au32Config, 2) < 0)
        return -1;                     /* failed to write user configuration */

    printf("\nSet Data Flash base as 0x%x.\n", DATA_FLASH_BASE);  /* debug message */

    /* Perform chip reset to make new User Config take effect. */
    SYS->IPRST0 = SYS_IPRST0_CHIPRST_Msk;
    return 0;                          /* success */
}

void flash_driver_init(void)
{
    uint32_t    i, u32Data;            /* variables */
    printf("+------------------------------------------+\n");
    printf("|    M480 FMC Read/Write Sample Demo       |\n");
    printf("+------------------------------------------+\n");

    SYS_UnlockReg();                   /* Unlock register lock protect */

    FMC_Open();                        /* Enable FMC ISP function */

    /* Enable Data Flash and set base address. */
#if 0
    if (set_data_flash_base(DATA_FLASH_BASE) < 0)
    {
        printf("Failed to set Data Flash base address!\n");          /* error message */
        //goto lexit;                    /* failed to configure Data Flash, aborted */
    }
#endif
    /* Get booting source (APROM/LDROM) */
    printf("  Boot Mode ............................. ");
    if (FMC_GetBootSource() == 0)
        printf("[APROM]\n");           /* debug message */
    else
    {
        printf("[LDROM]\n");           /* debug message */
        printf("  WARNING: The driver sample code must execute in AP mode!\n");
        //goto lexit;                    /* failed to get boot source */
    }

    u32Data = FMC_ReadCID();           /* Get company ID, should be 0xDA. */
    printf("  Company ID ............................ [0x%08x]\n", u32Data);   /* information message */

    u32Data = FMC_ReadPID();           /* Get product ID. */
    printf("  Product ID ............................ [0x%08x]\n", u32Data);   /* information message */

    for (i = 0; i < 3; i++)            /* Get 96-bits UID. */
    {
        u32Data = FMC_ReadUID(i);
        printf("  Unique ID %d ........................... [0x%08x]\n", i, u32Data);  /* information message */
    }

    for (i = 0; i < 4; i++)            /* Get 4 words UCID. */
    {
        u32Data = FMC_ReadUCID(i);
        printf("  Unique Customer ID %d .................. [0x%08x]\n", i, u32Data);  /* information message */
    }

    /* Read User Configuration CONFIG0 */
    printf("  User Config 0 ......................... [0x%08x]\n", FMC_Read(FMC_CONFIG_BASE));
    /* Read User Configuration CONFIG1 */
    printf("  User Config 1 ......................... [0x%08x]\n", FMC_Read(FMC_CONFIG_BASE+4));

    /* Read Data Flash base address */
    u32Data = FMC_ReadDataFlashBaseAddr();
    printf("  Data Flash Base Address ............... [0x%08x]\n", u32Data);   /* information message */
    FMC_Close();                       /* Disable FMC ISP function */

    SYS_LockReg();                     /* Lock protected registers */
}

void flash_erase(uint32_t u32StartAddr, uint32_t u32EndAddr)
{
    uint32_t    u32Addr;               /* flash address */
    SYS_UnlockReg();                   /* Unlock register lock protect */
    FMC_Open();                        /* Enable FMC ISP function */
    FMC_ENABLE_AP_UPDATE();            /* Enable APROM update. */
    for (u32Addr = u32StartAddr; u32Addr < u32EndAddr; u32Addr += FMC_FLASH_PAGE_SIZE)
    {
        FMC_Erase(u32Addr);            /* Erase page */
    }
    FMC_DISABLE_AP_UPDATE();           /* Disable APROM update. */
    FMC_Close();                       /* Disable FMC ISP function */
    SYS_LockReg();                     /* Lock protected registers */
}

void flash_write(uint32_t u32StartAddr, uint32_t *data, uint32_t len)
{
    uint32_t u32Addr, loop;                  /* flash address */
    SYS_UnlockReg();                   /* Unlock register lock protect */
    FMC_Open();                        /* Enable FMC ISP function */
    FMC_ENABLE_AP_UPDATE();            /* Enable APROM update. */
    /* Fill flash range from u32StartAddr to u32EndAddr with data word u32Pattern. */
    for (u32Addr = u32StartAddr, loop = 0 ; loop < len; loop ++)
    {
        FMC_Write(u32Addr, *data);          /* Program flash */
        data ++;
        u32Addr += 4;
    }
    FMC_DISABLE_AP_UPDATE();           /* Disable APROM update. */
    FMC_Close();                       /* Disable FMC ISP function */
    SYS_LockReg();                     /* Lock protected registers */
}

void flash_read(uint32_t u32StartAddr, uint32_t *data, uint32_t len)
{
    uint32_t u32Addr, loop;                  /* flash address */
    SYS_UnlockReg();                   /* Unlock register lock protect */
    FMC_Open();                        /* Enable FMC ISP function */
    FMC_ENABLE_AP_UPDATE();            /* Enable APROM update. */
    /* Fill flash range from u32StartAddr to u32EndAddr with data word u32Pattern. */
    for (u32Addr = u32StartAddr, loop = 0 ; loop < len; loop ++)
    {
        *data = FMC_Read(u32Addr);
        data ++;
        u32Addr += 4;
    }
    FMC_DISABLE_AP_UPDATE();           /* Disable APROM update. */
    FMC_Close();                       /* Disable FMC ISP function */
    SYS_LockReg();                     /* Lock protected registers */
}

void devinfoinit(void)
{
  //WWDG_SetCounter(1);
  //WWDG_ResetCfg(ENABLE);
  stDevice_Info.devid[0] = 0x20;
  stDevice_Info.devid[1] = 0x19;
  stDevice_Info.devid[2] = 0x11;
  stDevice_Info.devid[3] = 0x01;
  stDevice_Info.devid[4] = 0x00;
  stDevice_Info.devid[5] = 0x00;
  stDevice_Info.devid[6] = 0x00;
  stDevice_Info.devid[7] = 0x01;

  //GetMACAddress(stDevice_Info.macaddr);
  //WWDG_SetCounter(1);
  memset(stDevice_Info.devtype, 0, 64 + 1);
  memcpy(stDevice_Info.devtype, "ETH-DTU-MOD", strlen("ETH-DTU-MOD"));

  stDevice_Info.hardwareversion = HARDWARE_VERSION;

  stDevice_Info.softversion = SOFTWARE_VERSION;

  stDevice_Info.crc32 = crc32((const char*)&stDevice_Info, sizeof(stDevice_Info) - 4);
  //WWDG_SetCounter(1);

  //DataFlash_BlockErase(PRAGMA_DATA_FLASH_BLOCK_1);
  flash_erase(PRAGMA_DATA_FLASH_BLOCK_1, PRAGMA_DATA_FLASH_BLOCK_2);
  uint32_t *pdata = (uint32_t *)&stDevice_Info;
  flash_write(PRAGMA_DATA_FLASH_BLOCK_1, pdata, ((sizeof(stDevice_Info) % 4)?((int)sizeof(stDevice_Info) / 4 + 1):((int)sizeof(stDevice_Info) / 4)));
  /*for(int loop = 0; loop < ((sizeof(stDevice_Info) % 4)?((int)sizeof(stDevice_Info) / 4 + 1):((int)sizeof(stDevice_Info) / 4)); loop ++)
  {
      //DataFlash_WriteDW(PRAGMA_DATA_FLASH_BLOCK_1 + loop * 4, pdata[loop]);
  }*/
  if(stDevice_Info.crc32 != ((pst_Device_Info)(PRAGMA_DATA_FLASH_BLOCK_1))->crc32)
  {
    printf("[ERROR:] stDevice_Info store error\r\n");
  }
  else
  {
    printf("stDevice_Info write flash success!\r\n");
  }
}

void getdevinfo(void)
{
  //WWDG_ResetCfg(ENABLE);
  stModule_Info.rxbytecnt = 0;
  stModule_Info.txbytecnt = 0;
  memcpy(&stDevice_Info,(void*)(PRAGMA_DATA_FLASH_BLOCK_1),sizeof(stDevice_Info));
  if(stDevice_Info.crc32 != crc32((const char*)&stDevice_Info, sizeof(stDevice_Info) - 4))
  {
    printf("[ERROR:] nvs error! system init device info by default!\r\n");
    devinfoinit();
  }
  else
  {
    printf("get device info success\r\n");
  }
  //WWDG_SetCounter(1);
}

void factoryresetworkpragma(void)
{
  stWork_Pragma.workmode = MODULE_WORK_MODE_TCP_SERVER;
  stWork_Pragma.stLocal_IP.dhcp = PRAGMA_DHCP_ON;
  ip4addr_aton(DEFAULT_LOCAL_IP,&stWork_Pragma.stLocal_IP.localip);
  stWork_Pragma.stLocal_IP.localport = DEFAULT_LOCAL_PORT;
  ip4addr_aton(DEFAULT_LOCAL_NETMASK,&stWork_Pragma.stLocal_IP.netmask);
  ip4addr_aton(DEFAULT_LOCAL_GATEWAY,&stWork_Pragma.stLocal_IP.gateway);
  ip4addr_aton(DEFAULT_DNS_IP,&stWork_Pragma.stLocal_IP.dns);

  ip4addr_aton(DEFAULT_REMOTE_IP,&stWork_Pragma.stRemote_IP.remoteip);
  memset(stWork_Pragma.stRemote_IP.hostname,0,HOST_NAME_MAX_LEN);
  memcpy(stWork_Pragma.stRemote_IP.hostname,DEFAULT_REMOTE_HOST_NAME,strlen(DEFAULT_REMOTE_HOST_NAME));
  stWork_Pragma.stRemote_IP.remoteport = DEFAULT_REMOTE_PORT;

  stWork_Pragma.stUart_Pragma.baud = 115200;
  stWork_Pragma.stUart_Pragma.databit = 8;
  stWork_Pragma.stUart_Pragma.stopbit = PRAGMA_UART_STOP_BIT_1;
  stWork_Pragma.stUart_Pragma.parity = PRAGMA_UART_PARITY_NONE;
  stWork_Pragma.stUart_Pragma.flowctrl = pdTRUE;

  memset(stWork_Pragma.devname, 0, 64 + 1);
  memcpy(stWork_Pragma.devname, "恒大华府0001", strlen("恒大华府0001"));

  stWork_Pragma.crc32 = crc32((const char*)&stWork_Pragma, sizeof(stWork_Pragma) - 4);
  //WWDG_SetCounter(1);
  restoreworkpragma();
}

void getworkpragma(void)
{
  memcpy(&stWork_Pragma,(void*)(PRAGMA_DATA_FLASH_BLOCK_0),sizeof(stWork_Pragma));
  if(stWork_Pragma.crc32 != crc32((const char*)&stWork_Pragma, sizeof(stWork_Pragma) - 4))
  {
    printf("[ERROR:] nvs error! system do factory reset by default!\r\n");
    factoryresetworkpragma();
  }
  else
  {
    printf("get workpragma success\r\n");
  }
  //WWDG_SetCounter(1);
}

void restoreworkpragma(void)
{
  flash_erase(PRAGMA_DATA_FLASH_BLOCK_0, PRAGMA_DATA_FLASH_BLOCK_1);
  uint32_t *pdata = (uint32_t *)&stWork_Pragma;

  flash_write(PRAGMA_DATA_FLASH_BLOCK_0, pdata, ((sizeof(stWork_Pragma) % 4)?((int)sizeof(stWork_Pragma) / 4 + 1):((int)sizeof(stWork_Pragma) / 4)));
  /*for(int loop = 0; loop < ((sizeof(stWork_Pragma) % 4)?((int)sizeof(stWork_Pragma) / 4 + 1):((int)sizeof(stWork_Pragma) / 4)); loop ++)
  {
      //DataFlash_WriteDW(PRAGMA_DATA_FLASH_BLOCK_0 + loop * 4, pdata[loop]);
  }*/
  if(stWork_Pragma.crc32 != ((pst_Work_Pragma)(PRAGMA_DATA_FLASH_BLOCK_0))->crc32)
  {
    printf("[ERROR:] stWork_Pragma store error\r\n");
  }
  else
  {
    printf("stWork_Pragma write flash success!\r\n");
  }
}