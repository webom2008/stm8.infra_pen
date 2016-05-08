
#include "rbuf.h"


//接收缓冲区初始化 为在*start中存或取
void  rbuf_init (rbuf_t* buffer, u8 *start, u16 size)  //rbuf_init(&rUart1RxBuf, Uart1RxBuf, UART1_RX_BUF_SIZE); 
{    
    disableInterrupts();
    buffer->pStart = start;
    buffer->pIn    = start;
    buffer->pOut   = start;
    buffer->size   = size;
    buffer->len    = 0;
    enableInterrupts();
}

// 接收到的数据data ,存入循环单链表中 buffer -> pin++
u8  rbuf_put_data (rbuf_t* buffer, u8 data)
{    
	disableInterrupts();
	if (buffer->len < buffer->size)                                //如果没有取一直存，满啦就不存啦
	{
		*(buffer->pIn++) = data;
		if (buffer->pIn == (buffer->pStart + buffer->size)) 
		{
			buffer->pIn = buffer->pStart;
		}
		buffer->len++;
		enableInterrupts();
		return true;
	}
	enableInterrupts();
	return false;
}

//从buffer-> * pout中取数据到 * data中，数据不为空返回true,一次取一个
u8  rbuf_get_data (rbuf_t* buffer, u8 *data)
{    
    disableInterrupts();
    if (buffer->len > 0)
	{                                                  //如果一直取不存，取完，就不取
        *data = *(buffer->pOut++);
        if (buffer->pOut == (buffer->pStart + buffer->size))
		{
            buffer->pOut = buffer->pStart;
        }
        buffer->len--;
        enableInterrupts();
        return true;
    }
    enableInterrupts();
    return false;
}


