/**************************************************************************//**
 * @file     mem_alloc.c
 * @version  V1.10
 * $Revision: 11 $
 * $Date: 14/10/03 1:54p $
 * @brief   USB host library memory allocation functions.
 *
 * @note
 * Copyright (C) 2017 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "M480.h"

#include "usb.h"


/// @cond HIDDEN_SYMBOLS

//#define MEM_DEBUG

#ifdef MEM_DEBUG
#define mem_debug       printf
#else
#define mem_debug(...)
#endif

#ifdef __ICCARM__
#pragma data_alignment=32
static uint8_t  _mem_pool[MEM_POOL_UNIT_NUM][MEM_POOL_UNIT_SIZE];
#else
static __align(32) uint8_t _mem_pool[MEM_POOL_UNIT_NUM][MEM_POOL_UNIT_SIZE];
#endif
static uint8_t  _unit_used[MEM_POOL_UNIT_NUM];

static volatile int  _usbh_mem_used;
static volatile int  _usbh_max_mem_used;
static volatile int  _mem_pool_used;


UDEV_T * g_udev_list;
static volatile int  _device_num;

static  int  _sidx = 0;;

/*--------------------------------------------------------------------------*/
/*   Memory alloc/free recording                                            */
/*--------------------------------------------------------------------------*/

void usbh_memory_init(void)
{
    if (sizeof(TD_T) > MEM_POOL_UNIT_SIZE) {
        USB_error("TD_T - MEM_POOL_UNIT_SIZE too small!\n");
        while (1);
    }

    if (sizeof(ED_T) > MEM_POOL_UNIT_SIZE) {
        USB_error("ED_T - MEM_POOL_UNIT_SIZE too small!\n");
        while (1);
    }

    _usbh_mem_used = 0L;
    _usbh_max_mem_used = 0L;

    memset(_unit_used, 0, sizeof(_unit_used));
    _mem_pool_used = 0;
    _sidx = 0;

    g_udev_list = NULL;
    _device_num = 1;
}

uint32_t  usbh_memory_used(void)
{
    printf("USB static memory: %d/%d, heap used: %d\n", _mem_pool_used, MEM_POOL_UNIT_NUM, _usbh_mem_used);
    return _usbh_mem_used;
}

static void  memory_counter(int size)
{
    _usbh_mem_used += size;
    if (_usbh_mem_used > _usbh_max_mem_used)
        _usbh_max_mem_used = _usbh_mem_used;
}

void * usbh_alloc_mem(int size)
{
    void  *p;

    p = malloc(size);
    if (p == NULL) {
        USB_error("usbh_alloc_mem failed! %d\n", size);
        return NULL;
    }

    memset(p, 0, size);
    memory_counter(size);
    return p;
}

void usbh_free_mem(void *p, int size)
{
    free(p);
    memory_counter(0-size);
}


/*--------------------------------------------------------------------------*/
/*   USB device allocate/free                                               */
/*--------------------------------------------------------------------------*/

UDEV_T * alloc_device(void)
{
    UDEV_T  *udev;

    udev = malloc(sizeof(*udev));
    if (udev == NULL) {
        USB_error("alloc_device failed!\n");
        return NULL;
    }
    memset(udev, 0, sizeof(*udev));
    memory_counter(sizeof(*udev));
    udev->cur_conf = -1;                    /* must! used to identify the first SET CONFIGURATION */
    udev->next = g_udev_list;               /* chain to global device list */
    g_udev_list = udev;
    return udev;
}

void free_device(UDEV_T *udev)
{
    UDEV_T  *d;

    if (udev == NULL)
        return;

    if (udev->cfd_buff != NULL)
        usbh_free_mem(udev->cfd_buff, MAX_DESC_BUFF_SIZE);

    /*
     *  Remove it from the global device list
     */
    if (g_udev_list == udev) {
        g_udev_list = g_udev_list->next;
    } else {
        d = g_udev_list;
        while (d != NULL) {
            if (d->next == udev) {
                d->next = udev->next;
                break;
            }
            d = d->next;
        }
    }

    free(udev);
    memory_counter(-sizeof(*udev));
}

int  alloc_device_number(void)
{
    int   dev_num;
    dev_num = _device_num;                  /* allocate device number */
    _device_num = (_device_num % 254) + 1;  /* cannot be 0            */
    return dev_num;
}

/*--------------------------------------------------------------------------*/
/*   UTR (USB Transfer Request) allocate/free                               */
/*--------------------------------------------------------------------------*/

UTR_T * alloc_utr(UDEV_T *udev)
{
    UTR_T  *utr;

    utr = malloc(sizeof(*utr));
    if (utr == NULL) {
        USB_error("alloc_utr failed!\n");
        return NULL;
    }
    memory_counter(sizeof(*utr));
    memset(utr, 0, sizeof(*utr));
    utr->udev = udev;
    mem_debug("[ALLOC] [UTR] - 0x%x\n", (int)utr);
    return utr;
}

void free_utr(UTR_T *utr)
{
    if (utr == NULL)
        return;

    mem_debug("[FREE] [UTR] - 0x%x\n", (int)utr);
    free(utr);
    memory_counter(0-(int)sizeof(*utr));
}

/*--------------------------------------------------------------------------*/
/*   OHCI ED allocate/free                                                  */
/*--------------------------------------------------------------------------*/

ED_T * alloc_ohci_ED(void)
{
    int    i;
    ED_T   *ed;

    for (i = 0; i < MEM_POOL_UNIT_NUM; i++) {
        if (_unit_used[i] == 0) {
            _unit_used[i] = 1;
            _mem_pool_used++;
            ed = (ED_T *)&_mem_pool[i];
            memset(ed, 0, sizeof(*ed));
            mem_debug("[ALLOC] [ED] - 0x%x\n", (int)ed);
            return ed;
        }
    }
    USB_error("alloc_ohci_ED failed!\n");
    return NULL;
}

void free_ohci_ED(ED_T *ed)
{
    int      i;

    for (i = 0; i < MEM_POOL_UNIT_NUM; i++) {
        if ((uint32_t)&_mem_pool[i] == (uint32_t)ed) {
            mem_debug("[FREE]  [ED] - 0x%x\n", (int)ed);
            _unit_used[i] = 0;
            _mem_pool_used--;
            return;
        }
    }
    USB_debug("free_ohci_ED - not found! (ignored in case of multiple UTR)\n");
}

/*--------------------------------------------------------------------------*/
/*   OHCI TD allocate/free                                                  */
/*--------------------------------------------------------------------------*/
TD_T * alloc_ohci_TD(UTR_T *utr)
{
    int    i;
    TD_T   *td;

    for (i = 0; i < MEM_POOL_UNIT_NUM; i++) {
        if (_unit_used[i] == 0) {
            _unit_used[i] = 1;
            _mem_pool_used++;
            td = (TD_T *)&_mem_pool[i];

            memset(td, 0, sizeof(*td));
            td->utr = utr;
            mem_debug("[ALLOC] [TD] - 0x%x\n", (int)td);
            return td;
        }
    }
    USB_error("alloc_ohci_TD failed!\n");
    return NULL;
}

void free_ohci_TD(TD_T *td)
{
    int   i;

    for (i = 0; i < MEM_POOL_UNIT_NUM; i++) {
        if ((uint32_t)&_mem_pool[i] == (uint32_t)td) {
            mem_debug("[FREE]  [TD] - 0x%x\n", (int)td);
            _unit_used[i] = 0;
            _mem_pool_used--;
            return;
        }
    }
    USB_error("free_ohci_TD - not found!\n");
}

/*--------------------------------------------------------------------------*/
/*   EHCI QH allocate/free                                                  */
/*--------------------------------------------------------------------------*/
QH_T * alloc_ehci_QH(void)
{
    int    i;
    QH_T   *qh = NULL;

    for (i = (_sidx+1) % MEM_POOL_UNIT_NUM; i != _sidx; i = (i+1) % MEM_POOL_UNIT_NUM) {
        if (_unit_used[i] == 0) {
            _unit_used[i] = 1;
            _sidx = i;
            _mem_pool_used++;
            qh = (QH_T *)&_mem_pool[i];
            memset(qh, 0, sizeof(*qh));
            mem_debug("[ALLOC] [QH] - 0x%x\n", (int)qh);
            break;
        }
    }
    if (qh == NULL) {
        USB_error("alloc_ehci_QH failed!\n");
        return NULL;
    }
    qh->Curr_qTD        = QTD_LIST_END;
    qh->OL_Next_qTD     = QTD_LIST_END;
    qh->OL_Alt_Next_qTD = QTD_LIST_END;
    qh->OL_Token        = QTD_STS_HALT;
    return qh;
}

void free_ehci_QH(QH_T *qh)
{
    int      i;

    for (i = 0; i < MEM_POOL_UNIT_NUM; i++) {
        if ((uint32_t)&_mem_pool[i] == (uint32_t)qh) {
            mem_debug("[FREE]  [QH] - 0x%x\n", (int)qh);
            _unit_used[i] = 0;
            _mem_pool_used--;
            return;
        }
    }
    USB_debug("free_ehci_QH - not found! (ignored in case of multiple UTR)\n");
}

/*--------------------------------------------------------------------------*/
/*   EHCI qTD allocate/free                                                 */
/*--------------------------------------------------------------------------*/
qTD_T * alloc_ehci_qTD(UTR_T *utr)
{
    int     i;
    qTD_T   *qtd;

    for (i = (_sidx+1) % MEM_POOL_UNIT_NUM; i != _sidx; i = (i+1) % MEM_POOL_UNIT_NUM) {
        if (_unit_used[i] == 0) {
            _unit_used[i] = 1;
            _sidx = i;
            _mem_pool_used++;
            qtd = (qTD_T *)&_mem_pool[i];

            memset(qtd, 0, sizeof(*qtd));
            qtd->Next_qTD     = QTD_LIST_END;
            qtd->Alt_Next_qTD = QTD_LIST_END;
            qtd->Token        = QTD_STS_HALT;
            qtd->utr = utr;
            mem_debug("[ALLOC] [qTD] - 0x%x\n", (int)qtd);
            return qtd;
        }
    }
    USB_error("alloc_ehci_qTD failed!\n");
    return NULL;
}

void free_ehci_qTD(qTD_T *qtd)
{
    int   i;

    for (i = 0; i < MEM_POOL_UNIT_NUM; i++) {
        if ((uint32_t)&_mem_pool[i] == (uint32_t)qtd) {
            mem_debug("[FREE]  [qTD] - 0x%x\n", (int)qtd);
            _unit_used[i] = 0;
            _mem_pool_used--;
            return;
        }
    }
    USB_error("free_ehci_qTD 0x%x - not found!\n", (int)qtd);
}

/*--------------------------------------------------------------------------*/
/*   EHCI iTD allocate/free                                                 */
/*--------------------------------------------------------------------------*/
iTD_T * alloc_ehci_iTD(void)
{
    int     i;
    iTD_T   *itd;

    for (i = (_sidx+1) % MEM_POOL_UNIT_NUM; i != _sidx; i = (i+1) % MEM_POOL_UNIT_NUM) {
        if (i+2 >= MEM_POOL_UNIT_NUM)
            continue;

        if ((_unit_used[i] == 0) && (_unit_used[i+1] == 0)) {
            _unit_used[i] = _unit_used[i+1] = 1;
            _sidx = i+1;
            _mem_pool_used += 2;
            itd = (iTD_T *)&_mem_pool[i];
            memset(itd, 0, sizeof(*itd));
            mem_debug("[ALLOC] [iTD] - 0x%x\n", (int)itd);
            return itd;
        }
    }
    USB_error("alloc_ehci_iTD failed!\n");
    return NULL;
}

void free_ehci_iTD(iTD_T *itd)
{
    int   i;

    for (i = 0; i+1 < MEM_POOL_UNIT_NUM; i++) {
        if ((uint32_t)&_mem_pool[i] == (uint32_t)itd) {
            mem_debug("[FREE]  [iTD] - 0x%x\n", (int)itd);
            _unit_used[i] = _unit_used[i+1] = 0;
            _mem_pool_used -= 2;
            return;
        }
    }
    USB_error("free_ehci_iTD 0x%x - not found!\n", (int)itd);
}

/*--------------------------------------------------------------------------*/
/*   EHCI iTD allocate/free                                                 */
/*--------------------------------------------------------------------------*/
siTD_T * alloc_ehci_siTD(void)
{
    int     i;
    siTD_T  *sitd;

    for (i = (_sidx+1) % MEM_POOL_UNIT_NUM; i != _sidx; i = (i+1) % MEM_POOL_UNIT_NUM) {
        if (_unit_used[i] == 0) {
            _unit_used[i] = 1;
            _sidx = i;
            _mem_pool_used ++;
            sitd = (siTD_T *)&_mem_pool[i];
            memset(sitd, 0, sizeof(*sitd));
            mem_debug("[ALLOC] [siTD] - 0x%x\n", (int)sitd);
            return sitd;
        }
    }
    USB_error("alloc_ehci_siTD failed!\n");
    return NULL;
}

void free_ehci_siTD(siTD_T *sitd)
{
    int   i;

    for (i = 0; i < MEM_POOL_UNIT_NUM; i++) {
        if ((uint32_t)&_mem_pool[i] == (uint32_t)sitd) {
            mem_debug("[FREE]  [siTD] - 0x%x\n", (int)sitd);
            _unit_used[i] = 0;
            _mem_pool_used--;
            return;
        }
    }
    USB_error("free_ehci_siTD 0x%x - not found!\n", (int)sitd);
}

/// @endcond HIDDEN_SYMBOLS

/*** (C) COPYRIGHT 2017 Nuvoton Technology Corp. ***/

