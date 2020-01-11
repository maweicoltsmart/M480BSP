#ifndef __TCP_CLIENT_H__
#define __TCP_CLIENT_H__

#include "lwip/opt.h"
#include "lwip/def.h"
#include "lwip/mem.h"
#include "lwip/pbuf.h"
//#include "lwip/timers.h"
#include "netif/etharp.h"
#include "lwip/err.h"

#include "FreeRTOS.h"
#include "semphr.h"

#define TCP_CLIENT_THREAD_PRIO                  1
#define TCP_CLIENT_STK_SIZE                     (128 * 2)
#define TCP_CLIENT_RX_BUFSIZE                   128
#define UART_STREAM_RX_BUFSIZE                  128

#define DEFAULT_SERVER_IP                       "47.101.57.10"
#define DEFAULT_SERVER_PORT                     5001

extern xTaskHandle xTaskHandleTcpClient;
extern xTaskHandle xTaskHandleUartIn;

void tcp_client_init(void);

#endif