#ifndef __TCP_SERVER_H__
#define __TCP_SERVER_H__

#include "lwip/opt.h"
#include "lwip/def.h"
#include "lwip/mem.h"
#include "lwip/pbuf.h"
//#include "lwip/timers.h"
#include "netif/etharp.h"
#include "lwip/err.h"

#include "FreeRTOS.h"
#include "semphr.h"

#define TCPSERVER_THREAD_PRIO           1
#define TCPSERVER_STK_SIZE              (128 * 2 - 90)
#define TCP_SERVER_RX_BUFSIZE           1024

void tcp_server_init(void);

#endif