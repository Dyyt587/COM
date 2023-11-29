/*
 * @Description: 移植Linux2.6内核list.h
 * @Version: V1.0
 * @Autor: dyyt
 * @Date: 2020-09-12 22:54:51
 * @LastEditors: Carlos
 * @LastEditTime: 2020-09-16 00:35:17
 */
#ifndef DQUEUE_H
#define DQUEUE_H

#include <stdint.h>
#include <string.h>
#include "klist.h"


/*
    dqueue_create(Dled,dled_t);
    dled_t dd;
    dled_t* get;
    dqueue_put(&dd,&Dled,dled_t,create);
    dqueue_get(get,dled_t,&Dled);
    dqueue_del(&Dled);
 */
typedef struct {
    char *name;
    void *body;
    struct list_head list;
    struct list_head gc_list;
} dqueue_t;


dqueue_t* __dqueue_get(dqueue_t* queue);

#define dqueue_create(_name, type) static dqueue_t _name \
={.name=#type,.body=0, .list=LIST_HEAD_INIT(_name.list),.gc_list=LIST_HEAD_INIT(_name.gc_list)}
#define dqueue_init(ptr, type)  \
ptr.name=#type;                 \
ptr.body=0;                     \
INIT_LIST_HEAD(&(ptr.list)); \
INIT_LIST_HEAD(&(ptr.gc_list))
//不一定安全，存在body从gc队列回归的内存泄露问题
#define dqueue_put(____new, dqueue, type, init, ...) \
    {  if(!(dqueue))while(1);                                         \
    if(strcmp((dqueue)->name,#type)==0){  \
    dqueue_t* queue;\
       if(list_empty(&((dqueue)->gc_list))){ \
              queue = (dqueue_t*)malloc(sizeof(dqueue_t)); \
       }else{                            \
       struct list_head* __list=(dqueue)->gc_list.next;    \
       list_del(__list);                       \
       queue = list_entry(__list,dqueue_t,list); \
       }                                     \
       init(____new, ##__VA_ARGS__);                      \
       queue->body=(void*)____new;                  \
       list_add_tail(&(queue->list),&((dqueue)->list));    \
    }else{                                \
        while(1){}                                      \
    }}
	#define dqueue_put_p(____new, dqueue, type, init, ...) \
    {  if(!(dqueue))while(1);                                         \
    if(strcmp((dqueue)->name,#type)==0){  \
    dqueue_t* queue;\
       if(list_empty(&((dqueue)->gc_list))){ \
              queue = (dqueue_t*)malloc(sizeof(dqueue_t)); \
				 ____new = malloc(sizeof(type));\
       }else{                            \
       struct list_head* __list=(dqueue)->gc_list.next;    \
       list_del(__list);                       \
       queue = list_entry(__list,dqueue_t,list); \
				 ____new = queue->body;\
       }                                     \
       init(____new, ##__VA_ARGS__);                      \
       queue->body=(void*)____new;                  \
       list_add_tail(&(queue->list),&((dqueue)->list));    \
    }else{                                \
        while(1){}                                      \
    }}
		
//#define dqueue_put_p(____new,dqueue, type, init, ...) \
//    {  if(!(dqueue))while(1);                            \
//    if(strcmp((dqueue)->name,#type)==0){  \
//    dqueue_t* queue;\
//       if(list_empty(&((dqueue)->gc_list))){ \
//              queue = (dqueue_t*)malloc(sizeof(dqueue_t)); \
//				        init(____new, ##__VA_ARGS__);                  \
//       queue->body=(void*)____new;                  \
//       }else{                            \
//       struct list_head* __list=(dqueue)->gc_list.next;      \
//       list_del(__list);                       \
//       queue = list_entry(__list,dqueue_t,list); \
//				____new= queue->body;                  \
//       init(____new, ##__VA_ARGS__);                                  \
//       }                                     \
//       list_add_tail(&(queue->list),&((dqueue)->list));    \
//    }else{                                \
//        while(1){}                                      \
//    }}


#define dqueue_get(ptr, type, dqueue) {\
	dqueue_t *__##type##__COUNTER__##queue = __dqueue_get(dqueue);\
		if(__##type##__COUNTER__##queue){\
			    ptr=(type*)__##type##__COUNTER__##queue->body ;\
		}else ptr=0; }

#define dqueue_del(dqueue) \
    if(!(dqueue))while(1); if(!list_empty(&((dqueue)->list)))\
    list_move_tail((dqueue)->list.next,&((dqueue)->gc_list));
//#define dqueue_del(dqueue) \
//    if(!(dqueue))while(1); if(!list_empty(&((dqueue)->list)))\
//		free((void*)((dqueue)->body));\
//    list_move_tail((dqueue)->list.next,&((dqueue)->gc_list));
//		#define dqueue_del(dqueue) \


#endif

