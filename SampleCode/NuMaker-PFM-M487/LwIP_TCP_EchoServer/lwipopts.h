/*
 * Copyright (c) 2001, 2002 Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 *
 * Author: Adam Dunkels <adam@sics.se>
 *
 */

#ifndef __LWIPOPTS_FREERTOS_H__
#define __LWIPOPTS_FREERTOS_H__

#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"

/*
#define LWIP_UDPLITE                    1
*/
#define LWIP_NETIF_HOSTNAME             1
extern int32_t rand1( void );
#define LWIP_RAND                       xTaskGetTickCount

/* tcpip_thread should run on HIGH priority */
#define TCPIP_THREAD_NAME              "lwIP"
#define TCPIP_THREAD_STACKSIZE         (256)
#define TCPIP_THREAD_PRIO              (configMAX_PRIORITIES - 2)


#define TCPIP_MBOX_SIZE                 5
#define DEFAULT_RAW_RECVMBOX_SIZE       5
#define DEFAULT_UDP_RECVMBOX_SIZE       5
#define DEFAULT_TCP_RECVMBOX_SIZE       5
#define DEFAULT_ACCEPTMBOX_SIZE         5


/*
   ------------------------------------
   ----------- HTTPD options ----------
   ------------------------------------
 */
#define HTTPD_ENABLE_HTTPS                                              0
#define LWIP_HTTPD_SUPPORT_POST                                         1
#define LWIP_HTTPD_SSI                                                  1
#define LWIP_HTTPD_CGI                                                  1
#define LWIP_HTTPD_CGI_SSI                                              0
#define LWIP_HTTPD_KILL_OLD_ON_CONNECTIONS_EXCEEDED                     1
#define LWIP_HTTPD_POST_MANUAL_WND                                      1
#define LWIP_HTTPD_SUPPORT_11_KEEPALIVE                                 0
#define LWIP_HTTPD_SSI_RAW                                              1
#define LWIP_HTTPD_FILE_STATE                                           0
#define LWIP_HTTPD_SSI_MULTIPART                                        1
#define LWIP_HTTPD_ABORT_ON_CLOSE_MEM_ERROR                             0

#define LWIP_IPV4                  1
#define LWIP_IPV6                  0

#define NO_SYS                     0
#define LWIP_SOCKET                (NO_SYS==0)
#define LWIP_NETCONN               (NO_SYS==0)
#define LWIP_NETIF_API             (NO_SYS==0)

#define LWIP_IGMP                  LWIP_IPV4
#define LWIP_ICMP                  LWIP_IPV4

#define LWIP_SNMP                  LWIP_UDP
#define MIB2_STATS                 LWIP_SNMP
#ifdef LWIP_HAVE_MBEDTLS
#define LWIP_SNMP_V3               (LWIP_SNMP)
#endif

#define LWIP_DNS                   LWIP_UDP
#define LWIP_MDNS_RESPONDER        LWIP_UDP

#define LWIP_NUM_NETIF_CLIENT_DATA (LWIP_MDNS_RESPONDER)

#define LWIP_HAVE_LOOPIF           0
#define LWIP_NETIF_LOOPBACK        0
#define LWIP_LOOPBACK_MAX_PBUFS    10

#define TCP_LISTEN_BACKLOG         1

#define LWIP_COMPAT_SOCKETS        1
#define LWIP_SO_RCVTIMEO           1
#define LWIP_SO_RCVBUF             1

#define LWIP_FREERTOS_CHECK_CORE_LOCKING        1
#define LWIP_TCPIP_CORE_LOCKING    1

#define LWIP_NETIF_LINK_CALLBACK        0
#define LWIP_NETIF_STATUS_CALLBACK      1
#define LWIP_NETIF_EXT_STATUS_CALLBACK  1

#define LWIP_NETCONN_FULLDUPLEX         0
#define LWIP_NETCONN_SEM_PER_THREAD     0

#define LWIP_DEBUG
//#undef LWIP_DEBUG
#ifdef LWIP_DEBUG

#define LWIP_DBG_MIN_LEVEL         LWIP_DBG_LEVEL_WARNING//LWIP_DBG_LEVEL_ALL//
#define PPP_DEBUG                  LWIP_DBG_OFF
#define MEM_DEBUG                  LWIP_DBG_OFF
#define MEMP_DEBUG                 LWIP_DBG_OFF
#define PBUF_DEBUG                 LWIP_DBG_OFF
#define API_LIB_DEBUG              LWIP_DBG_OFF
#define API_MSG_DEBUG              LWIP_DBG_OFF
#define TCPIP_DEBUG                LWIP_DBG_OFF
#define NETIF_DEBUG                LWIP_DBG_OFF
#define SOCKETS_DEBUG              LWIP_DBG_OFF
#define DNS_DEBUG                  LWIP_DBG_OFF
#define AUTOIP_DEBUG               LWIP_DBG_OFF
#define DHCP_DEBUG                 LWIP_DBG_OFF
#define IP_DEBUG                   LWIP_DBG_OFF
#define IP_REASS_DEBUG             LWIP_DBG_OFF
#define ICMP_DEBUG                 LWIP_DBG_OFF
#define IGMP_DEBUG                 LWIP_DBG_OFF
#define UDP_DEBUG                  LWIP_DBG_OFF
#define TCP_DEBUG                  LWIP_DBG_OFF
#define TCP_INPUT_DEBUG            LWIP_DBG_OFF
#define TCP_OUTPUT_DEBUG           LWIP_DBG_OFF
#define TCP_RTO_DEBUG              LWIP_DBG_OFF
#define TCP_CWND_DEBUG             LWIP_DBG_OFF
#define TCP_WND_DEBUG              LWIP_DBG_OFF
#define TCP_FR_DEBUG               LWIP_DBG_OFF
#define TCP_QLEN_DEBUG             LWIP_DBG_OFF
#define TCP_RST_DEBUG              LWIP_DBG_OFF
#define HTTPD_DEBUG                LWIP_DBG_OFF
#endif

#define LWIP_DBG_TYPES_ON         (LWIP_DBG_ON|LWIP_DBG_TRACE|LWIP_DBG_STATE|LWIP_DBG_FRESH|LWIP_DBG_HALT)


/* ---------- Memory options ---------- */
/* MEM_ALIGNMENT: should be set to the alignment of the CPU for which
   lwIP is compiled. 4 byte alignment -> define MEM_ALIGNMENT to 4, 2
   byte alignment -> define MEM_ALIGNMENT to 2. */
/* MSVC port: intel processors don't need 4-byte alignment,
   but are faster that way! */
#define MEM_ALIGNMENT           4U

/* MEM_SIZE: the size of the heap memory. If the application will send
a lot of data that needs to be copied, this should be set high. */
#define MEM_SIZE               (30 * 1024)
//#define MEM_USE_POOLS                   1
//#define MEM_USE_POOLS_TRY_BIGGER_POOL   1
//#define MEMP_USE_CUSTOM_POOLS           1
/* MEMP_NUM_PBUF: the number of memp struct pbufs. If the application
   sends a lot of data out of ROM (or other static memory), this
   should be set high. */
#define MEMP_NUM_PBUF           8
/* MEMP_NUM_RAW_PCB: the number of UDP protocol control blocks. One
   per active RAW "connection". */
#define MEMP_NUM_RAW_PCB        0
/* MEMP_NUM_UDP_PCB: the number of UDP protocol control blocks. One
   per active UDP "connection". */
#define MEMP_NUM_UDP_PCB        2
/* MEMP_NUM_TCP_PCB: the number of simulatenously active TCP
   connections. */
#define MEMP_NUM_TCP_PCB        6
/* MEMP_NUM_TCP_PCB_LISTEN: the number of listening TCP
   connections. */
#define MEMP_NUM_TCP_PCB_LISTEN 2
/* MEMP_NUM_TCP_SEG: the number of simultaneously queued TCP
   segments. */
#define MEMP_NUM_TCP_SEG        TCP_SND_QUEUELEN//16
/* MEMP_NUM_SYS_TIMEOUT: the number of simulateously active
   timeouts. */
#define MEMP_NUM_SYS_TIMEOUT    17

/* The following four are used only with the sequential API and can be
   set to 0 if the application only will use the raw API. */
/* MEMP_NUM_NETBUF: the number of struct netbufs. */
#define MEMP_NUM_NETBUF         1
/* MEMP_NUM_NETCONN: the number of struct netconns. */
#define MEMP_NUM_NETCONN        (MEMP_NUM_TCP_PCB + MEMP_NUM_UDP_PCB - 1)
/* MEMP_NUM_TCPIP_MSG_*: the number of struct tcpip_msg, which is used
   for sequential API communication and incoming packets. Used in
   src/api/tcpip.c. */
#define MEMP_NUM_TCPIP_MSG_API   8
#define MEMP_NUM_TCPIP_MSG_INPKT 8


/* ---------- Pbuf options ---------- */
/* PBUF_POOL_SIZE: the number of buffers in the pbuf pool. */
#define PBUF_POOL_SIZE          (60)

/* PBUF_POOL_BUFSIZE: the size of each pbuf in the pbuf pool. */
#define PBUF_POOL_BUFSIZE       256

/** SYS_LIGHTWEIGHT_PROT
 * define SYS_LIGHTWEIGHT_PROT in lwipopts.h if you want inter-task protection
 * for certain critical regions during buffer allocation, deallocation and memory
 * allocation and deallocation.
 */
#define SYS_LIGHTWEIGHT_PROT    (NO_SYS==0)


/* ---------- TCP options ---------- */
#define LWIP_TCP                1
#define TCP_TTL                 255
#define LWIP_TCP_KEEPALIVE      LWIP_TCP
#define  TCP_KEEPIDLE_DEFAULT     5UL       // 5秒内连接双方都无数据，则发起保活探测（该值默认为2小时）
#define  TCP_KEEPINTVL_DEFAULT    1UL         // 每1秒发送一次保活探测
//保活机制启动后，一共发送5次保活探测包，如果这5个包对方均无回应，则表示连接异常，内核关闭连接，并发送err回调到用户程序
#define  TCP_KEEPCNT_DEFAULT      2UL               
#define  TCP_MAXIDLE  TCP_KEEPCNT_DEFAULT * TCP_KEEPINTVL_DEFAULT

//#define LWIP_ALTCP              (LWIP_TCP)
#define LWIP_HAVE_MBEDTLS               1
#ifdef LWIP_HAVE_MBEDTLS
#define LWIP_ALTCP_TLS          (LWIP_TCP)
#define LWIP_ALTCP_TLS_MBEDTLS  (LWIP_TCP)
#endif


/* Controls if TCP should queue segments that arrive out of
   order. Define to 0 if your device is low on memory. */
#define TCP_QUEUE_OOSEQ         1

/* TCP Maximum segment size. */
#define TCP_MSS                 1460

/* TCP sender buffer space (bytes). */
#define TCP_SND_BUF                     (10 * TCP_MSS)
#define TCP_SND_QUEUELEN                ((4 * (TCP_SND_BUF) + (TCP_MSS - 1))/(TCP_MSS))
#define TCP_SNDLOWAT                    LWIP_MIN(LWIP_MAX(((TCP_SND_BUF)/2), (2 * TCP_MSS) + 1), (TCP_SND_BUF) - 1)
#define TCP_SNDQUEUELOWAT               LWIP_MAX(((TCP_SND_QUEUELEN)/2), 5)
/* TCP receive window. */
#define TCP_WND                 (TCP_MSS * 6)

/* Maximum number of retransmissions of data segments. */
#define TCP_MAXRTX              12

/* Maximum number of retransmissions of SYN segments. */
#define TCP_SYNMAXRTX           4


/* ---------- ARP options ---------- */
#define LWIP_ARP                1
#define ARP_TABLE_SIZE          5
#define ARP_QUEUEING            1


/* ---------- IP options ---------- */
/* Define IP_FORWARD to 1 if you wish to have the ability to forward
   IP packets across network interfaces. If you are going to run lwIP
   on a device with only one network interface, define this to 0. */
#define IP_FORWARD              1

/* IP reassembly and segmentation.These are orthogonal even
 * if they both deal with IP fragments */
#define IP_REASSEMBLY           1
#define IP_REASS_MAX_PBUFS      (10 * ((1500 + PBUF_POOL_BUFSIZE - 1) / PBUF_POOL_BUFSIZE))
#define MEMP_NUM_REASSDATA      IP_REASS_MAX_PBUFS
#define IP_FRAG                 1
#define IPV6_FRAG_COPYHEADER    1

/* ---------- ICMP options ---------- */
#define ICMP_TTL                255


/* ---------- DHCP options ---------- */
/* Define LWIP_DHCP to 1 if you want DHCP configuration of
   interfaces. */
#define LWIP_DHCP               LWIP_UDP

/* 1 if you want to do an ARP check on the offered address
   (recommended). */
#define DHCP_DOES_ARP_CHECK    (LWIP_DHCP)


/* ---------- AUTOIP options ------- */
#define LWIP_AUTOIP            (LWIP_DHCP)
#define LWIP_DHCP_AUTOIP_COOP  (LWIP_DHCP && LWIP_AUTOIP)


/* ---------- UDP options ---------- */
#define LWIP_UDP                1
#define LWIP_UDPLITE            LWIP_UDP
#define UDP_TTL                 255


/* ---------- RAW options ---------- */
#define LWIP_RAW                0


/* ---------- Statistics options ---------- */

#define LWIP_STATS              1
#define LWIP_STATS_DISPLAY      1

#if LWIP_STATS
#define LINK_STATS              1
#define IP_STATS                1
#define ICMP_STATS              1
#define IGMP_STATS              1
#define IPFRAG_STATS            1
#define UDP_STATS               1
#define TCP_STATS               1
#define MEM_STATS               1
#define MEMP_STATS              1
#define PBUF_STATS              1
#define SYS_STATS               1
#endif /* LWIP_STATS */

/* ---------- NETBIOS options ---------- */
#define LWIP_NETBIOS_RESPOND_NAME_QUERY 1

/* ---------- PPP options ---------- */

#define PPP_SUPPORT             0      /* Set > 0 for PPP */

#if PPP_SUPPORT

#define NUM_PPP                 1      /* Max PPP sessions. */


/* Select modules to enable.  Ideally these would be set in the makefile but
 * we're limited by the command line length so you need to modify the settings
 * in this file.
 */
#define PPPOE_SUPPORT           1
#define PPPOS_SUPPORT           1

#define PAP_SUPPORT             1      /* Set > 0 for PAP. */
#define CHAP_SUPPORT            1      /* Set > 0 for CHAP. */
#define MSCHAP_SUPPORT          0      /* Set > 0 for MSCHAP */
#define CBCP_SUPPORT            0      /* Set > 0 for CBCP (NOT FUNCTIONAL!) */
#define CCP_SUPPORT             0      /* Set > 0 for CCP */
#define VJ_SUPPORT              1      /* Set > 0 for VJ header compression. */
#define MD5_SUPPORT             1      /* Set > 0 for MD5 (see also CHAP) */

#endif /* PPP_SUPPORT */

/* The following defines must be done even in OPTTEST mode: */

#if !defined(NO_SYS) || !NO_SYS /* default is 0 */
extern void sys_check_core_locking(void);
#define LWIP_ASSERT_CORE_LOCKED()  sys_check_core_locking()
extern void sys_mark_tcpip_thread(void);
#define LWIP_MARK_TCPIP_THREAD()   sys_mark_tcpip_thread()

#if !defined(LWIP_TCPIP_CORE_LOCKING) || LWIP_TCPIP_CORE_LOCKING /* default is 1 */
extern void sys_lock_tcpip_core(void);
#define LOCK_TCPIP_CORE()          sys_lock_tcpip_core()
extern void sys_unlock_tcpip_core(void);
#define UNLOCK_TCPIP_CORE()        sys_unlock_tcpip_core()
#endif
#endif

/*
 * Use the errno.h mechanism from FreeRTOS.
 */
#if ( configUSE_POSIX_ERRNO == 1 )
    /**
     * @brief System error variable, errno.
     */
    extern int FreeRTOS_errno;
    #define errno FreeRTOS_errno
#endif

#endif /* __LWIPOPTS_FREERTOS_H__ */