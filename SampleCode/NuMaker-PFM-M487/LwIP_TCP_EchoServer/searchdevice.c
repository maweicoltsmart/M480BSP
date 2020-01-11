#include "string.h"
#include "lwip/opt.h"
#include "lwip/arch.h"
#include "lwip/api.h"
#include "lwip/inet.h"
#include "lwip/sockets.h"
#include "lwip/dhcp.h"
#include "string.h"
#include "FreeRTOS.h"
#include "task.h"
#include "netconf.h"
#include <stdio.h>
#include "networktask.h"
#include "uart_stream.h"
#include "searchdevice.h"
#include "cJSON.h"
#include "pragma.h"
#include "hex.h"
#include "minijson.h"
#include "crc32.h"
//#include "m480sys.h"

static int sockfdsearchdev = 0;

//非阻塞
void setnonblocking(int sockfd)
{
    int flag = fcntl(sockfd, F_GETFL, 0);

    if (flag < 0) {
        printf("fcntl F_GETFL fail");
        return;
    }
    if (fcntl(sockfd, F_SETFL, flag | O_NONBLOCK) < 0) {
        printf("fcntl F_SETFL fail");
    }
}

void search_device_init(void)
{
    struct sockaddr_in my_addr;
    socklen_t addr_len;

    if((sockfdsearchdev = socket(AF_INET,SOCK_DGRAM,IPPROTO_IP)) == -1)
    {
        printf("%s, %d\r\n", __func__, __LINE__);
        return;
    }
    //setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, &charOpt,sizeof(char));
    fcntl(sockfdsearchdev, F_SETFL, fcntl(sockfdsearchdev, F_GETFL, 0) | O_NONBLOCK);
    //setnonblocking(sockfdsearchdev);
    //bzero(&my_addr, sizeof(my_addr));
    my_addr.sin_family=AF_INET;
    my_addr.sin_port=htons(DEV_SEARCH_UDP_BIND_PORT);
    my_addr.sin_addr.s_addr = htonl(INADDR_ANY);


    memset(my_addr.sin_zero,0,8);
    addr_len = sizeof(my_addr);
    int re_flag=1;
    int re_len=sizeof(int);
    setsockopt(sockfdsearchdev,SOL_SOCKET,SO_REUSEADDR,&re_flag,re_len);
    if(bind(sockfdsearchdev,(const struct sockaddr *)&my_addr,addr_len)==-1)
    {
        printf("error in binding");
        return;
    }
}

void search_device_check(void)
{
    int ret = 0;
    struct sockaddr_in addr;
    socklen_t addr_len;

    addr_len = sizeof(addr);
    memset(temp_buffer, 0, sizeof(temp_buffer));
    if((ret = recvfrom(sockfdsearchdev,temp_buffer,NET_WORK_TASK_TEMP_BUFSIZE - 1,MSG_DONTWAIT,(struct sockaddr *)&addr,&addr_len)) > 0)
    {
        //printf("%s, %d, %d\r\n", __func__, __LINE__, ret);
        temp_buffer[ret] = 0x00;
        if(strstr((char const*)temp_buffer, "search coltsmart com device!"))
        {
            memset(temp_buffer, 0, sizeof(temp_buffer));
            sprintf((char*)temp_buffer + strlen((char const*)temp_buffer), "%s", "{");
            uint8_t devidstr[17];
            memset(devidstr, 0, sizeof(devidstr));
            puthex(devidstr, stDevice_Info.devid, 8);
            devidstr[sizeof(devidstr) - 1] = '\0';
            sprintf((char*)temp_buffer + strlen((char const*)temp_buffer), "%s%s%s%s", "\"DeviceID\":", "\"", (char const*)devidstr, "\"");
            sprintf((char*)temp_buffer + strlen((char const*)temp_buffer), "%s", ",");
            memset(devidstr, 0, sizeof(devidstr));
            puthex(devidstr, stDevice_Info.macaddr, 6);
            devidstr[12] = '\0';
            sprintf((char*)temp_buffer + strlen((char const*)temp_buffer), "%s%s%s%s", "\"DeviceMAC\":", "\"", (char const*)devidstr, "\"");
            sprintf((char*)temp_buffer + strlen((char const*)temp_buffer), "%s", ",");
            sprintf((char*)temp_buffer + strlen((char const*)temp_buffer), "%s%s%s%s", "\"DeviceName\":", "\"", (char const*)stWork_Pragma.devname, "\"");
            sprintf((char*)temp_buffer + strlen((char const*)temp_buffer), "%s", ",");
            sprintf((char*)temp_buffer + strlen((char const*)temp_buffer), "%s%s%s%s", "\"DeviceType\":", "\"", (char const*)stDevice_Info.devtype, "\"");
            sprintf((char*)temp_buffer + strlen((char const*)temp_buffer), "%s", ",");
            sprintf((char*)temp_buffer + strlen((char const*)temp_buffer), "%s%s%s%s", "\"HardWareVersion\":", "\"", HARDWARE_VERSION, "\"");
            sprintf((char*)temp_buffer + strlen((char const*)temp_buffer), "%s", ",");
            sprintf((char*)temp_buffer + strlen((char const*)temp_buffer), "%s%s%s%s", "\"SoftWareVersion\":", "\"", SOFTWARE_VERSION, "\"");
            sprintf((char*)temp_buffer + strlen((char const*)temp_buffer), "%s", ",");
            sprintf((char*)temp_buffer + strlen((char const*)temp_buffer), "%s%s%s%s", "\"WorkMode\":", "\"", strworkmode[stWork_Pragma.workmode], "\"");
            sprintf((char*)temp_buffer + strlen((char const*)temp_buffer), "%s", ",");
            sprintf((char*)temp_buffer + strlen((char const*)temp_buffer), "%s%s", "\"EnableDHCP\":", (stWork_Pragma.stLocal_IP.dhcp == PRAGMA_DHCP_ON)?"true":"false");
            sprintf((char*)temp_buffer + strlen((char const*)temp_buffer), "%s", ",");
            sprintf((char*)temp_buffer + strlen((char const*)temp_buffer), "%s%s%s%s", "\"LocalIP\":", "\"", ip4addr_ntoa(&stWork_Pragma.stLocal_IP.localip), "\"");
            sprintf((char*)temp_buffer + strlen((char const*)temp_buffer), "%s", ",");
            sprintf((char*)temp_buffer + strlen((char const*)temp_buffer), "%s%d", "\"LocalPort\":", stWork_Pragma.stLocal_IP.localport);
            sprintf((char*)temp_buffer + strlen((char const*)temp_buffer), "%s", ",");
            sprintf((char*)temp_buffer + strlen((char const*)temp_buffer), "%s%s%s%s", "\"NetMask\":", "\"", ip4addr_ntoa(&stWork_Pragma.stLocal_IP.netmask), "\"");
            sprintf((char*)temp_buffer + strlen((char const*)temp_buffer), "%s", ",");
            sprintf((char*)temp_buffer + strlen((char const*)temp_buffer), "%s%s%s%s", "\"GateWay\":","\"", ip4addr_ntoa(&stWork_Pragma.stLocal_IP.gateway), "\"");
            sprintf((char*)temp_buffer + strlen((char const*)temp_buffer), "%s", ",");
            sprintf((char*)temp_buffer + strlen((char const*)temp_buffer), "%s%s%s%s", "\"DNS\":", "\"", ip4addr_ntoa(&stWork_Pragma.stLocal_IP.dns), "\"");
            sprintf((char*)temp_buffer + strlen((char const*)temp_buffer), "%s", ",");
            sprintf((char*)temp_buffer + strlen((char const*)temp_buffer), "%s%s%s%s", "\"RemoteHost\":", "\"", (char const*)stWork_Pragma.stRemote_IP.hostname, "\"");
            sprintf((char*)temp_buffer + strlen((char const*)temp_buffer), "%s", ",");
            sprintf((char*)temp_buffer + strlen((char const*)temp_buffer), "%s%d", "\"RemotePort\":",stWork_Pragma.stRemote_IP.remoteport);
            sprintf((char*)temp_buffer + strlen((char const*)temp_buffer), "%s", ",");
            sprintf((char*)temp_buffer + strlen((char const*)temp_buffer), "%s%d", "\"UartBaud\":",stWork_Pragma.stUart_Pragma.baud);
            sprintf((char*)temp_buffer + strlen((char const*)temp_buffer), "%s", ",");
            sprintf((char*)temp_buffer + strlen((char const*)temp_buffer), "%s%d", "\"UartDataBit\":",stWork_Pragma.stUart_Pragma.databit);
            sprintf((char*)temp_buffer + strlen((char const*)temp_buffer), "%s", ",");
            sprintf((char*)temp_buffer + strlen((char const*)temp_buffer), "%s%s%s%s", "\"UartStopBit\":", "\"", strstopbit[stWork_Pragma.stUart_Pragma.stopbit], "\"");
            sprintf((char*)temp_buffer + strlen((char const*)temp_buffer), "%s", ",");
            sprintf((char*)temp_buffer + strlen((char const*)temp_buffer), "%s%s%s%s", "\"UartParity\":", "\"", strparity[stWork_Pragma.stUart_Pragma.parity], "\"");
            sprintf((char*)temp_buffer + strlen((char const*)temp_buffer), "%s", ",");
            sprintf((char*)temp_buffer + strlen((char const*)temp_buffer), "%s%s%s%s", "\"UartFlowctrl\":", "\"", strflowctrl[stWork_Pragma.stUart_Pragma.flowctrl], "\"");
            sprintf((char*)temp_buffer + strlen((char const*)temp_buffer), "%s", "}");
            addr.sin_port=htons(DEV_SEARCH_UDP_SEND_PORT);
            //printf("%s, %d, %d\r\n", __func__, __LINE__, strlen((char const*)temp_buffer));
            sendto(sockfdsearchdev, temp_buffer, strlen((char const*)temp_buffer), MSG_DONTWAIT, (struct sockaddr *)&addr, addr_len);
        }
        else if(strstr((char const*)temp_buffer, "factory reset coltsmart com device!"))
        {
            factoryresetworkpragma();
        }
        else if(strstr((char const*)temp_buffer, "reset coltsmart com device now!"))
        {
            //SYS_ResetChip();
        }
        else
        {
            /*cJSON *json;
            cJSON *item;
            json=cJSON_Parse((char const *)temp_buffer);
            item = cJSON_GetObjectItem(json,"DeviceID");
            printf("DeviceID:%s\n",item->valuestring);
            item = cJSON_GetObjectItem(json,"DeviceMAC");
            printf("DeviceMAC:%s\n",item->valuestring);
            item = cJSON_GetObjectItem(json,"DeviceName");
            printf("DeviceName:%s\n",item->valuestring);
            item = cJSON_GetObjectItem(json,"DeviceType");
            printf("DeviceType:%s\n",item->valuestring);
            item = cJSON_GetObjectItem(json,"HardWareVersion");
            printf("HardWareVersion:%s\n",item->valuestring);
            item = cJSON_GetObjectItem(json,"SoftWareVersion");
            printf("SoftWareVersion:%s\n",item->valuestring);
            item = cJSON_GetObjectItem(json,"WorkMode");
            printf("WorkMode:%s\n",item->valuestring);
            item = cJSON_GetObjectItem(json,"EnableDHCP");
            printf("EnableDHCP:%s\n",item->valuestring);
            item = cJSON_GetObjectItem(json,"LocalIP");
            printf("LocalIP:%s\n",item->valuestring);
            item = cJSON_GetObjectItem(json,"LocalPort");
            printf("LocalPort:%s\n",item->valuestring);
            item = cJSON_GetObjectItem(json,"NetMask");
            printf("NetMask:%s\n",item->valuestring);
            item = cJSON_GetObjectItem(json,"GateWay");
            printf("GateWay:%s\n",item->valuestring);
            item = cJSON_GetObjectItem(json,"DNS");
            printf("DNS:%s\n",item->valuestring);
            item = cJSON_GetObjectItem(json,"RemoteHost");
            printf("RemoteHost:%s\n",item->valuestring);
            item = cJSON_GetObjectItem(json,"RemotePort");
            printf("RemotePort:%s\n",item->valuestring);
            item = cJSON_GetObjectItem(json,"UartBaud");
            printf("UartBaud:%s\n",item->valuestring);
            item = cJSON_GetObjectItem(json,"UartDataBit");
            printf("UartDataBit:%s\n",item->valuestring);
            item = cJSON_GetObjectItem(json,"UartStopBit");
            printf("UartStopBit:%s\n",item->valuestring);
            item = cJSON_GetObjectItem(json,"UartParity");
            printf("UartParity:%s\n",item->valuestring);
            item = cJSON_GetObjectItem(json,"UartFlowctrl");
            printf("UartFlowctrl:%s\n",item->valuestring);
            cJSON_Delete(json);*/
            cJSON_Minify((char *)temp_buffer);
            if(json_check((char const*)temp_buffer) != 1)
            {
                printf("invalid json string\r\n");
                return;
            }
            char devidstr[HOST_NAME_MAX_LEN];
            memset(devidstr, 0, sizeof(devidstr));
            if(json_get_value((char const*)temp_buffer,"DeviceID",devidstr, sizeof(devidstr)) == 1)
            {
                printf("DeviceID: %s\r\n", devidstr);
                gethex((uint8_t *)devidstr, (uint8_t const *)devidstr, 16);
                if(memcmp(devidstr, stDevice_Info.devid, 8))
                {
                    printf("DeviceID is not me\r\n");
                }
            }
            memset(devidstr, 0, sizeof(devidstr));
            if(json_get_value((char const*)temp_buffer,"WorkMode",devidstr, sizeof(devidstr)) == 1)
            {
                printf("WorkMode: %s\r\n", devidstr);
                if(!strcmp(devidstr,(char const *)strworkmode[MODULE_WORK_MODE_MJ_CLOUD]))
                {
                    stWork_Pragma.workmode = MODULE_WORK_MODE_MJ_CLOUD;
                }
                else if(!strcmp(devidstr,(char const *)strworkmode[MODULE_WORK_MODE_TCP_SERVER]))
                {
                    stWork_Pragma.workmode = MODULE_WORK_MODE_TCP_SERVER;
                }
                else if(!strcmp(devidstr,(char const *)strworkmode[MODULE_WORK_MODE_TCP_CLIENT]))
                {
                    stWork_Pragma.workmode = MODULE_WORK_MODE_TCP_CLIENT;
                }
                else if(!strcmp(devidstr,(char const *)strworkmode[MODULE_WORK_MODE_UDP]))
                {
                    stWork_Pragma.workmode = MODULE_WORK_MODE_UDP;
                }
                else if(!strcmp(devidstr,(char const *)strworkmode[MODULE_WORK_MODE_HTTP_CLIENT]))
                {
                    stWork_Pragma.workmode = MODULE_WORK_MODE_HTTP_CLIENT;
                }
                else if(!strcmp(devidstr,(char const *)strworkmode[MODULE_WORK_MODE_VSPD]))
                {
                    stWork_Pragma.workmode = MODULE_WORK_MODE_VSPD;
                }
            }
            memset(devidstr, 0, sizeof(devidstr));
            if(json_get_value((char const*)temp_buffer,"EnableDHCP",devidstr, sizeof(devidstr)) == 1)
            {
                printf("EnableDHCP: %s\r\n", devidstr);
                (!strcmp(devidstr, "true"))?(stWork_Pragma.stLocal_IP.dhcp = PRAGMA_DHCP_ON):(stWork_Pragma.stLocal_IP.dhcp = PRAGMA_DHCP_OFF);
            }
            memset(devidstr, 0, sizeof(devidstr));
            if(json_get_value((char const*)temp_buffer,"LocalIP",devidstr, sizeof(devidstr)) == 1)
            {
                printf("LocalIP: %s\r\n", devidstr);
                ip4addr_aton(devidstr, &stWork_Pragma.stLocal_IP.localip);
            }
            memset(devidstr, 0, sizeof(devidstr));
            if(json_get_value((char const*)temp_buffer,"LocalPort",devidstr, sizeof(devidstr)) == 1)
            {
                printf("LocalPort: %s\r\n", devidstr);
                sscanf( devidstr, "%hud", &stWork_Pragma.stLocal_IP.localport );
            }
            memset(devidstr, 0, sizeof(devidstr));
            if(json_get_value((char const*)temp_buffer,"NetMask",devidstr, sizeof(devidstr)) == 1)
            {
                printf("NetMask: %s\r\n", devidstr);
                ip4addr_aton(devidstr, &stWork_Pragma.stLocal_IP.netmask);
            }
            memset(devidstr, 0, sizeof(devidstr));
            if(json_get_value((char const*)temp_buffer,"GateWay",devidstr, sizeof(devidstr)) == 1)
            {
                printf("GateWay: %s\r\n", devidstr);
                ip4addr_aton(devidstr, &stWork_Pragma.stLocal_IP.gateway);
            }
            memset(devidstr, 0, sizeof(devidstr));
            if(json_get_value((char const*)temp_buffer,"DNS",devidstr, sizeof(devidstr)) == 1)
            {
                printf("DNS: %s\r\n", devidstr);
                ip4addr_aton(devidstr,&stWork_Pragma.stLocal_IP.dns);
            }
            memset(devidstr, 0, sizeof(devidstr));
            if(json_get_value((char const*)temp_buffer,"RemoteHost",devidstr, sizeof(devidstr)) == 1)
            {
                printf("RemoteHost: %s\r\n", devidstr);
                strcpy((char*)stWork_Pragma.stRemote_IP.hostname, devidstr);
            }
            memset(devidstr, 0, sizeof(devidstr));
            if(json_get_value((char const*)temp_buffer,"RemotePort",devidstr, sizeof(devidstr)) == 1)
            {
                printf("RemotePort: %s\r\n", devidstr);
                sscanf( devidstr, "%hud", &stWork_Pragma.stRemote_IP.remoteport );
            }
            memset(devidstr, 0, sizeof(devidstr));
            if(json_get_value((char const*)temp_buffer,"UartBaud",devidstr, sizeof(devidstr)) == 1)
            {
                printf("UartBaud: %s\r\n", devidstr);
                sscanf( devidstr, "%ud", &stWork_Pragma.stUart_Pragma.baud );
            }
            memset(devidstr, 0, sizeof(devidstr));
            if(json_get_value((char const*)temp_buffer,"UartDataBit",devidstr, sizeof(devidstr)) == 1)
            {
                printf("UartDataBit: %s\r\n", devidstr);
                stWork_Pragma.stUart_Pragma.databit = ((devidstr[0] > 0x36) && (devidstr[0] < 0x3a))?(devidstr[0] - 0x30):8;
            }
            memset(devidstr, 0, sizeof(devidstr));
            if(json_get_value((char const*)temp_buffer,"UartStopBit",devidstr, sizeof(devidstr)) == 1)
            {
                printf("UartStopBit: %s\r\n", devidstr);
                if(!strcmp(devidstr, (char const*)strstopbit[PRAGMA_UART_STOP_BIT_1]))
                {
                  stWork_Pragma.stUart_Pragma.stopbit = PRAGMA_UART_STOP_BIT_1;
                }
                else if(!strcmp(devidstr, (char const*)strstopbit[PRAGMA_UART_STOP_BIT_1_5]))
                {
                  stWork_Pragma.stUart_Pragma.stopbit = PRAGMA_UART_STOP_BIT_1_5;
                }
                else if(!strcmp(devidstr, (char const*)strstopbit[PRAGMA_UART_STOP_BIT_2]))
                {
                  stWork_Pragma.stUart_Pragma.stopbit = PRAGMA_UART_STOP_BIT_2;
                }
            }
            memset(devidstr, 0, sizeof(devidstr));
            if(json_get_value((char const*)temp_buffer,"UartParity",devidstr, sizeof(devidstr)) == 1)
            {
                printf("UartParity: %s\r\n", devidstr);
                if(!strcmp(devidstr, (char const*)strparity[PRAGMA_UART_PARITY_NONE]))
                {
                  stWork_Pragma.stUart_Pragma.parity = PRAGMA_UART_PARITY_NONE;
                }
                else if(!strcmp(devidstr, (char const*)strparity[PRAGMA_UART_PARITY_ODD]))
                {
                  stWork_Pragma.stUart_Pragma.parity = PRAGMA_UART_PARITY_ODD;
                }
                else if(!strcmp(devidstr, (char const*)strparity[PRAGMA_UART_PARITY_EVEN]))
                {
                  stWork_Pragma.stUart_Pragma.parity = PRAGMA_UART_PARITY_EVEN;
                }
                else if(!strcmp(devidstr, (char const*)strparity[PRAGMA_UART_PARITY_MARK]))
                {
                  stWork_Pragma.stUart_Pragma.parity = PRAGMA_UART_PARITY_MARK;
                }
                else if(!strcmp(devidstr, (char const*)strparity[PRAGMA_UART_PARITY_SPACE]))
                {
                  stWork_Pragma.stUart_Pragma.parity = PRAGMA_UART_PARITY_SPACE;
                }
            }
            memset(devidstr, 0, sizeof(devidstr));
            if(json_get_value((char const*)temp_buffer,"UartFlowctrl",devidstr, sizeof(devidstr)) == 1)
            {
                printf("UartFlowctrl: %s\r\n", devidstr);
                if(!strcmp(devidstr, (char const*)strflowctrl[PRAGMA_UART_FLOWCTRL_NONE]))
                {
                  stWork_Pragma.stUart_Pragma.flowctrl = PRAGMA_UART_FLOWCTRL_NONE;
                }
                else if(!strcmp(devidstr, (char const*)strflowctrl[PRAGMA_UART_FLOWCTRL_HARDWARE]))
                {
                  stWork_Pragma.stUart_Pragma.flowctrl = PRAGMA_UART_FLOWCTRL_HARDWARE;
                }
                else if(!strcmp(devidstr, (char const*)strflowctrl[PRAGMA_UART_FLOWCTRL_XONOFF]))
                {
                  stWork_Pragma.stUart_Pragma.flowctrl = PRAGMA_UART_FLOWCTRL_XONOFF;
                }
            }
            stWork_Pragma.crc32 = crc32((const char*)&stWork_Pragma, sizeof(stWork_Pragma) - 4);
            restoreworkpragma();
        }
    }
}