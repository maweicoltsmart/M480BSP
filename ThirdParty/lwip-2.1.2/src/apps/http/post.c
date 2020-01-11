/**
 * @file
 * HTTPD example for simple POST
 */

 /*
 * Copyright (c) 2017 Simon Goldschmidt
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 *
 * Author: Simon Goldschmidt <goldsimon@gmx.de>
 *
 */

#include "lwip/opt.h"

#include "lwip/apps/httpd.h"
#include "lwip/def.h"
#include "lwip/mem.h"
#include "pragma.h"
#include <stdio.h>
#include <string.h>

/** define LWIP_HTTPD_EXAMPLE_GENERATEDFILES to 1 to enable this file system */
#ifndef LWIP_HTTPD_EXAMPLE_SIMPLEPOST
#define LWIP_HTTPD_EXAMPLE_SIMPLEPOST 1
#endif

#if LWIP_HTTPD_EXAMPLE_SIMPLEPOST

#if !LWIP_HTTPD_SUPPORT_POST
#error This needs LWIP_HTTPD_SUPPORT_POST
#endif

#define USER_PASS_BUFSIZE 16

static void *current_connection;
static void *valid_connection;
static char last_user[USER_PASS_BUFSIZE];
static char *posturi = NULL;
err_t
httpd_post_begin(void *connection, const char *uri, const char *http_request,
                 u16_t http_request_len, int content_len, char *response_uri,
                 u16_t response_uri_len, u8_t *post_auto_wnd)
{
  LWIP_UNUSED_ARG(connection);
  LWIP_UNUSED_ARG(http_request);
  LWIP_UNUSED_ARG(http_request_len);
  LWIP_UNUSED_ARG(content_len);
  LWIP_UNUSED_ARG(post_auto_wnd);
  posturi = (char *)uri;
  if (!memcmp(uri, "/login.cgi", 11)) {
    if (current_connection != connection) {
      current_connection = connection;
      valid_connection = NULL;
      /* default page is "login failed" */
      snprintf(response_uri, response_uri_len, "/index.shtml");
      /* e.g. for large uploads to slow flash over a fast connection, you should
         manually update the rx window. That way, a sender can only send a full
         tcp window at a time. If this is required, set 'post_aut_wnd' to 0.
         We do not need to throttle upload speed here, so: */
      *post_auto_wnd = 1;
      return ERR_OK;
    }
  }
  if (!memcmp(uri, "/modeset.cgi", 13)) {
    if (current_connection != connection) {
      current_connection = connection;
      valid_connection = NULL;
      /* default page is "login failed" */
      snprintf(response_uri, response_uri_len, "/cfg.shtml");
      /* e.g. for large uploads to slow flash over a fast connection, you should
         manually update the rx window. That way, a sender can only send a full
         tcp window at a time. If this is required, set 'post_aut_wnd' to 0.
         We do not need to throttle upload speed here, so: */
      *post_auto_wnd = 1;
      return ERR_OK;
    }
  }
  if (!memcmp(uri, "/localcfg.cgi", 14)) {
    if (current_connection != connection) {
      current_connection = connection;
      valid_connection = NULL;
      /* default page is "login failed" */
      snprintf(response_uri, response_uri_len, "/cfg.shtml");
      /* e.g. for large uploads to slow flash over a fast connection, you should
         manually update the rx window. That way, a sender can only send a full
         tcp window at a time. If this is required, set 'post_aut_wnd' to 0.
         We do not need to throttle upload speed here, so: */
      *post_auto_wnd = 1;
      return ERR_OK;
    }
  }
  if (!memcmp(uri, "/remotecfg.cgi", 15)) {
    if (current_connection != connection) {
      current_connection = connection;
      valid_connection = NULL;
      /* default page is "login failed" */
      snprintf(response_uri, response_uri_len, "/cfg.shtml");
      /* e.g. for large uploads to slow flash over a fast connection, you should
         manually update the rx window. That way, a sender can only send a full
         tcp window at a time. If this is required, set 'post_aut_wnd' to 0.
         We do not need to throttle upload speed here, so: */
      *post_auto_wnd = 1;
      return ERR_OK;
    }
  }
  if (!memcmp(uri, "/comcfg.cgi", 12)) {
    if (current_connection != connection) {
      current_connection = connection;
      valid_connection = NULL;
      /* default page is "login failed" */
      snprintf(response_uri, response_uri_len, "/cfg.shtml");
      /* e.g. for large uploads to slow flash over a fast connection, you should
         manually update the rx window. That way, a sender can only send a full
         tcp window at a time. If this is required, set 'post_aut_wnd' to 0.
         We do not need to throttle upload speed here, so: */
      *post_auto_wnd = 1;
      return ERR_OK;
    }
  }
  return ERR_VAL;
}

err_t
httpd_post_receive_data(void *connection, struct pbuf *p)
{
  if (current_connection == connection) {
    if(!memcmp(posturi, "/login.cgi", 11))
    {
      u16_t token_user = pbuf_memfind(p, "user=", 5, 0);
      u16_t token_pass = pbuf_memfind(p, "pass=", 5, 0);
      if ((token_user != 0xFFFF) && (token_pass != 0xFFFF)) {
        u16_t value_user = token_user + 5;
        u16_t value_pass = token_pass + 5;
        u16_t len_user = 0;
        u16_t len_pass = 0;
        u16_t tmp;
        /* find user len */
        tmp = pbuf_memfind(p, "&", 1, value_user);
        if (tmp != 0xFFFF) {
          len_user = tmp - value_user;
        } else {
          len_user = p->tot_len - value_user;
        }
        /* find pass len */
        tmp = pbuf_memfind(p, "&", 1, value_pass);
        if (tmp != 0xFFFF) {
          len_pass = tmp - value_pass;
        } else {
          len_pass = p->tot_len - value_pass;
        }
        if ((len_user > 0) && (len_user < USER_PASS_BUFSIZE) &&
            (len_pass > 0) && (len_pass < USER_PASS_BUFSIZE)) {
          /* provide contiguous storage if p is a chained pbuf */
          char buf_user[USER_PASS_BUFSIZE];
          char buf_pass[USER_PASS_BUFSIZE];
          char *user = (char *)pbuf_get_contiguous(p, buf_user, sizeof(buf_user), len_user, value_user);
          char *pass = (char *)pbuf_get_contiguous(p, buf_pass, sizeof(buf_pass), len_pass, value_pass);
          if (user && pass) {
            user[len_user] = 0;
            pass[len_pass] = 0;
            if (!strcmp(user, "admin") && !strcmp(pass, "admin")) {
              /* user and password are correct, create a "session" */
              valid_connection = connection;
              memcpy(last_user, user, sizeof(last_user));
            }
          }
        }
      }
    }
    else if(!memcmp(posturi, "/modeset.cgi", 13))
    {
      u16_t token_workmode = pbuf_memfind(p, "workmode=", 9, 0);
      if (token_workmode != 0xFFFF) {
        u16_t value_workmode = token_workmode + 9;
        u16_t len_workmode = 0;
        u16_t tmp;
        /* find workmode len */
        tmp = pbuf_memfind(p, "&", 1, value_workmode);
        if (tmp != 0xFFFF) {
          len_workmode = tmp - value_workmode;
        } else {
          len_workmode = p->tot_len - value_workmode;
        }
        if ((len_workmode > 0) && (len_workmode < USER_PASS_BUFSIZE)) {
          /* provide contiguous storage if p is a chained pbuf */
          char buf_workmode[USER_PASS_BUFSIZE];
          char *workmode = (char *)pbuf_get_contiguous(p, buf_workmode, sizeof(buf_workmode), len_workmode, value_workmode);
          if (workmode) {
            workmode[len_workmode] = 0;
            if(!strcmp(workmode, (char const*)strworkmode[MODULE_WORK_MODE_MJ_CLOUD]))
            {
              stWork_Pragma.workmode = MODULE_WORK_MODE_MJ_CLOUD;
            }
            else if(!strcmp(workmode, (char const*)strworkmode[MODULE_WORK_MODE_TCP_SERVER]))
            {
              stWork_Pragma.workmode = MODULE_WORK_MODE_TCP_SERVER;
            }
            else if(!strcmp(workmode, (char const*)strworkmode[MODULE_WORK_MODE_TCP_CLIENT]))
            {
              stWork_Pragma.workmode = MODULE_WORK_MODE_TCP_CLIENT;
            }
            else if(!strcmp(workmode, (char const*)strworkmode[MODULE_WORK_MODE_UDP]))
            {
              stWork_Pragma.workmode = MODULE_WORK_MODE_UDP;
            }
            else if(!strcmp(workmode, (char const*)strworkmode[MODULE_WORK_MODE_HTTP_CLIENT]))
            {
              stWork_Pragma.workmode = MODULE_WORK_MODE_HTTP_CLIENT;
            }
            else if(!strcmp(workmode, (char const*)strworkmode[MODULE_WORK_MODE_VSPD]))
            {
              stWork_Pragma.workmode = MODULE_WORK_MODE_VSPD;
            }
            valid_connection = connection;
          }
        }
      }
    }
    else if(!memcmp(posturi, "/localcfg.cgi", 14))
    {
      u16_t token_dhcp = pbuf_memfind(p, "dhcp=", 5, 0);
      u16_t token_localip = pbuf_memfind(p, "localip=", 8, 0);
      u16_t token_localport = pbuf_memfind(p, "localport=", 10, 0);
      u16_t token_gateway = pbuf_memfind(p, "gateway=", 8, 0);
      u16_t token_netmask = pbuf_memfind(p, "netmask=", 8, 0);
      u16_t token_dns = pbuf_memfind(p, "dns=", 4, 0);
      if ((token_localip != 0xFFFF) && 
          (token_localport != 0xFFFF) && (token_gateway != 0xFFFF) && 
          (token_netmask != 0xFFFF) && (token_dns != 0xFFFF)) {
        //u16_t value_dhcp = token_dhcp + 5;
        u16_t value_localip = token_localip + 8;
        u16_t value_localport = token_localport + 10;
        u16_t value_gateway = token_gateway + 8;
        u16_t value_netmask = token_netmask + 8;
        u16_t value_dns = token_dns + 4;
        //u16_t len_dhcp = 0;
        u16_t len_localip = 0;
        u16_t len_localport = 0;
        u16_t len_gateway = 0;
        u16_t len_netmask = 0;
        u16_t len_dns = 0;
        u16_t tmp;
        /* find dhcp len
        tmp = pbuf_memfind(p, "&", 1, value_dhcp);
        if (tmp != 0xFFFF) {
          len_dhcp = tmp - value_dhcp;
        } else {
          len_dhcp = p->tot_len - value_dhcp;
        }
        */
        /* find localip len */
        tmp = pbuf_memfind(p, "&", 1, value_localip);
        if (tmp != 0xFFFF) {
          len_localip = tmp - value_localip;
        } else {
          len_localip = p->tot_len - value_localip;
        }
        /* find localport len */
        tmp = pbuf_memfind(p, "&", 1, value_localport);
        if (tmp != 0xFFFF) {
          len_localport = tmp - value_localport;
        } else {
          len_localport = p->tot_len - value_localport;
        }
        /* find gateway len */
        tmp = pbuf_memfind(p, "&", 1, value_gateway);
        if (tmp != 0xFFFF) {
          len_gateway = tmp - value_gateway;
        } else {
          len_gateway = p->tot_len - value_gateway;
        }
        /* find netmask len */
        tmp = pbuf_memfind(p, "&", 1, value_netmask);
        if (tmp != 0xFFFF) {
          len_netmask = tmp - value_netmask;
        } else {
          len_netmask = p->tot_len - value_netmask;
        }
        /* find dns len */
        tmp = pbuf_memfind(p, "&", 1, value_dns);
        if (tmp != 0xFFFF) {
          len_dns = tmp - value_dns;
        } else {
          len_dns = p->tot_len - value_dns;
        }
        if ((len_localip > 0) && (len_localip < USER_PASS_BUFSIZE) &&
            (len_localport > 0) && (len_localport < USER_PASS_BUFSIZE) &&
            (len_gateway > 0) && (len_gateway < USER_PASS_BUFSIZE) &&
            (len_netmask > 0) && (len_netmask < USER_PASS_BUFSIZE) &&
            (len_dns > 0) && (len_dns < USER_PASS_BUFSIZE)) {
          /* provide contiguous storage if p is a chained pbuf */
          //char buf_dhcp[USER_PASS_BUFSIZE];
          char buf_localip[USER_PASS_BUFSIZE];
          char buf_localport[USER_PASS_BUFSIZE];
          char buf_gateway[USER_PASS_BUFSIZE];
          char buf_netmask[USER_PASS_BUFSIZE];
          char buf_dns[USER_PASS_BUFSIZE];
          //char *dhcp = (char *)pbuf_get_contiguous(p, buf_dhcp, sizeof(buf_dhcp), len_dhcp, value_dhcp);
          char *localip = (char *)pbuf_get_contiguous(p, buf_localip, sizeof(buf_localip), len_localip, value_localip);
          char *localport = (char *)pbuf_get_contiguous(p, buf_localport, sizeof(buf_localport), len_localport, value_localport);
          char *gateway = (char *)pbuf_get_contiguous(p, buf_gateway, sizeof(buf_gateway), len_gateway, value_gateway);
          char *netmask = (char *)pbuf_get_contiguous(p, buf_netmask, sizeof(buf_netmask), len_netmask, value_netmask);
          char *dns = (char *)pbuf_get_contiguous(p, buf_dns, sizeof(buf_dns), len_dns, value_dns);
          if (localip && localport && gateway && netmask && dns) {
            //dhcp[len_dhcp] = 0;
            localip[len_localip] = 0;
            localport[len_localport] = 0;
            gateway[len_gateway] = 0;
            netmask[len_netmask] = 0;
            dns[len_dns] = 0;
            ((token_dhcp != 0xFFFF))?(stWork_Pragma.stLocal_IP.dhcp = PRAGMA_DHCP_ON):(stWork_Pragma.stLocal_IP.dhcp = PRAGMA_DHCP_OFF);
            ip4addr_aton(localip,&stWork_Pragma.stLocal_IP.localip);
            sscanf( localport, "%hud", &stWork_Pragma.stLocal_IP.localport );
            ip4addr_aton(gateway,&stWork_Pragma.stLocal_IP.gateway);
            ip4addr_aton(netmask,&stWork_Pragma.stLocal_IP.netmask);
            ip4addr_aton(dns,&stWork_Pragma.stLocal_IP.dns);
            valid_connection = connection;
          }
        }
      }
    }
    else if(!memcmp(posturi, "/remotecfg.cgi", 15))
    {
      u16_t token_destip = pbuf_memfind(p, "destip=", 7, 0);
      u16_t token_destport = pbuf_memfind(p, "destport=", 9, 0);
      if ((token_destip != 0xFFFF) && (token_destport != 0xFFFF)) {
        u16_t value_destip = token_destip + 7;
        u16_t value_destport = token_destport + 9;
        u16_t len_destip = 0;
        u16_t len_destport = 0;
        u16_t tmp;
        /* find destip len */
        tmp = pbuf_memfind(p, "&", 1, value_destip);
        if (tmp != 0xFFFF) {
          len_destip = tmp - value_destip;
        } else {
          len_destip = p->tot_len - value_destip;
        }
        /* find destport len */
        tmp = pbuf_memfind(p, "&", 1, value_destport);
        if (tmp != 0xFFFF) {
          len_destport = tmp - value_destport;
        } else {
          len_destport = p->tot_len - value_destport;
        }
        if ((len_destip > 0) && (len_destip < USER_PASS_BUFSIZE) &&
            (len_destport > 0) && (len_destport < USER_PASS_BUFSIZE)) {
          /* provide contiguous storage if p is a chained pbuf */
          char buf_destip[USER_PASS_BUFSIZE];
          char buf_destport[USER_PASS_BUFSIZE];
          char *destip = (char *)pbuf_get_contiguous(p, buf_destip, sizeof(buf_destip), len_destip, value_destip);
          char *destport = (char *)pbuf_get_contiguous(p, buf_destport, sizeof(buf_destport), len_destport, value_destport);
          if (destip && destport) {
            destip[len_destip] = 0;
            destport[len_destport] = 0;
            strcpy((char*)stWork_Pragma.stRemote_IP.hostname, destip);
            //ip4addr_aton(destip,&stWork_Pragma.stRemote_IP.remoteip);
            sscanf( destport, "%hud", &stWork_Pragma.stRemote_IP.remoteport );
            valid_connection = connection;
          }
        }
      }
    }
    else if(!memcmp(posturi, "/comcfg.cgi", 12))
    {
      u16_t token_baud = pbuf_memfind(p, "baud=", 5, 0);
      u16_t token_databit = pbuf_memfind(p, "databit=", 8, 0);
      u16_t token_stopbit = pbuf_memfind(p, "stopbit=", 8, 0);
      u16_t token_parity = pbuf_memfind(p, "parity=", 7, 0);
      u16_t token_flowctrl = pbuf_memfind(p, "flowctrl=", 9, 0);
      if ((token_baud != 0xFFFF) && (token_databit != 0xFFFF) && 
          (token_stopbit != 0xFFFF) && (token_parity != 0xFFFF) && (token_flowctrl != 0xFFFF)) {
        u16_t value_baud = token_baud + 5;
        u16_t value_databit = token_databit + 8;
        u16_t value_stopbit = token_stopbit + 8;
        u16_t value_parity = token_parity + 7;
        u16_t value_flowctrl = token_flowctrl + 9;
        u16_t len_baud = 0;
        u16_t len_databit = 0;
        u16_t len_stopbit = 0;
        u16_t len_parity = 0;
        u16_t len_flowctrl = 0;
        u16_t tmp;
        /* find baud len */
        tmp = pbuf_memfind(p, "&", 1, value_baud);
        if (tmp != 0xFFFF) {
          len_baud = tmp - value_baud;
        } else {
          len_baud = p->tot_len - value_baud;
        }
        /* find databit len */
        tmp = pbuf_memfind(p, "&", 1, value_databit);
        if (tmp != 0xFFFF) {
          len_databit = tmp - value_databit;
        } else {
          len_databit = p->tot_len - value_databit;
        }
        /* find stopbit len */
        tmp = pbuf_memfind(p, "&", 1, value_stopbit);
        if (tmp != 0xFFFF) {
          len_stopbit = tmp - value_stopbit;
        } else {
          len_stopbit = p->tot_len - value_stopbit;
        }
        /* find parity len */
        tmp = pbuf_memfind(p, "&", 1, value_parity);
        if (tmp != 0xFFFF) {
          len_parity = tmp - value_parity;
        } else {
          len_parity = p->tot_len - value_parity;
        }
        /* find flowctrl len */
        tmp = pbuf_memfind(p, "&", 1, value_flowctrl);
        if (tmp != 0xFFFF) {
          len_flowctrl = tmp - value_flowctrl;
        } else {
          len_flowctrl = p->tot_len - value_flowctrl;
        }
        if ((len_baud > 0) && (len_baud < USER_PASS_BUFSIZE) &&
            (len_databit > 0) && (len_databit < USER_PASS_BUFSIZE) &&
            (len_stopbit > 0) && (len_stopbit < USER_PASS_BUFSIZE) &&
            (len_parity > 0) && (len_parity < USER_PASS_BUFSIZE) &&
            (len_flowctrl > 0) && (len_flowctrl < USER_PASS_BUFSIZE)) {
          /* provide contiguous storage if p is a chained pbuf */
          char buf_baud[USER_PASS_BUFSIZE];
          char buf_databit[USER_PASS_BUFSIZE];
          char buf_stopbit[USER_PASS_BUFSIZE];
          char buf_parity[USER_PASS_BUFSIZE];
          char buf_flowctrl[USER_PASS_BUFSIZE];
          char *baud = (char *)pbuf_get_contiguous(p, buf_baud, sizeof(buf_baud), len_baud, value_baud);
          char *databit = (char *)pbuf_get_contiguous(p, buf_databit, sizeof(buf_databit), len_databit, value_databit);
          char *stopbit = (char *)pbuf_get_contiguous(p, buf_stopbit, sizeof(buf_stopbit), len_stopbit, value_stopbit);
          char *parity = (char *)pbuf_get_contiguous(p, buf_parity, sizeof(buf_parity), len_parity, value_parity);
          char *flowctrl = (char *)pbuf_get_contiguous(p, buf_flowctrl, sizeof(buf_flowctrl), len_flowctrl, value_flowctrl);
          if (baud && databit && stopbit && parity && flowctrl) {
            baud[len_baud] = 0;
            databit[len_databit] = 0;
            stopbit[len_stopbit] = 0;
            parity[len_parity] = 0;
            flowctrl[len_flowctrl] = 0;
            sscanf( baud, "%ud", &stWork_Pragma.stUart_Pragma.baud );
            //sscanf( databit, "%ud", &stWork_Pragma.stUart_Pragma.databit );
            stWork_Pragma.stUart_Pragma.databit = ((databit[0] > 0x36) && (databit[0] < 0x3a))?(databit[0] - 0x30):8;
            if(!strcmp(stopbit, (char const*)strstopbit[PRAGMA_UART_STOP_BIT_1]))
            {
              stWork_Pragma.stUart_Pragma.stopbit = PRAGMA_UART_STOP_BIT_1;
            }
            else if(!strcmp(stopbit, (char const*)strstopbit[PRAGMA_UART_STOP_BIT_1_5]))
            {
              stWork_Pragma.stUart_Pragma.stopbit = PRAGMA_UART_STOP_BIT_1_5;
            }
            else if(!strcmp(stopbit, (char const*)strstopbit[PRAGMA_UART_STOP_BIT_2]))
            {
              stWork_Pragma.stUart_Pragma.stopbit = PRAGMA_UART_STOP_BIT_2;
            }
            if(!strcmp(parity, (char const*)strparity[PRAGMA_UART_PARITY_NONE]))
            {
              stWork_Pragma.stUart_Pragma.parity = PRAGMA_UART_PARITY_NONE;
            }
            else if(!strcmp(parity, (char const*)strparity[PRAGMA_UART_PARITY_ODD]))
            {
              stWork_Pragma.stUart_Pragma.parity = PRAGMA_UART_PARITY_ODD;
            }
            else if(!strcmp(parity, (char const*)strparity[PRAGMA_UART_PARITY_EVEN]))
            {
              stWork_Pragma.stUart_Pragma.parity = PRAGMA_UART_PARITY_EVEN;
            }
            else if(!strcmp(parity, (char const*)strparity[PRAGMA_UART_PARITY_MARK]))
            {
              stWork_Pragma.stUart_Pragma.parity = PRAGMA_UART_PARITY_MARK;
            }
            else if(!strcmp(parity, (char const*)strparity[PRAGMA_UART_PARITY_SPACE]))
            {
              stWork_Pragma.stUart_Pragma.parity = PRAGMA_UART_PARITY_SPACE;
            }
            if(!strcmp(flowctrl, (char const*)strflowctrl[PRAGMA_UART_FLOWCTRL_NONE]))
            {
              stWork_Pragma.stUart_Pragma.flowctrl = PRAGMA_UART_FLOWCTRL_NONE;
            }
            else if(!strcmp(flowctrl, (char const*)strflowctrl[PRAGMA_UART_FLOWCTRL_HARDWARE]))
            {
              stWork_Pragma.stUart_Pragma.flowctrl = PRAGMA_UART_FLOWCTRL_HARDWARE;
            }
            else if(!strcmp(flowctrl, (char const*)strflowctrl[PRAGMA_UART_FLOWCTRL_XONOFF]))
            {
              stWork_Pragma.stUart_Pragma.flowctrl = PRAGMA_UART_FLOWCTRL_XONOFF;
            }
            valid_connection = connection;
          }
        }
      }
    }
    /* not returning ERR_OK aborts the connection, so return ERR_OK unless the
       conenction is unknown */
    pbuf_free(p);
    return ERR_OK;
  }
  pbuf_free(p);
  return ERR_VAL;
}

void
httpd_post_finished(void *connection, char *response_uri, u16_t response_uri_len)
{
  /* default page is "login failed" */
  snprintf(response_uri, response_uri_len, "/index.shtml");
  if (current_connection == connection) {
    if (valid_connection == connection) {
      /* login succeeded */
      snprintf(response_uri, response_uri_len, "/cfg.shtml");
    }
    current_connection = NULL;
    valid_connection = NULL;
  }
}

#endif /* LWIP_HTTPD_EXAMPLE_SIMPLEPOST*/
