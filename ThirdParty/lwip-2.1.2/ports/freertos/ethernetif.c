/**
* @file
* Ethernet Interface Skeleton
*
*/

/*
* Copyright (c) 2001-2004 Swedish Institute of Computer Science.
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

/*
* This file is a skeleton for developing Ethernet network interface
* drivers for lwIP. Add code to the low_level functions and do a
* search-and-replace for the word "ethernetif" to replace it with
* something that better describes your network interface.
*/

#include "lwip/opt.h"
#include "lwip/def.h"
#include "lwip/mem.h"
#include "lwip/pbuf.h"
#include "lwip/timeouts.h"
#include "netif/etharp.h"
#include "lwip/err.h"
#include "ethernetif.h"

#include "FreeRTOS.h"
#include "semphr.h"
#include "CH57x_common.h"
#include <string.h>
#include "netconf.h"
#include "ETH.h"

#define netifMTU                                (TCP_MSS + 40)
#define netifINTERFACE_TASK_STACK_SIZE          (64 + 13)
#define netifINTERFACE_TASK_PRIORITY            ( configMAX_PRIORITIES )
#define netifGUARD_BLOCK_TIME                   ( 250 )
/* The time to block waiting for input. */
#define emacBLOCK_TIME_WAITING_FOR_INPUT        ( ( portTickType ) 100 )

/* Define those to better describe your network interface. */
#define IFNAME0 's'
#define IFNAME1 't'

static struct netif *s_pxNetIf = NULL;
xSemaphoreHandle s_xSemaphore = NULL;
//xSemaphoreHandle xTxSemaphore = NULL;
static void ethernetif_input( void * pvParameters );
static void arp_timer(void *arg);

extern UINT8 TestMAC[MACADDR_LEN];

/**
* In this function, the hardware should be initialized.
* Called from ethernetif_init().
*
* @param netif the already initialized lwip network interface structure
*        for this ethernetif
*/
static void low_level_init(struct netif *netif)
{
  /* set netif MAC hardware address length */
  netif->hwaddr_len = ETHARP_HWADDR_LEN;

  /* set netif MAC hardware address */
  netif->hwaddr[0] =  TestMAC[0];
  netif->hwaddr[1] =  TestMAC[1];
  netif->hwaddr[2] =  TestMAC[2];
  netif->hwaddr[3] =  TestMAC[3];
  netif->hwaddr[4] =  TestMAC[4];
  netif->hwaddr[5] =  TestMAC[5];

  /* set netif maximum transfer unit */
  netif->mtu = netifMTU;

  /* Accept broadcast address and ARP traffic */
  netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP;

  s_pxNetIf =netif;

  /* create binary semaphore used for informing ethernetif of frame reception */
  while (s_xSemaphore == NULL)
  {
    vSemaphoreCreateBinary(s_xSemaphore);
    xSemaphoreTake( s_xSemaphore, 0);
  }

  /*while (xTxSemaphore == NULL)
  {
    vSemaphoreCreateBinary(xTxSemaphore);
    //xSemaphoreTake( xTxSemaphore, 0);
  }*/
  // xSemaphoreGive(xTxSemaphore);
  /* initialize MAC address in ethernet MAC */
  // ETH_MACAddressConfig(ETH_MAC_Address0, netif->hwaddr);
  /*R8_ETH_MAADR1 = netif->hwaddr[5];                                                 //MAC赋值
  R8_ETH_MAADR2 = netif->hwaddr[4];
  R8_ETH_MAADR3 = netif->hwaddr[3];
  R8_ETH_MAADR4 = netif->hwaddr[2];
  R8_ETH_MAADR5 = netif->hwaddr[1];
  R8_ETH_MAADR6 = netif->hwaddr[0];*/

#if 0
  /* Initialize Tx Descriptors list: Chain Mode */
  ETH_DMATxDescChainInit(DMATxDscrTab, &Tx_Buff[0][0], ETH_TXBUFNB);
  /* Initialize Rx Descriptors list: Chain Mode  */
  ETH_DMARxDescChainInit(DMARxDscrTab, &Rx_Buff[0][0], ETH_RXBUFNB);

  /* Enable Ethernet Rx interrrupt */
  {
    for(i=0; i<ETH_RXBUFNB; i++)
    {
      ETH_DMARxDescReceiveITConfig(&DMARxDscrTab[i], ENABLE);
    }
  }

#ifdef CHECKSUM_BY_HARDWARE
  /* Enable the checksum insertion for the Tx frames */
  {
    for(i=0; i<ETH_TXBUFNB; i++)
    {
      ETH_DMATxDescChecksumInsertionConfig(&DMATxDscrTab[i], ETH_DMATxDesc_ChecksumTCPUDPICMPFull);
    }
  }
#endif
#endif
  /* create the task that handles the ETH_MAC */
  xTaskCreate(ethernetif_input, "Eth_if", netifINTERFACE_TASK_STACK_SIZE, NULL,
              netifINTERFACE_TASK_PRIORITY,NULL);

    /* Enable MAC and DMA transmission and reception */
    // ETH_Start();
    //NVIC_EnableIRQ(ETH_IRQn);
}


/**
* This function should do the actual transmission of the packet. The packet is
* contained in the pbuf that is passed to the function. This pbuf
* might be chained.
*
* @param netif the lwip network interface structure for this ethernetif
* @param p the MAC packet to send (e.g. IP packet including MAC addresses and type)
* @return ERR_OK if the packet could be sent
*         an err_t value if the packet couldn't be sent
*
* @note Returning ERR_MEM here if a DMA queue of your MAC is full can lead to
*       strange results. You might consider waiting for space in the DMA queue
*       to become availale since the stack doesn't retry to send a packet
*       dropped because of memory failure (except for the TCP timers).
*/
#ifndef ETH_RX_BUF_SIZE
 #define ETH_RX_BUF_SIZE         ETH_MAX_PACKET_SIZE
#endif
#ifndef ETH_TX_BUF_SIZE
 #define ETH_TX_BUF_SIZE         ETH_MAX_PACKET_SIZE
#endif
static err_t low_level_output(struct netif *netif, struct pbuf *p)
{
  //static xSemaphoreHandle xTxSemaphore = NULL;
  struct pbuf *q;
  st_framelistitem *pframe;
  u8_t *buffer ;
  uint16_t framelength = 0;
  uint32_t bufferoffset = 0;
  uint32_t byteslefttocopy = 0;
  uint32_t payloadoffset = 0;

  if((p == NULL) || (p->len < 1))
  {
      return ERR_BUF;
  }
  if(p->tot_len > ETH_TX_BUF_SIZE)
  {
      printf("%s, %d\r\n", __func__, __LINE__);
      while(1);
  }
  if(current_dma_tx_fram_point != NULL)
  {
      return ERR_MEM;
  }
  /*if (xTxSemaphore == NULL)
  {
    vSemaphoreCreateBinary (xTxSemaphore);
  }

  if (xSemaphoreTake(xTxSemaphore, netifGUARD_BLOCK_TIME))*/
  {
    //while(current_dma_tx_fram_point != NULL);
    taskENTER_CRITICAL();
    //NVIC_DisableIRQ(ETH_IRQn);
    pframe = eth_idle_frame_buffer_get();
    taskEXIT_CRITICAL();
    if(pframe == NULL)
    {
        //printf("%s, %d\r\n", __func__, __LINE__);
        //while(1);
        return ERR_MEM;//vTaskDelay(pdMS_TO_TICKS( 1 ));
    }
    buffer = (u8_t *)(pframe->buffer);
    bufferoffset = 0;

    for(q = p; q != NULL; q = q->next)
    {

      /* Get bytes in current lwIP buffer  */
      byteslefttocopy = q->len;
      payloadoffset = 0;

      /* Check if the length of data to copy is bigger than Tx buffer size*/
      while( (byteslefttocopy + bufferoffset) > ETH_TX_BUF_SIZE )
      {
        /* Copy data to Tx buffer*/
        memcpy( (u8_t*)((u8_t*)buffer + bufferoffset), (u8_t*)((u8_t*)q->payload + payloadoffset), (ETH_TX_BUF_SIZE - bufferoffset) );
        pframe->len = ETH_TX_BUF_SIZE;
        taskENTER_CRITICAL();
        //NVIC_DisableIRQ(ETH_IRQn);
        eth_tx_frame_buffer_list_add(pframe);
        taskEXIT_CRITICAL();
        taskENTER_CRITICAL();
        //NVIC_DisableIRQ(ETH_IRQn);
        pframe = eth_idle_frame_buffer_get();
        taskEXIT_CRITICAL();
        //NVIC_EnableIRQ(ETH_IRQn);
        if(pframe == NULL)
        {
          //printf("%s, %d\r\n", __func__, __LINE__);
          //while(1);
          return ERR_MEM;
        }

        buffer = (u8_t *)(pframe->buffer);

        byteslefttocopy = byteslefttocopy - (ETH_TX_BUF_SIZE - bufferoffset);
        payloadoffset = payloadoffset + (ETH_TX_BUF_SIZE - bufferoffset);
        framelength = framelength + (ETH_TX_BUF_SIZE - bufferoffset);
        bufferoffset = 0;
      }

      /* Copy the remaining bytes */
      memcpy( (u8_t*)((u8_t*)buffer + bufferoffset), (u8_t*)((u8_t*)q->payload + payloadoffset), byteslefttocopy );
      bufferoffset = bufferoffset + byteslefttocopy;
      framelength = framelength + byteslefttocopy;
    }
    pframe->len = bufferoffset;
    current_dma_tx_fram_point = pframe;
    /*taskENTER_CRITICAL();
    //NVIC_DisableIRQ(ETH_IRQn);
    eth_tx_frame_buffer_list_add(pframe);
    taskEXIT_CRITICAL();*/

//error:
    if(current_dma_tx_fram_point != NULL)
    {
      //printf("%s, %d\r\n", __func__, __LINE__);
      /*taskENTER_CRITICAL();
      //NVIC_DisableIRQ(ETH_IRQn);
      pframe = eth_tx_frame_buffer_list_get();
      taskEXIT_CRITICAL();*/
      //NVIC_EnableIRQ(ETH_IRQn);
      //if((pframe != NULL) && (pframe->len > 0))
      {
        //printf("%s, %d, %d\r\n", __func__, __LINE__, pframe->len);
        //current_dma_tx_fram_point = pframe;
        //ETH_Prepare_Transmit_Descriptors(framelength);
        R16_ETH_ETXLN = current_dma_tx_fram_point->len;
        R16_ETH_ETXST = (UINT16)current_dma_tx_fram_point->buffer;
        R8_ETH_ECON1 |= RB_ETH_ECON1_TXRTS;                                     //开始发送
      }

    }
    else
    {
      //printf("%s, %d\r\n", __func__, __LINE__);
    }
  }

  //xSemaphoreGive(xTxSemaphore);
  return ERR_OK;
}

/**
* Should allocate a pbuf and transfer the bytes of the incoming
* packet from the interface into the pbuf.
*
* @param netif the lwip network interface structure for this ethernetif
* @return a pbuf filled with the received packet (including MAC header)
*         NULL on memory error
*/
static struct pbuf * low_level_input(struct netif *netif)
{
  struct pbuf *p= NULL, *q;
  u32_t len;
  st_framelistitem *pframe;
  u8_t *buffer;
  //u8_t loop;
  //__IO ETH_DMADESCTypeDef *DMARxDesc;
  uint32_t bufferoffset = 0;
  uint32_t payloadoffset = 0;
  uint32_t byteslefttocopy = 0;

  len = 0;

  taskENTER_CRITICAL();
  pframe = eth_rx_frame_buffer_list_get();
  taskEXIT_CRITICAL();
  if(pframe != NULL)
  {
      len = pframe->len;
  }
  else
  {
      return p;
  }
  /*for(loop = 0;loop < 3;loop ++)
  {
    
    if(pframe[loop] != NULL)
    {
      len += pframe[loop]->len;
    }
    else
    {
      break;
    }
  }*/
  /* get received frame */
  //frame = ETH_Get_Received_Frame_interrupt();

  /* Obtain the size of the packet and put it into the "len" variable. */
  //len = frame.length;

  if (len > 0)
  {
    buffer = (u8_t *)pframe->buffer;
    /* We allocate a pbuf chain of pbufs from the Lwip buffer pool */
    p = pbuf_alloc(PBUF_RAW, len, PBUF_POOL);
  }
  //loop = 0;
  if (p != NULL)
  {
    bufferoffset = 0;
    for(q = p; q != NULL; q = q->next)
    {
      byteslefttocopy = q->len;
      payloadoffset = 0;

      /* Check if the length of bytes to copy in current pbuf is bigger than Rx buffer size*/
      while( (byteslefttocopy + bufferoffset) > ETH_RX_BUF_SIZE )
      {
        /* Copy data to pbuf*/
        memcpy( (u8_t*)((u8_t*)q->payload + payloadoffset), (u8_t*)((u8_t*)buffer + bufferoffset), (ETH_RX_BUF_SIZE - bufferoffset));

        /* Point to next descriptor */
        //loop ++;
        buffer = (u8_t *)pframe->buffer;

        byteslefttocopy = byteslefttocopy - (ETH_RX_BUF_SIZE - bufferoffset);
        payloadoffset = payloadoffset + (ETH_RX_BUF_SIZE - bufferoffset);
        bufferoffset = 0;
      }

      /* Copy remaining data in pbuf */
      memcpy( (u8_t*)((u8_t*)q->payload + payloadoffset), (u8_t*)((u8_t*)buffer + bufferoffset), byteslefttocopy);
      bufferoffset = bufferoffset + byteslefttocopy;
    }
  }
  /*else
  {
    if(len > 0)
    {
        printf("pbuf_alloc %d bytes failed\r\n", len);
        while(1);
    }
  }*/

  /* Release descriptors to DMA */
  //for(loop = 0;loop < 3;loop ++)
  {
    if (pframe != NULL)
    {
      taskENTER_CRITICAL();
      //NVIC_DisableIRQ(ETH_IRQn);
      eth_idle_frame_buffer_add(pframe);
      taskEXIT_CRITICAL();
    }
  }

  return p;
}


/**
* This function is the ethernetif_input task, it is processed when a packet
* is ready to be read from the interface. It uses the function low_level_input()
* that should handle the actual reception of bytes from the network
* interface. Then the type of the received packet is determined and
* the appropriate input function is called.
*
* @param netif the lwip network interface structure for this ethernetif
*/
void ethernetif_input( void * pvParameters )
{
  struct pbuf *p;

  for( ;; )
  {
    if (xSemaphoreTake( s_xSemaphore, emacBLOCK_TIME_WAITING_FOR_INPUT)==pdTRUE)
    {
TRY_GET_NEXT_FRAME:
      p = low_level_input( s_pxNetIf );
      if   (p != NULL)
      {
        if (ERR_OK != s_pxNetIf->input( p, s_pxNetIf))
        {
          pbuf_free(p);
          if(eth_rx_frame_buffer_list_len_get() > 0)
          {
            goto TRY_GET_NEXT_FRAME;
          }
        }
        else
        {
          goto TRY_GET_NEXT_FRAME;
        }
      }
    }
  }
}

/**
* Should be called at the beginning of the program to set up the
* network interface. It calls the function low_level_init() to do the
* actual setup of the hardware.
*
* This function should be passed as a parameter to netif_add().
*
* @param netif the lwip network interface structure for this ethernetif
* @return ERR_OK if the loopif is initialized
*         ERR_MEM if private data couldn't be allocated
*         any other err_t on error
*/
extern void srand1( uint32_t seed );
err_t ethernetif_init(struct netif *netif)
{
  LWIP_ASSERT("netif != NULL", (netif != NULL));
  srand1(0x55aa);
#if LWIP_NETIF_HOSTNAME
  /* Initialize interface hostname */
  netif->hostname = "QILAI-lwip";
#endif /* LWIP_NETIF_HOSTNAME */

  netif->name[0] = IFNAME0;
  netif->name[1] = IFNAME1;

  netif->output = etharp_output;
  netif->linkoutput = low_level_output;

  /* initialize the hardware */
  low_level_init(netif);

  etharp_init();
  sys_timeout(ARP_TMR_INTERVAL, arp_timer, NULL);

  return ERR_OK;
}


static void arp_timer(void *arg)
{
  etharp_tmr();
  sys_timeout(ARP_TMR_INTERVAL, arp_timer, NULL);
}

#define RAND_LOCAL_MAX 2147483647L

static uint32_t next = 1;

int32_t rand1( void )
{
    return ( ( next = next * 1103515245L + 12345L ) % RAND_LOCAL_MAX );
}

void srand1( uint32_t seed )
{
    next = seed;
}
// Standard random functions redefinition end

int32_t randr( int32_t min, int32_t max )
{
    return ( int32_t )rand1( ) % ( max - min + 1 ) + min;
}
