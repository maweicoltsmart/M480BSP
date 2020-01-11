#ifndef __PRAGMA_H__
#define __PRAGMA_H__

#include <stdio.h>
#include "string.h"
#include "lwip/ip4_addr.h"
#include "FreeRTOS.h"

/* Data Flash block and address */
#define PRAGMA_DATA_FLASH_BLOCK_0                       (0 * 512)
#define PRAGMA_DATA_FLASH_BLOCK_1                       (1 * 512)
#define PRAGMA_DATA_FLASH_BLOCK_2                       (2 * 512)
#define PRAGMA_DATA_FLASH_BLOCK_2                       (2 * 512)

#define MODULE_WORK_MODE_MJ_CLOUD                       (0U)
#define MODULE_WORK_MODE_TCP_SERVER                     (1U)
#define MODULE_WORK_MODE_TCP_CLIENT                     (2U)
#define MODULE_WORK_MODE_UDP                            (3U)
#define MODULE_WORK_MODE_HTTP_CLIENT                    (4U)
#define MODULE_WORK_MODE_VSPD                           (5U)
extern unsigned char *const strworkmode[];
/* Module state code */
#define MODULE_STATE_IDLE                               (1U << 0)
#define MODULE_STATE_LISTEN                             (1U << 1)
#define MODULE_STATE_CONNECTING                         (1U << 2)
#define MODULE_STATE_CONNECTED                          (1U << 3)
#define MODULE_STATE_ERROR                              (1U << 4)

/* DHCP on or not*/
#define PRAGMA_DHCP_ON                                  (1U)
#define PRAGMA_DHCP_OFF                                 (0U)

/* UART PARITY */
#define PRAGMA_UART_PARITY_NONE                         (0U)
#define PRAGMA_UART_PARITY_ODD                          (1U)
#define PRAGMA_UART_PARITY_EVEN                         (2U)
#define PRAGMA_UART_PARITY_MARK                         (3U)
#define PRAGMA_UART_PARITY_SPACE                        (4U)
extern unsigned char *const strparity[];
/* UART FLOWCTRL */
#define PRAGMA_UART_FLOWCTRL_NONE                       (0U)
#define PRAGMA_UART_FLOWCTRL_HARDWARE                   (1U)
#define PRAGMA_UART_FLOWCTRL_XONOFF                     (2U)
extern unsigned char *const strflowctrl[];
/* UART stop bit */
#define PRAGMA_UART_STOP_BIT_1                          (0U)
#define PRAGMA_UART_STOP_BIT_1_5                        (1U)
#define PRAGMA_UART_STOP_BIT_2                          (2U)
extern unsigned char *const strstopbit[];
/* DNS Option */
#define HOST_NAME_MAX_LEN                               (30U)
#define DEFAULT_DNS_IP                                  "114.114.114.114"

/* DEFAULT LOCAL IP */
#define DEFAULT_LOCAL_IP                                "192.168.1.101"
#define DEFAULT_LOCAL_PORT                              5001
#define DEFAULT_LOCAL_NETMASK                           "255.255.255.0"
#define DEFAULT_LOCAL_GATEWAY                           "192.168.1.1"

/* DEFAULT REMOTE IP */
#define DEFAULT_REMOTE_IP                               "192.168.2.104"
#define DEFAULT_REMOTE_PORT                             5001
#define DEFAULT_REMOTE_HOST_NAME                        "www.coltsmart.com"

#define MAX_TCP_CLIENT_SURPORT                          8

#define HARDWARE_VERSION               "V1.0.0"
#define SOFTWARE_VERSION               "VERSION 1.0 ("__DATE__" "__TIME__")"

typedef struct{
  uint8_t state;        // IDLE, LISTEN, CONNECTING, CONNECTED, ERROR
  uint64_t time;        // minisecondes
  uint64_t txbytecnt;
  uint64_t rxbytecnt;
}st_Module_Info, *pst_Module_Info;

typedef struct{
  uint8_t dhcp;        // PRAGMA_DHCP_ON, PRAGMA_DHCP_OFF
  ip4_addr_t localip;
  uint16_t localport;
  ip4_addr_t netmask;
  ip4_addr_t gateway;
  ip4_addr_t dns;
}st_Local_IP, *pst_Local_IP;

typedef struct{
  uint8_t hostname[HOST_NAME_MAX_LEN];
  ip4_addr_t remoteip;
  uint16_t remoteport;
}st_Remote_IP, *pst_Remote_IP;

typedef struct{
  uint32_t baud;        // 600bps ~ 1Mbps
  uint8_t databit;      // 5,6,7,8
  uint8_t stopbit;      // 1,2
  uint8_t parity;       // PRAGMA_UART_PARITY_ODD,PRAGMA_UART_PARITY_EVEN,PRAGMA_UART_PARITY_MARK,PRAGMA_UART_PARITY_CLEAR
  uint8_t flowctrl;
}st_Uart_Pragma, *pst_Uart_Pragma;

typedef struct{
  uint8_t workmode;
  st_Local_IP stLocal_IP;
  st_Remote_IP stRemote_IP;
  st_Uart_Pragma stUart_Pragma;
  uint8_t devname[64 + 1];
  uint32_t crc32;
}st_Work_Pragma, *pst_Work_Pragma;

typedef struct{
  uint8_t devid[8];
  uint8_t macaddr[6];
  uint8_t devtype[64 + 1];
  uint8_t * hardwareversion;
  uint8_t * softversion;
  uint32_t crc32;
}st_Device_Info, *pst_Device_Info;

extern st_Work_Pragma stWork_Pragma;
extern st_Device_Info stDevice_Info;
extern st_Module_Info stModule_Info;

extern void factoryset(void);
extern void getdevinfo(void);
extern void factoryresetworkpragma(void);
extern void getworkpragma(void);
extern void restoreworkpragma(void);

#endif