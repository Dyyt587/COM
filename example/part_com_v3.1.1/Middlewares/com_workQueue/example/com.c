/**
  ******************************************************************************
  * @file           : com.c
  * @author         : 80520
  * @brief          : None
  * @attention      : None
  * @date           : 2023/12/3
  ******************************************************************************
  */
#include "com.h"


#define __COM_DRIVER_CHECK(driver)    \
    do{\
        if(driver == NULL)\
        {\
            return COM_FAIL_NULL_POINTER;\
        }\
    }while(0)

//创建一个工作调度器
//模拟串口发送接收数据
#define COM_CHECK_ID(id) do{\
    if(id >= COM_NUM){\
        return COM_INVALID_ID;\
    }                       \
}while(0)

#define COM_CHECK_ID_P(id) do{\
    if(id >= COM_NUM){\
        return 0;\
    }                       \
}while(0)


com_t __coms[COM_NUM];

int com_init(void){
    COM_DEVICE_TABLE();//初始化软件相关
    return 0;
}
int com_init_mode(uint8_t com,uint32_t mode){
    COM_CHECK_ID(com);
    __coms[com].mode = mode;
    return 0;
}
int com_register_driver(uint8_t com, ll_driver_t driver){
    //检查com id是否合法
    COM_CHECK_ID(com);
    //注册驱动
    __coms[com].driver = driver;
    return COM_SUCCESS;
}

int com_try_communite(uint8_t com) {
    COM_CHECK_ID(com);
    int rec = 0;
	
	
	//查看任务是否是需要二次的
	
	
	
	
	
    //尝试获取任务
    rec = com_wq_get_task(&__coms[com].ws_put, &__coms[com].tasking_put);
    //判断获取成功
    if (rec != WQ_SUCCESS) {
        return rec;
    }
    //成功获取任务

    //调用底层驱动发送数据
    if(__coms[com].tasking_put.user_mode == COM_RX){

          //判断数据是否需要分两次接收
          uint32_t free_len = kfifo_get_free_block(__coms[com].rx_fifo);
          if(__coms[com].tasking_put.len > free_len) {
                //__coms[com].tx_len = __coms[com].tasking_put.len-free_len;
                __coms[com].tasking_put.user_mode+=1;
                __COM_DRIVER_CHECK(__coms[com].driver);
                // __coms[com].tasking_put.len = __coms[com].tasking_put.len-free_len;
                rec = __coms[com].driver(kfifo_get_in_pointer(__coms[com].rx_fifo),
                                         free_len,0,0, __coms[com].tasking_put.user_mode);
                ////////////////////
          } else {
                //不需要分两次
                __COM_DRIVER_CHECK(__coms[com].driver);
                rec = __coms[com].driver(
                        kfifo_get_in_pointer(__coms[com].rx_fifo),
                      __coms[com].tasking_put.len,0,0,
                        __coms[com].tasking_put.user_mode);
          }
          return rec;
    }else{
			
			
			//////////////发送函数//////
			
        uint32_t free_len = kfifo_get_free_block(__coms[com].tx_fifo);
        if(__coms[com].tasking_put.len > free_len) {
            //__coms[com].tx_len = __coms[com].tasking_put.len-free_len;
            __coms[com].tasking_put.user_mode+=1;
            __COM_DRIVER_CHECK(__coms[com].driver);
            // __coms[com].tasking_put.len = __coms[com].tasking_put.len-free_len;
            rec = __coms[com].driver(kfifo_get_out_pointer(__coms[com].tx_fifo),
                                     free_len,0,0,  __coms[com].tasking_put.user_mode);
            ////////////////////
        } else {
            //不需要分两次发送
            __COM_DRIVER_CHECK(__coms[com].driver);
            rec = __coms[com].driver(
                    kfifo_get_out_pointer(__coms[com].tx_fifo),
                    __coms[com].tasking_put.len,0,0,
                    __coms[com].tasking_put.user_mode);
        }
        return rec;
    }

}
/**?
 * @brief com口接收数据
 * @param com
 * @param addr
 * @param buf
 * @param len
 * @return
 * @note 调用该函数,表示会主动等待数据，建议iic spi使用，对于串口，会进行TODO:!!!!!!!!!!!!!!!!!!进行啥
 */
int com_rec_addr(uint8_t com, uint32_t  addr, uint8_t *buf, uint32_t len,uint32_t wait_ms) {
    int rec=0;
    //检查com id是否合法
    COM_CHECK_ID(com);

    //创建一个task
    work_task_t task;
    task.len = len;
    task.user_mode = COM_RX;
    task.pdata = buf;
    task.addr = 0;//0表示不需要addr

    //放入任务
    rec = com_wq_put_task(&__coms[com].ws_put, &task);
    //判断放入成功
    if(rec != WQ_SUCCESS){
        return rec;
    }

    int debug  = com_try_communite(com);
    return rec;
}

int com_rec(uint8_t com, uint8_t *buf, uint32_t len,uint32_t wait_ms) {
    return com_rec_addr(com,0,buf,len,wait_ms);
}
int com_send_addr(uint8_t com,uint32_t  addr,uint8_t *buf, uint32_t len,uint32_t wait_ms){
    int rec = 0;
    //检查com id是否合法
    COM_CHECK_ID(com);
    //创建一个task
    work_task_t task;
    task.len = len;
    task.user_mode = COM_TX;
    task.pdata = buf;//注意接收数据的时候，这个指针是无效的
    task.addr = addr;//0表示不需要addr

		//放入数据
		kfifo_put(__coms[com].tx_fifo,buf,len);
    //放入任务
    rec = com_wq_put_task(&__coms[com].ws_put, &task);

    //判断放入成功
    if(rec != WQ_SUCCESS){
        return rec;
    }

    int debug  = com_try_communite(com);
    return rec;
}
int com_send(uint8_t com,uint8_t *buf, uint32_t len,uint32_t wait_ms) {
    return com_send_addr(com,0,buf,len,wait_ms);
}
int com_cplt(int com,int data_size)
{
    COM_CHECK_ID(com);
    int rec=0;
    //如果自动获取，这表示使用tasking中的数据
/*    if(data_size == COM_AUTO_SIZE){
        data_size = __coms[com].tasking_put.len;
    }*/
    if((COM_TX_CONTINUE == __coms[com].tasking_put.user_mode)) {
        //需要发送到下半部分
        //获取缓冲区首地址
        uint32_t free_len = kfifo_get_free_block(__coms[com].tx_fifo);
       // __coms[com].tasking_put.len-=free_len;
        __coms[com].tasking_put.user_mode=COM_TX;
         rec = __coms[com].driver(__coms[com].tx_fifo->buffer,
                                     __coms[com].tasking_put.len-free_len,0,0,
                                  __coms[com].tasking_put.user_mode);//发送完成
        return rec;
    }else if(COM_RX_CONTINUE == __coms[com].tasking_put.user_mode){
        uint32_t free_len = kfifo_get_free_block(__coms[com].rx_fifo);
        //__coms[com].tasking_put.len-=free_len;//接收需要保留len以确保后面的自动入库
        __coms[com].tasking_put.user_mode--;
         rec = __coms[com].driver(__coms[com].rx_fifo->buffer,
                                     __coms[com].tasking_put.len-free_len,0,0,
                                  __coms[com].tasking_put.user_mode);//发送完成
        return rec;
    }else{
        if(data_size == COM_AUTO_SIZE){
            data_size = __coms[com].tasking_put.len;
        }
        //完成一个工作
         rec = com_wq_cplt_task(&__coms[com].ws_put);
				
				
        //如果是接收任务，需要更新接收缓存
        if(__coms[com].tasking_put.user_mode == COM_RX){
            kfifo_put_index(__coms[com].rx_fifo,data_size);
        }else{
            //发送任务
            kfifo_get_index(__coms[com].tx_fifo,data_size);
        }
    }
    //尝试发送下一次
    int debug  = com_try_communite(com);
    return rec;
}

int com_received(int com,uint32_t data_size0)//通常用于自动接收完成时
{
    //将收到的数据放入缓存
    COM_CHECK_ID(com);
    int rec=0;
    uint32_t puted  = kfifo_put_index(__coms[com].rx_fifo,data_size0);

    if(puted != data_size0){
        //缓存不够
        //WS_BUG("COM_RECEIVED_ERROR NO buf SIZE\n");
        //TODO:解决它
        while(1);
    }
    return 0;
}
/**?
 * @brief com口接收到的数据
 * @param com
 * @return
 */
uint32_t com_inquire_received(uint8_t com)
{
    COM_CHECK_ID(com);
    return kfifo_get_used(__coms[com].rx_fifo);
}

uint32_t com_read(uint8_t com,uint8_t*pdata,uint32_t length)
{
    COM_CHECK_ID(com);
    uint32_t  ret=0;
    ret = kfifo_get(__coms[com].rx_fifo,pdata,length);
    return ret;


}



uint32_t com_get_rxbuf_size(uint8_t com)
{
	COM_CHECK_ID(com);
	return __coms[com].rx_fifo->size;
}
uint8_t* com_get_rxbuf_addr(uint8_t com)
{
	COM_CHECK_ID_P(com);
	return __coms[com].rx_fifo->buffer;
}

uint8_t* com_get_rxbuf_in(uint8_t com)
{
	COM_CHECK_ID_P(com);
	return kfifo_get_in_pointer(__coms[com].rx_fifo);
}


int com_inquire_data(uint8_t com,uint8_t*pdata,uint32_t maxlength)
{
		COM_CHECK_ID_P(com);
return kfifo_get(__coms[com].rx_fifo,pdata,maxlength);
}






















































/*
int com_register_driver(uint8_t com, ll_driver_t driver){
    //检查com id是否合法
    COM_CHECK_ID(com);
    //注册驱动
    __coms[com].driver = driver;
    return COM_SUCCESS;
}
int com_try_communicate(uint8_t com){
    int rec = 0;
    work_task_t task;
    //尝试获取任务
    rec = ws_get_work_a(&__coms[com].ws, &task);
    //判断获取成功
    if(rec != WS_SUCCESS){
        return rec;
    }
    //成功获取任务
    //调用底层驱动发送数据
    //TODO:由于可能存在要发生或者接收两次已实现dma的操作，故需要在这里判断
    //TODO:如果是dma操作，需要将数据拷贝到dma缓冲区(目前的方案)
    if(task.user_mode == COM_RX){
        //接收数据
        //判断数据是否需要分两次发送
        uint32_t free_len = kfifo_get_free_block(__coms[com].ws.work_fifo_b);
        if(task.len > free_len) {
            __coms[com].rx_len = task.len-free_len;
            rec = __coms[com].driver(task.pdata, free_len,task.user_mode);
        } else {
            //不需要分两次发送
            rec = __coms[com].driver(task.pdata, task.len,task.user_mode);
        }
        return rec;
    }else{
        //判断数据是否需要分两次发送
        uint32_t free_len = kfifo_get_free_block(__coms[com].ws.work_fifo_a);
        if(task.len > free_len) {
            __coms[com].tx_len = task.len-free_len;
            rec = __coms[com].driver(task.pdata, free_len,task.user_mode);
        } else {
            //不需要分两次发送
            rec = __coms[com].driver(task.pdata, task.len,task.user_mode);
        }
        return rec;
    }

}
*/
/**
 * @brief com口发送数据
 * @param com
 * @param buf
 * @param len
 * @return
 *//*

int com_send(uint8_t com,uint8_t *buf, uint32_t len){
    int rec = 0;
    //检查com id是否合法
    COM_CHECK_ID(com);
    //放入任务
    rec = ws_post_a(&__coms[com].ws, 0, buf, len, COM_TX);
    //判断放入成功
    if(rec != WS_SUCCESS){
        return rec;
    }
    //尝试获取任务
    rec = com_try_communicate(com);
    //判断发送成功
    if(rec != WS_SUCCESS){
        WS_BUG("COM_PUT_BUSY\n");
        //return COM_PUT_BUSY;
    }
    rec=COM_SUCCESS;
    return rec;
}

int com_send_cplt(uint8_t com) {
    int rec = 0;
    //检查com id是否合法
    COM_CHECK_ID(com);

    //前面要先判断
    //判断任务类型，根据类型判断是否需要发送到什么地方
    if(__coms[com].tx_len>0) {
        //需要发送到下半部分
        //获取缓冲区首地址
        uint32_t len = __coms[com].tx_len;
        __coms[com].tx_len = 0;
        rec = __coms[com].driver(__coms[com].ws.work_fifo_a->buffer, len,COM_TX);//发送完成
        return rec;
    }
    //完成一个工作
    rec = ws_cplt_work_a(&__coms[com].ws);
    //判断完成成功
    if(rec != WS_SUCCESS){
        return rec;
    }
    //尝试获取任务
    rec = com_try_communicate(com);
    //判断发送成功
    if(rec != WS_SUCCESS){
        return COM_PUT_BUSY;
    }
    return rec;
}
*/
/**
 * @brief com口接收数据
 * @param com
 * @param buf
 * @param len
 * @return
 * @note 调用该函数前应该已经将数据放入任务队列的workfifo中
 *//*

int com_received(uint8_t com, uint32_t len){
    int rec = 0;
    //检查com id是否合法
    COM_CHECK_ID(com);
    //放入任务
    rec = ws_post_b(&__coms[com].ws,  0, len, 0);
    //判断放入成功
    if(rec != WS_SUCCESS){
        return rec;
    }
    return rec;
}
*/
/**
 * @brief com口接收数据
 * @param com
 * @param buf
 * @param len
 * @return
 * @note 调用该函数前应该已经将数据放入任务队列的workfifo中
 *//*

int com_receive(uint8_t com, uint32_t len){
    int rec = 0;
    //检查com id是否合法
    COM_CHECK_ID(com);
    //检测模式
    if(__coms[com].mode == hostControlledComMode){
        //主机控制模式
        //创建接收任务
        work_task_t task;
        task.len = len;
        task.user_mode = COM_RX;
        task.pdata = 0;

        //放入任务,仅仅放入任务
        rec = ws_post_a_task(&__coms[com].ws, &task);
        //判断放入成功
        if(rec != WS_SUCCESS){
            return rec;
        }
        return rec;
    }else{
        //TODO:有改变！！！！！！！！！！！！！！！！！！！！！！！！！
        //主机控制模式
        //创建接收任务
        work_task_t task;
        task.len = len;
        task.user_mode = COM_RX;
        task.pdata = 0;

        //放入任务,仅仅放入任务
        rec = ws_post_a_task(&__coms[com].ws, &task);
        //判断放入成功
        if(rec != WS_SUCCESS){
            return rec;
        }
        return rec;
    }
    //放入任务
    //rec = ws_post_b(&__coms[com].ws,  0, len, 0);
    //判断放入成功
*/
/*    if(rec != WS_SUCCESS){
        return rec;
    }*//*

    return rec;
}

*/
/**
 * @brief com口接收数据
 * @param com
 * @param buf
 * @param len
 * @return
 * @note 调用该函数前应该已经将数据放入任务队列的workfifo中
 *//*

uint8_t* com_get_rxbuf_addr(uint8_t com){
    int rec = 0;
    //检查com id是否合法
    if(com >= COM_NUM){\
        return 0;\
    }    //放入任务

    return __coms[com].ws.work_fifo_b->buffer;
}
*/
/**
 * @brief com口接收数据
 * @param com
 * @param buf
 * @param len
 * @return
 * @note 调用该函数前应该已经将数据放入任务队列的workfifo中
 *//*

uint32_t com_get_rxbuf_size(uint8_t com){
    int rec = 0;
    //检查com id是否合法
    if(com >= COM_NUM){\
        return 0;\
    }    //放入任务

    return __coms[com].ws.work_fifo_b->size;
}
*/
/**
 * @brief com口接收数据
 * @param com
 * @param buf
 * @param len
 * @return
 * @note 调用该函数前应该已经将数据放入任务队列的workfifo中
 *//*

uint32_t com_get_rx_data(uint8_t com,uint8_t*pdata,uint32_t length){
    uint32_t rec = 0;
    //检查com id是否合法
    if(com >= COM_NUM){\
        return 0;\
    }    
    rec = kfifo_get(__coms[com1].ws.work_fifo_b,pdata,length);

    return rec;
}
*/
/**
 * @brief com口接收数据
 * @param com
 * @param buf
 * @param len
 * @return
 * @note 调用该函数前应该已经将数据放入任务队列的workfifo中
 *//*

uint8_t* com_get_rxbuf_in(uint8_t com){
    int rec = 0;
    //检查com id是否合法
    if(com >= COM_NUM){\
        return 0;\
    }    //放入任务

    return (uint8_t*)kfifo_get_in_pointer(__coms[com].ws.work_fifo_b);
}

//获取一个包的数据接收
int com_get_pack(uint8_t com,uint8_t*pdata,uint32_t maxlength){
    int rec = 0;
    //检查com id是否合法
    if(com >= COM_NUM){\
        return 0;\
    }
    //获取一个接收task
    work_task_t task;

    rec = ws_get_work_b(&__coms[com].ws, &task);
    if(rec != WS_SUCCESS){

        return rec;
    }
    //对比数据大小是否足够
    if(task.len > maxlength){
        //数据不够
        //打印错误
        WS_BUG("COM_GET_PACK_ERROR NO buf SIZE\n");        
        while(1);
    }
            //复制数据
    memcpy(pdata,task.pdata,task.len);
    return rec;   
}*/
