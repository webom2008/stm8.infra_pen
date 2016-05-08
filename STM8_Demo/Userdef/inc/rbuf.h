#ifndef _RBUF_H
#define _RBUF_H

#include "stm8s.h"

#define true                        1
#define false                       0


typedef struct {
    u8 *   pStart;
    u8 *    pIn;
    u8  *pOut;
    u16  size;
    u16  len;
} rbuf_t;

#ifdef _UART_C_
#define INTERFACE
#else
#define INTERFACE extern 
#endif

INTERFACE void  rbuf_init     (rbuf_t* buffer, u8 *start, u16 size);
INTERFACE u8  rbuf_put_data (rbuf_t* buffer, u8  data);
INTERFACE u8  rbuf_get_data (rbuf_t* buffer, u8 *data);
#endif
































