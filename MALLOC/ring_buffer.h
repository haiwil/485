/**@brief ����linux kfifoд��ring buffer
 *@atuher   river date:2015-12-29
* ring_buffer.h
 * */
 
#ifndef RING_BUFFER_H 
#define RING_BUFFER_H

#include "sys.h"
/* ------------------------------------------------------------------------------------------------
 *                                        Standard Defines
 * ------------------------------------------------------------------------------------------------
 */
//#ifndef ERROR
//#define ERROR 0xFF
//#endif

#ifndef NULL
#define NULL 0
#endif

typedef unsigned int  fifo_size_t;

//�ж�x�Ƿ���2�Ĵη�
#define is_power_of_2(x) ((x) != 0 && (((x) & ((x) - 1)) == 0))
//ȡa��b����Сֵ
#define min(a, b) (((a) < (b)) ? (a) : (b))

/** 
  * @brief   RingBuffer structure definition  
  */ 
struct __kfifo
{
    u8    *buffer;     //������
    fifo_size_t     size;       //��С
    fifo_size_t     in;         //���λ��
    fifo_size_t     out;        //����λ��
};

fifo_size_t ring_buffer_len_used(const struct __kfifo *ring_buf);
fifo_size_t ring_buffer_len_unused(const struct __kfifo *ring_buf);
fifo_size_t ring_buffer_get(struct __kfifo *ring_buf, u8 *buffer, fifo_size_t size);
fifo_size_t ring_buffer_put(struct __kfifo *ring_buf, u8 *buffer, fifo_size_t size);
void ring_buffer_init(struct __kfifo *ring_buf);
u8 ring_buffer_get_byte(struct __kfifo *ring_buf, fifo_size_t size);

#endif
