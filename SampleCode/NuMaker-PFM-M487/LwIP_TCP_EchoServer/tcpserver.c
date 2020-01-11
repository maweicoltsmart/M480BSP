#include "lwip/opt.h"
#include "lwip/arch.h"
#include "lwip/api.h"
#include "lwip/inet.h"
#include "lwip/sockets.h"
#include "string.h"
#include "FreeRTOS.h"
#include "task.h"
#include "tcpserver.h"

#define TCP_SERVER_PORT                 5000

static const uint8_t tcp_server_sendbuf[] = "Welcome to connect to the LWIP server.\r\n";    
unsigned char recv_buffer[TCP_SERVER_RX_BUFSIZE];
void tcp_server_serve(int conn) 
{
  int ret;
  struct fs_file * file;
  

  while(1)
  {
      memset(recv_buffer, 0, TCP_SERVER_RX_BUFSIZE);
      /* Read in the request */
      ret = read(conn, recv_buffer, TCP_SERVER_RX_BUFSIZE - 1); 
      if(ret < 0) 
      {
          close(conn);
          return;
      }

      write(conn, recv_buffer, strlen(recv_buffer));
  }
  close(conn);
}
static void tcp_server_thread(void *arg)
{
  int sock, newconn, size;
  struct sockaddr_in address, remotehost;

 /* create a TCP socket */
  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
  {
    printf("can not create socket");
    return;
  }
  
  /* bind to port 80 at any interface */
  address.sin_family = AF_INET;
  address.sin_port = htons(TCP_SERVER_PORT);
  address.sin_addr.s_addr = INADDR_ANY;

  if (bind(sock, (struct sockaddr *)&address, sizeof (address)) < 0)
  {
    printf("can not bind socket");
    return;
  }

  /* listen for incoming connections (TCP listen backlog = 5) */
  listen(sock, 5);

  while (1) 
  {
    newconn = accept(sock, (struct sockaddr *)&remotehost, (socklen_t *)&size);
    
    printf("%s:%d\r\n",inet_ntoa(remotehost.sin_addr.s_addr), ntohs(address.sin_port));
    tcp_server_serve(newconn);
  }
}


void tcp_server_init(void)
{
    sys_thread_new("TCP SERVER", tcp_server_thread, NULL, TCPSERVER_STK_SIZE, TCPSERVER_THREAD_PRIO);

}