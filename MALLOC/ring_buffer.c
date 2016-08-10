/**@brief 仿照linux kfifo写的ring buffer
 *@atuher   river date:2015-12-29
* ring_buffer.c
 * */
#include "sys.h"
#include "ring_buffer.h" 

/*
#define MY_FIFO_SIZE  1024  //size must be power of 2. 
u8 my_fifo[MY_FIFO_SIZE] = {0};



//缓冲区结构定义和初始化
struct __kfifo my_fifo_dev=
{
    my_fifo,    //缓冲区
    MY_FIFO_SIZE,  //大小
    0,       	//入口位置
    0,        	//出口位置
};*/


//互斥锁函数，多线程系统中使用
 void pthread_mutex_lock(void)
 {

 }
 void pthread_mutex_unlock(void)
 {

 }
 
//复制内存
//*des:目的地址
//*src:源地址
//n:需要复制的内存长度(字节为单位)
void __fifo_memcpy(void *des,void *src,fifo_size_t n)  
{  
    u8 *xdes=des;
	u8 *xsrc=src; 
    while(n--)*xdes++=*xsrc++;  
} 

//获取缓冲区的长度
//*ring_buf:缓冲区结构的地址
//返回值:缓冲区的长度
fifo_size_t __ring_buffer_len(const struct __kfifo *ring_buf)
{
    return (ring_buf->in - ring_buf->out);
}

//从缓冲区中取数据
//*ring_buf:缓冲区结构的地址
//* buffer:目的缓存区的地址
//size:要求取出的数据字节数
//返回值:实际取出的数据字节数
fifo_size_t __ring_buffer_get(struct __kfifo *ring_buf, u8 * buffer, fifo_size_t size)
{
    fifo_size_t len = 0;
    size  = min(size, ring_buf->in - ring_buf->out);        
    /* first get the data from fifo->out until the end of the buffer */
    len = min(size, ring_buf->size - (ring_buf->out & (ring_buf->size - 1)));
    __fifo_memcpy(buffer, ring_buf->buffer + (ring_buf->out & (ring_buf->size - 1)), len);
    /* then get the rest (if any) from the beginning of the buffer */
    __fifo_memcpy(buffer + len, ring_buf->buffer, size - len);
    ring_buf->out += size;
    return size;
}

//向缓冲区中存放数据
//*ring_buf:缓冲区结构的地址
//* buffer:数据源缓存区的地址
//size:要求存入缓冲区的数据字节数
//返回值:实际存入缓冲区的数据字节数
fifo_size_t __ring_buffer_put(struct __kfifo *ring_buf, u8 *buffer, fifo_size_t size)
{
    fifo_size_t len = 0;
    size = min(size, ring_buf->size - ring_buf->in + ring_buf->out);
    /* first put the data starting from fifo->in to buffer end */
    len  = min(size, ring_buf->size - (ring_buf->in & (ring_buf->size - 1)));
    __fifo_memcpy(ring_buf->buffer + (ring_buf->in & (ring_buf->size - 1)), buffer, len);
    /* then put the rest (if any) at the beginning of the buffer */
    __fifo_memcpy(ring_buf->buffer, buffer + len, size - len);
    ring_buf->in += size;
    return size;
}

//获取已使用缓冲区的长度
//*ring_buf:缓冲区结构的地址
//返回值:缓冲区的长度
fifo_size_t ring_buffer_len_used(const struct __kfifo *ring_buf)
{
    fifo_size_t len = 0;
    pthread_mutex_lock();
    len = __ring_buffer_len(ring_buf);
    pthread_mutex_unlock();
    return len;
}

//获取未使用缓冲区的长度
//*ring_buf:缓冲区结构的地址
//返回值:缓冲区的长度
fifo_size_t ring_buffer_len_unused(const struct __kfifo *ring_buf)
{
    fifo_size_t len = 0;
    pthread_mutex_lock();
    len = ring_buf->size  -  __ring_buffer_len(ring_buf);
    pthread_mutex_unlock();
    return len;
}

//从缓冲区中取数据
//*ring_buf:缓冲区结构的地址
//* buffer:目的缓存区的地址
//size:要求取出的数据字节数
//返回值:实际取出的数据字节数
fifo_size_t ring_buffer_get(struct __kfifo *ring_buf, u8 *buffer, fifo_size_t size)
{
    fifo_size_t ret;
    pthread_mutex_lock();
    ret = __ring_buffer_get(ring_buf, buffer, size);
    //buffer中没有数据
    if (ring_buf->in == ring_buf->out){
    	ring_buf->in = 0;
        ring_buf->out = 0;
    }
    pthread_mutex_unlock();
    return ret;
}

//向缓冲区中存放数据
//*ring_buf:缓冲区结构的地址
//* buffer:数据源缓存区的地址
//size:要求存入缓冲区的数据字节数
//返回值:实际存入缓冲区的数据字节数
fifo_size_t ring_buffer_put(struct __kfifo *ring_buf, u8 *buffer, fifo_size_t size)
{
    fifo_size_t ret;
    pthread_mutex_lock();
    ret = __ring_buffer_put(ring_buf, buffer, size);
    pthread_mutex_unlock();
    return ret;
}

//从缓冲区中读一个字节的数据，但不改变缓冲区
//*ring_buf:缓冲区结构的地址
//size:读第几个字节的数据
//返回值:实际取出的数据字节数
u8 ring_buffer_get_byte(struct __kfifo *ring_buf, fifo_size_t size)
{
   return ring_buf->buffer[(ring_buf->out + size) & (ring_buf->size - 1)];
}

//缓冲区初始化
//*ring_buf:缓冲区结构的地址
//* buffer:数据源缓存区的地址
//size:要求存入缓冲区的数据字节数
//返回值:实际存入缓冲区的数据字节数
void ring_buffer_init(struct __kfifo *ring_buf)
{
	if( ring_buf == NULL )
		return;
    ring_buf->in = 0;
	ring_buf->out = 0;
}


