#ifndef __NET_WORK_TASK_H__
#define __NET_WORK_TASK_H__

#define NET_WORK_TASK_TEMP_BUFSIZE                      (512 * 1)
#define NET_WORK_TASK_THREAD_PRIO                       (TCPIP_THREAD_PRIO - 1)
#define NET_WORK_TASK_STK_SIZE                          (128 * 2 + 30)

extern unsigned char temp_buffer[NET_WORK_TASK_TEMP_BUFSIZE];

void net_work_task_init(void);

#endif