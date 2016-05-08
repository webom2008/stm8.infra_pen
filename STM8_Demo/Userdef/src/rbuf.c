
#include "rbuf.h"


//���ջ�������ʼ�� Ϊ��*start�д��ȡ
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

// ���յ�������data ,����ѭ���������� buffer -> pin++
u8  rbuf_put_data (rbuf_t* buffer, u8 data)
{    
	disableInterrupts();
	if (buffer->len < buffer->size)                                //���û��ȡһֱ�棬�����Ͳ�����
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

//��buffer-> * pout��ȡ���ݵ� * data�У����ݲ�Ϊ�շ���true,һ��ȡһ��
u8  rbuf_get_data (rbuf_t* buffer, u8 *data)
{    
    disableInterrupts();
    if (buffer->len > 0)
	{                                                  //���һֱȡ���棬ȡ�꣬�Ͳ�ȡ
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


