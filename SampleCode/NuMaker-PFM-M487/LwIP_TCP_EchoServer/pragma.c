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
/*
  //DataFlash_BlockErase(PRAGMA_DATA_FLASH_BLOCK_1);

  uint32_t *pdata = (uint32_t *)&stDevice_Info;
  for(int loop = 0; loop < ((sizeof(stDevice_Info) % 4)?((int)sizeof(stDevice_Info) / 4 + 1):((int)sizeof(stDevice_Info) / 4)); loop ++)
  {
      //DataFlash_WriteDW(PRAGMA_DATA_FLASH_BLOCK_1 + loop * 4, pdata[loop]);
  }
  if(stDevice_Info.crc32 != ((pst_Device_Info)(DATA_FLASH_ADDR + PRAGMA_DATA_FLASH_BLOCK_1))->crc32)
  {
    printf("[ERROR:] stDevice_Info store error\r\n");
  }
  else
  {
    printf("stDevice_Info write flash success!\r\n");
  }
*/
}

void getdevinfo(void)
{
  //WWDG_ResetCfg(ENABLE);
  stModule_Info.rxbytecnt = 0;
  stModule_Info.txbytecnt = 0;
  /*memcpy(&stDevice_Info,(void*)(DATA_FLASH_ADDR + PRAGMA_DATA_FLASH_BLOCK_1),sizeof(stDevice_Info));
  if(stDevice_Info.crc32 != crc32((const char*)&stDevice_Info, sizeof(stDevice_Info) - 4))
  {
    printf("[ERROR:] nvs error! system init device info by default!\r\n");
    devinfoinit();
  }
  else
  {
    printf("get device info success\r\n");
  }
  WWDG_SetCounter(1);*/
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
  //memcpy(&stWork_Pragma,(void*)(DATA_FLASH_ADDR + PRAGMA_DATA_FLASH_BLOCK_0),sizeof(stWork_Pragma));
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
  //DataFlash_BlockErase(PRAGMA_DATA_FLASH_BLOCK_0);
  uint32_t *pdata = (uint32_t *)&stWork_Pragma;
  /*
  for(int loop = 0; loop < ((sizeof(stWork_Pragma) % 4)?((int)sizeof(stWork_Pragma) / 4 + 1):((int)sizeof(stWork_Pragma) / 4)); loop ++)
  {
      //DataFlash_WriteDW(PRAGMA_DATA_FLASH_BLOCK_0 + loop * 4, pdata[loop]);
  }
  if(stWork_Pragma.crc32 != ((pst_Work_Pragma)(DATA_FLASH_ADDR + PRAGMA_DATA_FLASH_BLOCK_0))->crc32)
  {
    printf("[ERROR:] stWork_Pragma store error\r\n");
  }
  else
  {
    printf("stWork_Pragma write flash success!\r\n");
  }*/
}