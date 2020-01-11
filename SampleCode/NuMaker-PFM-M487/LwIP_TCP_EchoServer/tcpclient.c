#include "lwip/opt.h"
#include "lwip/arch.h"
#include "lwip/api.h"
#include "lwip/inet.h"
#include "lwip/sockets.h"
#include "string.h"
#include "FreeRTOS.h"
#include "task.h"
#include "tcpclient.h"
#include "uart_stream.h"
#include "netconf.h"
#include <stdio.h>
#include "mqtt_example.h"

static int sockfd = 0;
xTaskHandle xTaskHandleTcpClient = 0;
xTaskHandle xTaskHandleUartIn = 0;

static unsigned char recv_buffer[TCP_CLIENT_RX_BUFSIZE];
static unsigned char uart_recv_buffer[UART_STREAM_RX_BUFSIZE];

static void uart_data_in_thread(void *arg)
{
    int ret = 0;
    size_t xReceivedBytes;
    int index = 0;
    static long long len = 0;
    const TickType_t xBlockTime = pdMS_TO_TICKS( 100 );

    while(1)
    {
        xReceivedBytes = xStreamBufferReceive( xStreamBufferUartRx,
                                               ( void * ) uart_recv_buffer,
                                               UART_STREAM_RX_BUFSIZE,
                                               xBlockTime );
        //printf("%s, %d\r\n", __func__, __LINE__);
        if(xReceivedBytes > 0)
        {
            //printf("%s, %d\r\n", __func__, __LINE__);
            len += xReceivedBytes;
            //printf("tx:%lld\r\n", len);
            index = 0;
            ret = send(sockfd, &uart_recv_buffer, xReceivedBytes, 0);
#if 0
            while(index < (xReceivedBytes))
            {
                ret = send(sockfd, &uart_recv_buffer[index], xReceivedBytes - index, 0);
                if(ret <= 0)
                {
                    printf("%s, %d\r\n", __func__, __LINE__);
                    while(1);
                    vTaskDelete(NULL);
                }
                else
                {
                    index += ret;
                }
                vTaskDelay(10);
            }
#endif
        }
    }
}

static void tcp_client_thread(void *arg)
{
    int ret = 0;
    struct sockaddr_in their_addr;
    static long long len = 0;
    //const char charOpt = 1;
    const TickType_t xBlockTime = pdMS_TO_TICKS( 10 );
    size_t xReceivedBytes;
    const int32_t keepalive = 1;
    const int32_t keepidle = 2;
    const int32_t keepinterval = 2;
    const int32_t keepcount = 2;
    //const int32_t flag = 1;

    //mqtt_example_init();
connect:
    printf("connectting\r\n");
    while((sockfd = socket(AF_INET,SOCK_STREAM,0)) == -1)vTaskDelay(100);;
    //printf("tcp client socket got\r\n");
    their_addr.sin_family = AF_INET;
    their_addr.sin_port = htons(DEFAULT_SERVER_PORT);
    their_addr.sin_addr.s_addr=inet_addr(DEFAULT_SERVER_IP);
    memset(&(their_addr.sin_zero), 0, 8);
    /*setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, &charOpt,sizeof(char));
    if(setsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE, (void *)&keepalive, sizeof(keepalive)) < 0)// ´ò¿ªkeepalive¹¦ÄÜ£¬ÐèÒªÔÚÖ´ÐÐconnect()Ö®Ç°Ö´ÐÐ
    {
        printf("set keepalive fail");
    }*/
    while(connect(sockfd,(struct sockaddr*)&their_addr,sizeof(struct sockaddr)) != 0)
    {
        vTaskDelay(pdMS_TO_TICKS( 100 ));
        printf("connectting\r\n");
    }
    /*setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPIDLE, (void*)&keepidle , sizeof(keepidle));
    setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPINTVL, (void *)&keepinterval , sizeof(keepinterval));
    setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPCNT, (void *)&keepcount , sizeof(keepcount));*/
    //setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, (void *)&flag , sizeof(flag));
    printf("Get the Server~Cheers!\n");
    //xTaskCreate(uart_data_in_thread, "Uart Stream In", 128 * 1 , NULL, 1, &xTaskHandleUartIn);
    //printf("%s, %d\r\n", __func__, __LINE__);
    while(1)
    {
        //prvDisplaySystmInfo();
        //printf("%s, %d\r\n", __func__, __LINE__);
        xReceivedBytes = xStreamBufferReceive( xStreamBufferUartRx,
                                               ( void * ) uart_recv_buffer,
                                               UART_STREAM_RX_BUFSIZE,
                                               xBlockTime );
        //printf("%s, %d\r\n", __func__, __LINE__);
        if(xReceivedBytes > 0)
        {
            //printf("%s, %d\r\n", __func__, __LINE__);
            //len += xReceivedBytes;
            //printf("tx:%lld\r\n", len);
            //index = 0;
            ret = send(sockfd, &uart_recv_buffer, xReceivedBytes, MSG_DONTWAIT);
            if(ret < 0)
            {
                printf("%s, %d\r\n", __func__, __LINE__);
                goto error;
            }
        }
        while((ret = recv(sockfd, recv_buffer, TCP_CLIENT_RX_BUFSIZE, MSG_DONTWAIT)) > 0)
        {
            //len += ret;
            //printf("rx:%lld\r\n", len);
            for(uint16_t loop = 0; loop < ret; loop ++)
            {
                putchar(recv_buffer[loop]);
            }
        }
        if(ret == 0)
        {
            printf("%s, %d\r\n", __func__, __LINE__);
            goto error;
        }
    }
error:
    printf("%s, %d\r\n", __func__, __LINE__);
    //while(1);
    //vTaskDelete(xTaskHandleUartIn);
    shutdown(sockfd, SHUT_RDWR);
    close(sockfd);
    goto connect;
    return;
}

void tcp_client_init(void)
{
    xTaskCreate(tcp_client_thread, "TCP CLIENT", TCP_CLIENT_STK_SIZE , NULL, TCP_CLIENT_THREAD_PRIO, &xTaskHandleTcpClient);
}