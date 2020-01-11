#include "lwip/mem.h"
#include "lwip/memp.h"
#include "lwip/dhcp.h"
#include "lwip/inet.h"
#include "ethernetif.h"
#include "netconf.h"
#include "lwip/tcpip.h"
#include <stdio.h>
#include "tcpclient.h"
#include "lwip/sockets.h"
#include "networktask.h"
#include "pragma.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define MAX_DHCP_TRIES 4

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
struct netif xnetif; /* network interface structure */
//uint8_t DHCP_state;
/* Private functions ---------------------------------------------------------*/

void ETH_link_callback(struct netif *netif)
{
    struct ip4_addr ipaddr;
    struct ip4_addr netmask;
    struct ip4_addr gw;
    //struct dhcp *dhcp;

    if(netif_is_link_up(netif))
    {
        //printf("ETH link up\r\n");
        ipaddr.addr = 0;
        netmask.addr = 0;
        gw.addr = 0;

        //DHCP_state = DHCP_START;
        LOCK_TCPIP_CORE();
        netif_set_addr(netif, &ipaddr , &netmask, &gw);
        UNLOCK_TCPIP_CORE();
        /* When the netif is fully configured this function must be called.*/
        LOCK_TCPIP_CORE();
        netif_set_up(netif);
        UNLOCK_TCPIP_CORE();
    }
    else
    {
        /*printf("%s, %d\r\n", __func__, __LINE__);
        LOCK_TCPIP_CORE();
        dhcp_stop(netif);
        UNLOCK_TCPIP_CORE();*/
        //printf("%s, %d\r\n", __func__, __LINE__);
        /*  When the netif link is down this function must be called.*/
        LOCK_TCPIP_CORE();
        netif_set_down(&xnetif);
        UNLOCK_TCPIP_CORE();
        //printf("ETH link down\r\n");
    }
    
}

/**
  * @brief  Initializes the lwIP stack
  * @param  None
  * @retval None
  */
void LwIP_Init(void)
{
    struct ip4_addr ipaddr;
    struct ip4_addr netmask;
    struct ip4_addr gw;

    /* Create tcp_ip stack thread */
    tcpip_init( NULL, NULL );

    /* IP address setting */

    if(stWork_Pragma.stLocal_IP.dhcp == PRAGMA_DHCP_ON)
    {
        ipaddr.addr = 0;
        netmask.addr = 0;
        gw.addr = 0;
    }
    else
    {
        IP4_ADDR(&ipaddr, IP_ADDR0, IP_ADDR1, IP_ADDR2, IP_ADDR3);
        IP4_ADDR(&netmask, NETMASK_ADDR0, NETMASK_ADDR1 , NETMASK_ADDR2, NETMASK_ADDR3);
        IP4_ADDR(&gw, GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3);
    }
  
    /* - netif_add(struct netif *netif, struct ip_addr *ipaddr,
    struct ip_addr *netmask, struct ip_addr *gw,
    void *state, err_t (* init)(struct netif *netif),
    err_t (* input)(struct pbuf *p, struct netif *netif))

    Adds your network interface to the netif_list. Allocate a struct
    netif and pass a pointer to this structure as the first argument.
    Give pointers to cleared ip_addr structures when using DHCP,
    or fill them with sane numbers otherwise. The state pointer may be NULL.

    The init function pointer must point to a initialization function for
    your ethernet netif interface. The following code illustrates it's use.*/
    LOCK_TCPIP_CORE();
    netif_add(&xnetif, &ipaddr, &netmask, &gw, NULL, &ethernetif_init, &tcpip_input);

    /*  Registers the default network interface.*/
    netif_set_default(&xnetif);
    /* Set the link callback function, this function is called on change of link status*/
    //netif_set_link_callback(&xnetif, ETH_link_callback);
    netif_set_up(&xnetif);
    printf("%s, %d\r\n", __func__, __LINE__);
    UNLOCK_TCPIP_CORE();
}
#if 0
/**
  * @brief  LwIP_DHCP_Process_Handle
  * @param  None
  * @retval None
  */
//#define netif_dhcp_data(netif) ((struct dhcp*)netif_get_client_data(netif, LWIP_NETIF_CLIENT_DATA_INDEX_DHCP))
void LwIP_DHCP_task(void * pvParameters)
{
  struct dhcp *dhcp;
  
  while(DHCP_state != DHCP_START)
  {
      vTaskDelay(250);
  }
  printf("DHCP_START\r\n");

  dhcp_start(&xnetif);
  //autoip_start(&xnetif);
  while (dhcp_supplied_address(&xnetif) == 0)
  {
      /* wait 250 ms */
      //printf("wait!\r\n");
      vTaskDelay(1000);
  }
  //printf("Got\r\n");
  dhcp = netif_dhcp_data(&xnetif);
  printf("IP address: %s\n", ip4addr_ntoa(&dhcp->offered_ip_addr));
  printf("Subnet mask: %s\n", ip4addr_ntoa(&dhcp->offered_sn_mask));
  printf("Default gateway: %s\n", ip4addr_ntoa(&dhcp->offered_gw_addr));
  vTaskDelay(pdMS_TO_TICKS(1000));
  //dhcp_stop(&xnetif);
  netif_set_default(&xnetif);
  
  netif_set_up(&xnetif);
  //netif_add(&xnetif, &dhcp->offered_ip_addr, &dhcp->offered_sn_mask, &dhcp->offered_gw_addr, NULL, &ethernetif_init, &tcpip_input);
  /*  Registers the default network interface.*/
  //netif_set_default(&xnetif);
  //netif_set_up(&xnetif);
  //printf("%s, %d\r\n", __func__, __LINE__);
  tcp_client_init();
  //net_work_task_init();
  //printf("%s, %d\r\n", __func__, __LINE__);
  //tcp_server_init();
  
  while(1)
  {
      //prvDisplaySystmInfo();
      //printf("%s, %d\r\n", __func__, __LINE__);
      vTaskDelay(pdMS_TO_TICKS(1000));
  }
}
#endif
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
