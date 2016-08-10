/**@brief 仿照linux kfifo写的ring buffer
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

//判断x是否是2的次方
#define is_power_of_2(x) ((x) != 0 && (((x) & ((x) - 1)) == 0))
//取a和b中最小值
#define min(a, b) (((a) < (b)) ? (a) : (b))

/** 
  * @brief   RingBuffer structure definition  
  */ 
struct __kfifo
{
    u8    *buffer;     //缓冲区
    fifo_size_t     size;       //大小
    fifo_size_t     in;         //入口位置
    fifo_size_t     out;        //出口位置
};

fifo_size_t ring_buffer_len_used(const struct __kfifo *ring_buf);
fifo_size_t ring_buffer_len_unused(const struct __kfifo *ring_buf);
fifo_size_t ring_buffer_get(struct __kfifo *ring_buf, u8 *buffer, fifo_size_t size);
fifo_size_t ring_buffer_put(struct __kfifo *ring_buf, u8 *buffer, fifo_size_t size);
void ring_buffer_init(struct __kfifo *ring_buf);
u8 ring_buffer_get_byte(struct __kfifo *ring_buf, fifo_size_t size);

#endif
