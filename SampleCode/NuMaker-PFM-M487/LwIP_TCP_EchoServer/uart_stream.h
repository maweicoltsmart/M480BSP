#ifndef __UART_STREAM_H__
#define __UART_STREAM_H__
#include "stream_buffer.h"
#include "semphr.h"

#define MAX_UART_RECV_LIST_ITEM                                         50

/* The definition of the frame tx & rx list. */
struct uartrecvlistitem
{
    struct uartrecvlistitem *next;
    uint16_t cnt;
};
typedef struct uartrecvlistitem st_uartrecvlistitem;


extern StreamBufferHandle_t xStreamBufferUartRx;
extern StreamBufferHandle_t xStreamBufferUartTx;
extern xSemaphoreHandle s_xSemaphoreUartRecv;
void uart_stream_create( void );
st_uartrecvlistitem * uart_idle_list_item_get(void);
void uart_idle_list_item_add(st_uartrecvlistitem *item);
void uart_recv_list_item_add(st_uartrecvlistitem *item);
st_uartrecvlistitem *uart_recv_list_item_get(void);
size_t uart_stream_bytes_get(void *buf);
size_t uart_stream_bytes_put_tx(void *buf, size_t len);

#endif