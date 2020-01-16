#include "stdio.h"
#include "NuMicro.h"
/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "stream_buffer.h"
#include "uart_stream.h"
#include "tcpclient.h"
#include "contiki_list.h"
#include "networktask.h"

LIST(uart_recv_list_head);
LIST(uart_idle_list_head);
st_uartrecvlistitem uartrecvlistitem[MAX_UART_RECV_LIST_ITEM];

StreamBufferHandle_t xStreamBufferUartRx = NULL;
StreamBufferHandle_t xStreamBufferUartTx = NULL;

void uart_stream_create( void )
{
    const size_t xStreamBufferSizeBytes = 1024 * 5, xTriggerLevel = 0;

    // Create a stream buffer that can hold 100 bytes.  The memory used to hold
    // both the stream buffer structure and the data in the stream buffer is
    // allocated dynamically.
    if(!xStreamBufferUartRx)
    {
      xStreamBufferUartRx = xStreamBufferCreate( xStreamBufferSizeBytes, xTriggerLevel );

      if( xStreamBufferUartRx == NULL )
      {
          printf("The xStreamBufferUartRx buffer was created failed!\r\n");
          // There was not enough heap memory space available to create the
          // stream buffer.
      }
      else
      {
          printf("The xStreamBufferUartRx buffer was created successfully!\r\n");
          // The stream buffer was created successfully and can now be used.
      }
    }
    if(!xStreamBufferUartTx)
    {
      xStreamBufferUartTx = xStreamBufferCreate( 1024 * 6, xTriggerLevel );

      if( xStreamBufferUartTx == NULL )
      {
          printf("The xStreamBufferUartTx buffer was created failed!\r\n");
          // There was not enough heap memory space available to create the
          // stream buffer.
      }
      else
      {
          printf("The xStreamBufferUartTx buffer was created successfully!\r\n");
          // The stream buffer was created successfully and can now be used.
      }
    }
    list_init(uart_recv_list_head);
    list_init(uart_idle_list_head);
    for(uint8_t loop = 0; loop < MAX_UART_RECV_LIST_ITEM; loop ++)
    {
        uartrecvlistitem[loop].cnt = 0;
        list_add(uart_idle_list_head, &uartrecvlistitem[loop]);
    }
    UART_EnableInt(UART0, (UART_INTEN_RDAIEN_Msk | UART_INTEN_THREIEN_Msk));
}

st_uartrecvlistitem * uart_idle_list_item_get(void)
{
    st_uartrecvlistitem *pframe;
    UBaseType_t uxSavedInterruptStatus;
    //taskENTER_CRITICAL();
    uxSavedInterruptStatus = taskENTER_CRITICAL_FROM_ISR();
    pframe = list_pop(uart_idle_list_head);
    taskEXIT_CRITICAL_FROM_ISR( uxSavedInterruptStatus );
    //taskEXIT_CRITICAL();
    return pframe;
}

void uart_idle_list_item_add(st_uartrecvlistitem *item)
{
    taskENTER_CRITICAL();
    list_add(uart_idle_list_head, item);
    taskEXIT_CRITICAL();
}

void uart_recv_list_item_add(st_uartrecvlistitem *item)
{
    UBaseType_t uxSavedInterruptStatus;
    //taskENTER_CRITICAL();
    uxSavedInterruptStatus = taskENTER_CRITICAL_FROM_ISR();
    list_add(uart_recv_list_head, item);
    taskEXIT_CRITICAL_FROM_ISR( uxSavedInterruptStatus );
    //taskEXIT_CRITICAL();
}

st_uartrecvlistitem *uart_recv_list_item_get(void)
{
    st_uartrecvlistitem *item;
    taskENTER_CRITICAL();
    item = list_pop(uart_recv_list_head);
    taskEXIT_CRITICAL();
    return item;
}
    
size_t uart_stream_bytes_get(void *buf)
{
    size_t bytes = 0;
    st_uartrecvlistitem *item = uart_recv_list_item_get();
    if(item)
    {
        //taskENTER_CRITICAL();
        bytes = xStreamBufferReceive( xStreamBufferUartRx,
                                               buf,
                                               item->cnt,
                                               pdMS_TO_TICKS( 0 ) );
        //taskEXIT_CRITICAL();
        uart_idle_list_item_add(item);
        return bytes;
    }
    else
    {
        return 0;
    }
}
size_t minspace = 10 * 1024;
size_t uart_stream_bytes_put_tx(void *buf, size_t len)
{
    size_t bytes = 0, space = 0;

    //taskENTER_CRITICAL();
    bytes = xStreamBufferSend( xStreamBufferUartTx, (const void *)buf, len, pdMS_TO_TICKS( 0 ) );
    //UART1_INTCfg( ENABLE, RB_IER_THR_EMPTY );
    UART_EnableInt(UART0, UART_INTEN_THREIEN_Msk);
    taskENTER_CRITICAL();
    space = xStreamBufferSpacesAvailable(xStreamBufferUartTx);
    taskEXIT_CRITICAL();
    minspace = (minspace < space)?minspace:space;
    //taskEXIT_CRITICAL();
    
    
    //printf("\r\n%d, %d, %d\r\n", space, len, bytes);
    
    return bytes;
}

static uint16_t uartrecvbytecnt = 0;
void TMR0_IRQHandler(void)
{
    if(TIMER_GetIntFlag(TIMER0) == 1)
    {
        /* Clear Timer0 time-out interrupt flag */
        TIMER_ClearIntFlag(TIMER0);

        if(uartrecvbytecnt > 0)
        {
            st_uartrecvlistitem *item;
            item = uart_idle_list_item_get();
            if(item)
            {
                if(uartrecvbytecnt >= NET_WORK_TASK_TEMP_BUFSIZE)
                {
                    item->cnt = NET_WORK_TASK_TEMP_BUFSIZE;
                    uart_recv_list_item_add(item);
                    uartrecvbytecnt -= NET_WORK_TASK_TEMP_BUFSIZE;
                }
                else
                {
                    item->cnt = uartrecvbytecnt;
                    uart_recv_list_item_add(item);
                    uartrecvbytecnt = 0;
                }
            }
        }
        else
        {
            TIMER_DisableInt(TIMER0);
            TIMER_ResetCounter(TIMER0);
        }
    }
}
/*---------------------------------------------------------------------------------------------------------*/
/* ISR to handle UART Channel 0 interrupt event                                                            */
/*---------------------------------------------------------------------------------------------------------*/
void UART0_IRQHandler(void)
{
    uint8_t u8InChar = 0xFF;
    uint32_t u32IntSts = UART0->INTSTS;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE; // Initialised to pdFALSE.

    if(u32IntSts & UART_INTSTS_RDAINT_Msk)
    {
        /* Get all the input characters */
        while(UART_GET_RX_EMPTY(UART0) == 0)
        {
            /* Get the character from UART Buffer */
            u8InChar = UART_READ(UART0);
            TIMER_ResetCounter(TIMER0);
            TIMER_EnableInt(TIMER0);
            if(xStreamBufferUartRx != NULL)
            {
                if(xStreamBufferSendFromISR( xStreamBufferUartRx,
                                           ( void * ) &u8InChar,
                                           1,
                                           &xHigherPriorityTaskWoken ) > 0)
                {
                    uartrecvbytecnt ++;
                    if(uartrecvbytecnt >= NET_WORK_TASK_TEMP_BUFSIZE)
                    {
                        st_uartrecvlistitem *item;
                        item = uart_idle_list_item_get();
                        if(item)
                        {
                            item->cnt = NET_WORK_TASK_TEMP_BUFSIZE;
                            uart_recv_list_item_add(item);
                            uartrecvbytecnt -= NET_WORK_TASK_TEMP_BUFSIZE;
                        }
                    }
                }
            }
        }
        //UART_EnableInt(UART0, UART_INTEN_RXTOIEN_Msk);
    }
    if(u32IntSts & UART_INTSTS_RXTOINT_Msk)
    {
        if(uartrecvbytecnt > 0)
        {
            st_uartrecvlistitem *item;
            item = uart_idle_list_item_get();
            if(item)
            {
                if(uartrecvbytecnt >= NET_WORK_TASK_TEMP_BUFSIZE)
                {
                    item->cnt = NET_WORK_TASK_TEMP_BUFSIZE;
                    uart_recv_list_item_add(item);
                    uartrecvbytecnt -= NET_WORK_TASK_TEMP_BUFSIZE;
                }
                else
                {
                    item->cnt = uartrecvbytecnt;
                    uart_recv_list_item_add(item);
                    uartrecvbytecnt = 0;
                }
            }
            UART_EnableInt(UART0, UART_INTEN_RXTOIEN_Msk);
        }
        else
        {
            UART_DisableInt(UART0, UART_INTEN_RXTOIEN_Msk);
        }
    }

    if(u32IntSts & UART_INTSTS_THREINT_Msk)
    {
        //uxSavedInterruptStatus = taskENTER_CRITICAL_FROM_ISR();
        if(xStreamBufferReceiveFromISR( xStreamBufferUartTx, ( void * ) &u8InChar, 1, &xHigherPriorityTaskWoken ) > 0)
        {
            while(UART_IS_TX_FULL(UART0));  /* Wait Tx is not full to transmit data */
            UART_WRITE(UART0, u8InChar);
        }
        else
        {
            UART_DisableInt(UART0, ( UART_INTEN_THREIEN_Msk));
        }
    }

    if(UART0->FIFOSTS & (UART_FIFOSTS_BIF_Msk | UART_FIFOSTS_FEF_Msk | UART_FIFOSTS_PEF_Msk | UART_FIFOSTS_RXOVIF_Msk))
    {
        UART0->FIFOSTS = (UART_FIFOSTS_BIF_Msk | UART_FIFOSTS_FEF_Msk | UART_FIFOSTS_PEF_Msk | UART_FIFOSTS_RXOVIF_Msk);
    }
    if( xHigherPriorityTaskWoken != pdFALSE )
    {
        portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
    }
}