/**@brief ����linux kfifoд��ring buffer
 *@atuher   river date:2015-12-29
* ring_buffer.c
 * */
#include "sys.h"
#include "ring_buffer.h" 

/*
#define MY_FIFO_SIZE  1024  //size must be power of 2. 
u8 my_fifo[MY_FIFO_SIZE] = {0};



//�������ṹ����ͳ�ʼ��
struct __kfifo my_fifo_dev=
{
    my_fifo,    //������
    MY_FIFO_SIZE,  //��С
    0,       	//���λ��
    0,        	//����λ��
};*/


//���������������߳�ϵͳ��ʹ��
 void pthread_mutex_lock(void)
 {

 }
 void pthread_mutex_unlock(void)
 {

 }
 
//�����ڴ�
//*des:Ŀ�ĵ�ַ
//*src:Դ��ַ
//n:��Ҫ���Ƶ��ڴ泤��(�ֽ�Ϊ��λ)
void __fifo_memcpy(void *des,void *src,fifo_size_t n)  
{  
    u8 *xdes=des;
	u8 *xsrc=src; 
    while(n--)*xdes++=*xsrc++;  
} 

//��ȡ�������ĳ���
//*ring_buf:�������ṹ�ĵ�ַ
//����ֵ:�������ĳ���
fifo_size_t __ring_buffer_len(const struct __kfifo *ring_buf)
{
    return (ring_buf->in - ring_buf->out);
}

//�ӻ�������ȡ����
//*ring_buf:�������ṹ�ĵ�ַ
//* buffer:Ŀ�Ļ������ĵ�ַ
//size:Ҫ��ȡ���������ֽ���
//����ֵ:ʵ��ȡ���������ֽ���
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

//�򻺳����д������
//*ring_buf:�������ṹ�ĵ�ַ
//* buffer:����Դ�������ĵ�ַ
//size:Ҫ����뻺�����������ֽ���
//����ֵ:ʵ�ʴ��뻺�����������ֽ���
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

//��ȡ��ʹ�û������ĳ���
//*ring_buf:�������ṹ�ĵ�ַ
//����ֵ:�������ĳ���
fifo_size_t ring_buffer_len_used(const struct __kfifo *ring_buf)
{
    fifo_size_t len = 0;
    pthread_mutex_lock();
    len = __ring_buffer_len(ring_buf);
    pthread_mutex_unlock();
    return len;
}

//��ȡδʹ�û������ĳ���
//*ring_buf:�������ṹ�ĵ�ַ
//����ֵ:�������ĳ���
fifo_size_t ring_buffer_len_unused(const struct __kfifo *ring_buf)
{
    fifo_size_t len = 0;
    pthread_mutex_lock();
    len = ring_buf->size  -  __ring_buffer_len(ring_buf);
    pthread_mutex_unlock();
    return len;
}

//�ӻ�������ȡ����
//*ring_buf:�������ṹ�ĵ�ַ
//* buffer:Ŀ�Ļ������ĵ�ַ
//size:Ҫ��ȡ���������ֽ���
//����ֵ:ʵ��ȡ���������ֽ���
fifo_size_t ring_buffer_get(struct __kfifo *ring_buf, u8 *buffer, fifo_size_t size)
{
    fifo_size_t ret;
    pthread_mutex_lock();
    ret = __ring_buffer_get(ring_buf, buffer, size);
    //buffer��û������
    if (ring_buf->in == ring_buf->out){
    	ring_buf->in = 0;
        ring_buf->out = 0;
    }
    pthread_mutex_unlock();
    return ret;
}

//�򻺳����д������
//*ring_buf:�������ṹ�ĵ�ַ
//* buffer:����Դ�������ĵ�ַ
//size:Ҫ����뻺�����������ֽ���
//����ֵ:ʵ�ʴ��뻺�����������ֽ���
fifo_size_t ring_buffer_put(struct __kfifo *ring_buf, u8 *buffer, fifo_size_t size)
{
    fifo_size_t ret;
    pthread_mutex_lock();
    ret = __ring_buffer_put(ring_buf, buffer, size);
    pthread_mutex_unlock();
    return ret;
}

//�ӻ������ж�һ���ֽڵ����ݣ������ı仺����
//*ring_buf:�������ṹ�ĵ�ַ
//size:���ڼ����ֽڵ�����
//����ֵ:ʵ��ȡ���������ֽ���
u8 ring_buffer_get_byte(struct __kfifo *ring_buf, fifo_size_t size)
{
   return ring_buf->buffer[(ring_buf->out + size) & (ring_buf->size - 1)];
}

//��������ʼ��
//*ring_buf:�������ṹ�ĵ�ַ
//* buffer:����Դ�������ĵ�ַ
//size:Ҫ����뻺�����������ֽ���
//����ֵ:ʵ�ʴ��뻺�����������ֽ���
void ring_buffer_init(struct __kfifo *ring_buf)
{
	if( ring_buf == NULL )
		return;
    ring_buf->in = 0;
	ring_buf->out = 0;
}


