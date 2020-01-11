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
#include "lwip/apps/httpd.h"
#include "ssi.h"
#include "cgi.h"
#include "uart_stream.h"
#include "lwip/tcpip.h"
#include "netif/ethernetif.h"
#include "pragma.h"
#include "contiki_list.h"

#define MY_MAX_VALUE(a, b)                                      ((a > b)?a:b)
#define MAX_TCP_CLIENT_LIST_ITEM                                (MEMP_NUM_TCP_PCB - 0)
#define MAC_TCP_CLIENT_ACTIVE                                   (MEMP_NUM_TCP_PCB - 2)
struct tcpclientlistitem
{
    struct tcpclientlistitem *next;
    int socket;
};
typedef struct tcpclientlistitem st_tcpclientlistitem;
extern struct netif xnetif;
xTaskHandle xTaskHandleNetWorkTask = 0;
LIST(tcp_client_idle_list_head);
LIST(tcp_client_active_list_head);
//LIST(tcp_client_close_list_head);
st_tcpclientlistitem tcpclientlistitem[MAX_TCP_CLIENT_LIST_ITEM];

unsigned char temp_buffer[NET_WORK_TASK_TEMP_BUFSIZE];

void tcpclient_list_init(void)
{
    list_init(tcp_client_idle_list_head);
    list_init(tcp_client_active_list_head);
    //list_init(tcp_client_close_list_head);
    for(uint8_t loop = 0; loop < MAX_TCP_CLIENT_LIST_ITEM; loop ++)
    {
        tcpclientlistitem[loop].socket = -1;
        list_add(tcp_client_idle_list_head, &tcpclientlistitem[loop]);
    }
}

static void net_work_task_thread(void *arg)
{
    struct dhcp *dhcp;
    ip_addr_t ipaddr;
    ip_addr_t netmask;
    ip_addr_t gw;
    size_t uartrecvbytes = 0;
    int sockfdsearchdev = 0;
    int server_sockfd, client_sockfd; 
    socklen_t server_len, client_len; 
    struct sockaddr_in server_address; 
    struct sockaddr_in client_address; 
    int result; 
    fd_set readfds, testfds;
    int sockfdudp = 0;
    int maxfd = -1;
    int re_flag=1;
    int re_len=sizeof(int);
    st_tcpclientlistitem *tcpclient;
    struct timeval timeout;
    int32_t keepalive = LWIP_TCP_KEEPALIVE;
    int32_t keepidle = TCP_KEEPIDLE_DEFAULT;
    int32_t keepinterval = TCP_KEEPINTVL_DEFAULT;
    int32_t keepcount = TCP_KEEPCNT_DEFAULT;
    timeout.tv_sec = 0;
    timeout.tv_usec = 1000;
    tcpclient_list_init();
    tcpip_init(NULL, NULL);
    LOCK_TCPIP_CORE();
    netif_add(&xnetif, &ipaddr, &netmask, &gw, NULL, ethernetif_init, tcpip_input);
    netif_set_default(&xnetif);
    netif_set_up(&xnetif);
    UNLOCK_TCPIP_CORE();

    LOCK_TCPIP_CORE();
    httpd_init();
    ssi_ex_init();
    cgi_ex_init();
    UNLOCK_TCPIP_CORE();

    while (!netif_is_link_up(&xnetif))
    {
        //WWDG_SetCounter(1);
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    LOCK_TCPIP_CORE();
    dhcp_stop(&xnetif);
    dhcp_start(&xnetif);
    UNLOCK_TCPIP_CORE();
    while (dhcp_supplied_address(&xnetif) == 0)
    {
        //WWDG_SetCounter(1);
        vTaskDelay(pdMS_TO_TICKS(10));
    }

    dhcp = netif_dhcp_data(&xnetif);
    printf("IP address: %s\n", ip4addr_ntoa(&dhcp->offered_ip_addr));
    printf("Subnet mask: %s\n", ip4addr_ntoa(&dhcp->offered_sn_mask));
    printf("Default gateway: %s\n", ip4addr_ntoa(&dhcp->offered_gw_addr));
    printf("task cnt = %d\r\n", uxTaskGetNumberOfTasks());

    /****************** UDP serch device socket*******************/
    if((sockfdsearchdev = socket(AF_INET,SOCK_DGRAM,IPPROTO_IP)) == -1)
    {
        printf("udp search device socket alloc failed\r\n");
        return;
    }
    //setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, &charOpt,sizeof(char));
    fcntl(sockfdsearchdev, F_SETFL, fcntl(sockfdsearchdev, F_GETFL, 0) | O_NONBLOCK);
    //setnonblocking(sockfdsearchdev);
    //bzero(&my_addr, sizeof(my_addr));
    server_address.sin_family=AF_INET;
    server_address.sin_port=htons(DEV_SEARCH_UDP_BIND_PORT);
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);


    memset(server_address.sin_zero,0,8);
    server_len = sizeof(server_address);
    re_flag=1;
    re_len=sizeof(int);
    setsockopt(sockfdsearchdev,SOL_SOCKET,SO_REUSEADDR,&re_flag,re_len);
    if(bind(sockfdsearchdev,(const struct sockaddr *)&server_address,server_len)==-1)
    {
        printf("error in binding");
        return;
    }
    printf("sockfdsearchdev = %d\r\n", sockfdsearchdev);
    switch(stWork_Pragma.workmode)
    {
        case MODULE_WORK_MODE_MJ_CLOUD:
          printf("mj cloud mode\r\n");
          break;
        case MODULE_WORK_MODE_TCP_SERVER:
          printf("tcp server mode\r\n");
          /****************** TCP server listen socket*******************/
          server_sockfd = socket(AF_INET, SOCK_STREAM, 0);//建立服务器端socket 
          printf("server_sockfd = %d\r\n", server_sockfd);
          server_address.sin_family = AF_INET; 
          server_address.sin_addr.s_addr = htonl(INADDR_ANY); 
          server_address.sin_port = htons(5001); 
          server_len = sizeof(server_address); 
          bind(server_sockfd, (struct sockaddr *)&server_address, server_len); 
          listen(server_sockfd, MAX_TCP_CLIENT_LIST_ITEM); //监听队列最多容纳个数
          FD_ZERO(&readfds); 
          FD_SET(server_sockfd, &readfds);//将服务器端socket加入到集合中
          maxfd = MY_MAX_VALUE(maxfd, server_sockfd);
          if(setsockopt(server_sockfd, SOL_SOCKET, SO_KEEPALIVE, (void *)&keepalive, sizeof(keepalive)) < 0)// 打开keepalive功能，需要在执行accept()之前执行
          {
            printf("set keepalive fail");
          }
          break;
        case MODULE_WORK_MODE_TCP_CLIENT:
          printf("tcp client mode\r\n");
          break;
        case MODULE_WORK_MODE_UDP:
          /****************** UDP server & client socket*******************/
          if((sockfdudp = socket(AF_INET,SOCK_DGRAM,IPPROTO_IP)) == -1)
          {
              printf("%s, %d\r\n", __func__, __LINE__);
              return;
          }
          //setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, &charOpt,sizeof(char));
          fcntl(sockfdudp, F_SETFL, fcntl(sockfdudp, F_GETFL, 0) | O_NONBLOCK);
          //setnonblocking(sockfdudp);
          //bzero(&server_address, sizeof(server_address));
          server_address.sin_family=AF_INET;
          server_address.sin_port=htons(stWork_Pragma.stLocal_IP.localport);
          server_address.sin_addr.s_addr = htonl(INADDR_ANY);


          memset(server_address.sin_zero,0,8);
          server_len = sizeof(server_address);
          re_flag=1;
          re_len=sizeof(int);
          setsockopt(sockfdudp,SOL_SOCKET,SO_REUSEADDR,&re_flag,re_len);
          if(bind(sockfdudp,(const struct sockaddr *)&server_address,server_len)==-1)
          {
              printf("error in binding");
              return;
          }
          break;
        case MODULE_WORK_MODE_HTTP_CLIENT:
          printf("http client mode\r\n");
          break;
        case MODULE_WORK_MODE_VSPD:
          printf("vspd mode\r\n");
          break;
        default:
          break;
    }
    uart_stream_create();
    while(netif_is_link_up(&xnetif))
    {
        int nread;
        uint8_t flag = 1;
        //WWDG_SetCounter(1);
        //printf("\r\n%d\r\n", xTaskGetTickCount());
        testfds = readfds;//将需要监视的描述符集copy到select查询队列中，select会对其修改，所以一定要分开使用变量 
        uartrecvbytes = uart_stream_bytes_get(temp_buffer);
        if(uartrecvbytes > 0)
        {
            for(tcpclient = list_head(tcp_client_active_list_head); tcpclient != NULL; tcpclient = list_item_next(tcpclient))
            {
                //WWDG_SetCounter(1);
                uartrecvbytes = send(tcpclient->socket, temp_buffer, uartrecvbytes, MSG_DONTWAIT);
                stModule_Info.txbytecnt += uartrecvbytes;
            }
        }
        /*无限期阻塞，并测试文件描述符变动 */
        result = select(maxfd + 1, &testfds, NULL, NULL, &timeout); //FD_SETSIZE：系统默认的最大文件描述符
        //WWDG_SetCounter(1);
        if(result < 1) 
        {
            continue;
        }
        /*扫描所有的文件描述符*/
        if(FD_ISSET(server_sockfd,&testfds))
        {
            client_len = sizeof(client_address);
            client_sockfd = accept(server_sockfd, 
            (struct sockaddr *)&client_address, &client_len); 
            
            FD_SET(client_sockfd, &readfds);//将客户端socket加入到集合中
            maxfd = MY_MAX_VALUE(maxfd, client_sockfd);
            tcpclient = list_pop(tcp_client_idle_list_head);
            tcpclient->socket = client_sockfd;
            list_add(tcp_client_active_list_head, tcpclient);
            setsockopt(client_sockfd, IPPROTO_TCP, TCP_KEEPIDLE, (void*)&keepidle , sizeof(keepidle));
            setsockopt(client_sockfd, IPPROTO_TCP, TCP_KEEPINTVL, (void *)&keepinterval , sizeof(keepinterval));
            setsockopt(client_sockfd, IPPROTO_TCP, TCP_KEEPCNT, (void *)&keepcount , sizeof(keepcount));
            setsockopt(client_sockfd, IPPROTO_TCP, TCP_NODELAY, (void *)&flag , sizeof(flag));
            //printf("adding client on fd %d\n", client_sockfd); 
            while(list_length(tcp_client_active_list_head) > MAC_TCP_CLIENT_ACTIVE)
            {
                tcpclient = list_pop(tcp_client_active_list_head);
                close(tcpclient->socket);
                FD_CLR(tcpclient->socket, &readfds); //去掉关闭的fd
                list_remove(tcp_client_active_list_head, tcpclient);
                list_add(tcp_client_idle_list_head, tcpclient);
            }
        }
        tcpclient = list_head(tcp_client_active_list_head);
        while(tcpclient != NULL)
        {
            //WWDG_SetCounter(1);
            //size_t space = 0;
            st_tcpclientlistitem *tcpclienttemp;
            /*找到相关文件描述符*/
            if(FD_ISSET(tcpclient->socket,&testfds)) 
            {
                /*taskENTER_CRITICAL();
                space = xStreamBufferSpacesAvailable(xStreamBufferUartTx);
                taskEXIT_CRITICAL();
                space = (space < NET_WORK_TASK_TEMP_BUFSIZE)?space:NET_WORK_TASK_TEMP_BUFSIZE;*/
                if((nread = recv(tcpclient->socket, temp_buffer, NET_WORK_TASK_TEMP_BUFSIZE, MSG_DONTWAIT)) > 0)//read(fd, temp_buffer, NET_WORK_TASK_TEMP_BUFSIZE); 
                //if(nread > 0)
                {
                    stModule_Info.rxbytecnt += nread;
                    //nread = send(tcpclient->socket, temp_buffer, nread, MSG_DONTWAIT);
                    nread = uart_stream_bytes_put_tx(temp_buffer, (size_t)nread);
                    tcpclient = list_item_next(tcpclient);
                }
                else
                {
                    close(tcpclient->socket);
                    FD_CLR(tcpclient->socket, &readfds); //去掉关闭的fd
                    tcpclienttemp = list_item_next(tcpclient);
                    list_remove(tcp_client_active_list_head, tcpclient);
                    list_add(tcp_client_idle_list_head, tcpclient);
                    tcpclient = tcpclienttemp;
                }
            }
            else
            {
                tcpclient = list_item_next(tcpclient);
            }
            
        }
        /*WWDG_SetCounter(1);
        while((uartrecvbytes = uart_stream_bytes_get(temp_buffer)) > 0)
        {
            uart_stream_bytes_put_tx(temp_buffer, (size_t)uartrecvbytes);
        }*/
    }
    printf("\r\nreboot now\r\n");
    //SYS_ResetExecute();
}

void net_work_task_init(void)
{
    xTaskCreate(net_work_task_thread, "netmain", NET_WORK_TASK_STK_SIZE , NULL, NET_WORK_TASK_THREAD_PRIO, &xTaskHandleNetWorkTask);
}