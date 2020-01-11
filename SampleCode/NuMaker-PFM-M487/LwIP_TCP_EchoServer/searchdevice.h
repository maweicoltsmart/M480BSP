#ifndef __SEARCH_DEVICE_H__
#define __SEARCH_DEVICE_H__

#define DEV_SEARCH_UDP_BIND_PORT                        5002
#define DEV_SEARCH_UDP_SEND_PORT                        5003

void search_device_init(void);
void search_device_check(void);

#endif