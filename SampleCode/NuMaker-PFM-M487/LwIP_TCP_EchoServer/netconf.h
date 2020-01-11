/**
  ******************************************************************************
  * @file    netconf.h
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    31-July-2013 
  * @brief   This file contains all the functions prototypes for the netconf.c 
  *          file.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2013 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __NETCONF_H
#define __NETCONF_H

#ifdef __cplusplus
 extern "C" {
#endif
   
/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* MAC ADDRESS*/
//#define MAC_ADDR0   0x84
//#define MAC_ADDR1   0xc2
//#define MAC_ADDR2   0xe4
//#define MAC_ADDR3   0x02
//#define MAC_ADDR4   0x03
//#define MAC_ADDR5   0x04
 
/*Static IP ADDRESS*/
#define IP_ADDR0   192
#define IP_ADDR1   168
#define IP_ADDR2   3
#define IP_ADDR3   203
   
/*NETMASK*/
#define NETMASK_ADDR0   255
#define NETMASK_ADDR1   255
#define NETMASK_ADDR2   255
#define NETMASK_ADDR3   0

/*Gateway Address*/
#define GW_ADDR0   192
#define GW_ADDR1   168
#define GW_ADDR2   3
#define GW_ADDR3   1  


#define DHCP_START                 1
#define DHCP_WAIT_ADDRESS          2
#define DHCP_ADDRESS_ASSIGNED      3
#define DHCP_TIMEOUT               4
#define DHCP_LINK_DOWN             5
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
extern struct netif xnetif; /* network interface structure */

void LwIP_Init(void);
void LwIP_DHCP_task(void * pvParameters);
void prvDisplaySystmInfo(void);
#ifdef __cplusplus
}
#endif

#endif /* __NETCONF_H */


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
