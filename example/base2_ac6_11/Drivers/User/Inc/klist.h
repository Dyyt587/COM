/*
 * @Description: 移植Linux2.6内核list.h
 * @Version: V1.0
 * @Autor: https://blog.csdn.net/qq_16933601
 * @Date: 2020-09-12 22:54:51
 * @LastEditors: Carlos
 * @LastEditTime: 2020-09-16 00:35:17
 */
#ifndef KLIST_H
#define KLIST_H


#include "stdio.h"
/**
 * list示例
*/
// struct Person
// {
// 	char name[20];
// 	int age;
// 	struct list_head mylist;	//其中 list_head 在kernel_list.h 中有定义，这个结构体就只有两个结构体指针（next和prev）
// };

// int main()
// {
// 	struct Person *p;
// 	INIT_LIST_HEAD(&(p->mylist));		//主要是使用 INIT_LIST_HEAD（）这个宏初始化那个链表。
// 	return 0;
// }


//原来链表删除后指向的位置，这里我们修改成 0
// # define POISON_POINTER_DELTA 0
// #define LIST_POISON1  ((void *) 0x00100100 + POISON_POINTER_DELTA)
// #define LIST_POISON2  ((void *) 0x00200200 + POISON_POINTER_DELTA)
//#define NULL (0)
#define LIST_POISON1  NULL
#define LIST_POISON2  NULL

//计算member在type中的位置
#ifndef offsetof
#define offsetof(type, member)  (size_t)(&((type*)0)->member)
#endif
//根据member的地址获取type的起始地址
#define container_of(ptr, type, member) ({          \
        const __typeof__(((type *)0)->member)*__mptr = (ptr);    \
    (type *)((char *)__mptr - offsetof(type, member)); })

//链表结构
struct list_head {
    struct list_head *prev;
    struct list_head *next;
};
#define LIST_HEAD_INIT(name) { &(name), &(name) }

#define LIST_HEAD(name) \
    struct list_head name = LIST_HEAD_INIT(name)

static inline void INIT_LIST_HEAD(struct list_head *list) {
    list->next = list;
    list->prev = list;
}

static inline void init_list_head(struct list_head *list) {
    list->prev = list;
    list->next = list;
}

#ifndef CONFIG_DEBUG_LIST

static inline void __list_add(struct list_head *_new,
                              struct list_head *prev,
                              struct list_head *next) {
    next->prev = _new;
    _new->next = next;
    _new->prev = prev;
    prev->next = _new;
    //print("tt");
}

#else
extern void __list_add(struct list_head *_new,
                  struct list_head *prev,
                  struct list_head *next);
#endif

//从头部添加
/**
 * list_add - add a _new entry
 * @_new: _new entry to be added
 * @head: list head to add it after
 *
 * Insert a _new entry after the specified head.
 * This is good for implementing stacks.
 */
#ifndef CONFIG_DEBUG_LIST

static inline void list_add(struct list_head *_new, struct list_head *head) {
    __list_add(_new, head, head->next);
}

#else
extern void list_add(struct list_head *_new, struct list_head *head);
#endif

//从尾部添加
static inline void list_add_tail(struct list_head *_new, struct list_head *head) {
    __list_add(_new, head->prev, head);
}

static inline void __list_del(struct list_head *prev, struct list_head *next) {
    prev->next = next;
    next->prev = prev;
}

static inline void list_del(struct list_head *entry) {
    __list_del(entry->prev, entry->next);
    entry->next = LIST_POISON1;
    entry->prev = LIST_POISON2;
}


static inline void __list_splice(struct list_head *list,
                                 struct list_head *head) {
    struct list_head *first = list->next;
    struct list_head *last = list->prev;
    struct list_head *at = head->next;

    first->prev = head;
    head->next = first;

    last->next = at;
    at->prev = last;
}

/**
 * list_empty - tests whether a list is empty
 * @head: the list to test.
 */
static inline int list_empty(const struct list_head *head) {
    return head->next == head;
}

/**
 * list_splice - join two lists
 * @list: the _new list to add.
 * @head: the place to add it in the first list.
 */
static inline void list_splice(struct list_head *list, struct list_head *head) {
    if (!list_empty(list))
        __list_splice(list, head);
}

/**
 * list_replace - replace old entry by _new one
 * @old : the element to be replaced
 * @_new : the _new element to insert
 *
 * If @old was empty, it will be overwritten.
 */
static inline void list_replace(struct list_head *old,
                                struct list_head *_new) {
    _new->next = old->next;
    _new->next->prev = _new;
    _new->prev = old->prev;
    _new->prev->next = _new;
}
///**
// * list_replace - replace old entry by _new one
// * @old : the element to be replaced
// * @_new : the _new element to insert
// *
// * If @old was empty, it will be overwritten.
// */
//static inline void list_remove(struct list_head *old)
//{
//   if(old->next == old){
//       old->prev->next=old->prev;
//   }else{
//       __list_del(old->prev,old->next);
//   }
//}

static inline void list_replace_init(struct list_head *old,
                                     struct list_head *_new) {
    list_replace(old, _new);
    INIT_LIST_HEAD(old);
}

/**
 * list_move - delete from one list and add as another's head
 * @list: the entry to move
 * @head: the head that will precede our entry
 */
static inline void list_move(struct list_head *list, struct list_head *head) {
    __list_del(list->prev, list->next);
    list_add(list, head);
}

/**
 * list_move_tail - delete from one list and add as another's tail
 * @list: the entry to move
 * @head: the head that will follow our entry
 */
static inline void list_move_tail(struct list_head *list,
                                  struct list_head *head) {
    //printf("test110\n");
    __list_del(list->prev, list->next);
    //printf("test111\n");
    list_add_tail(list, head);
}

#define list_entry(ptr, type, member) \
    container_of(ptr, type, member)

#define list_first_entry(ptr, type, member) \
    list_entry((ptr)->next, type, member)

#define list_for_each(pos, head) \
    for (pos = (head)->next; pos != (head); pos = pos->next)
/**
 * list_for_each_entry	-	iterate over list of given type
 * @pos:	the type * to use as a loop cursor.
 * @head:	the head for your list.
 * @member:	the name of the list_struct within the struct.
 */
#define list_for_each_entry(pos, head, member)      \
    for (pos = list_entry((head)->next, typeof(*pos), member);    \
         &pos->member != (head);    \
         pos = list_entry(pos->member.next, typeof(*pos), member))
/**
 * list_for_each_prev	-	iterate over a list backwards
 * @pos:	the &struct list_head to use as a loop cursor.
 * @head:	the head for your list.
 */
#define list_for_each_prev(pos, head) \
    for (pos = (head)->prev;  pos != (head); \
            pos = pos->prev)

/**
 * list_move_tail - delete from one list and add as another's tail
 * @list: the entry to move
 * @head: the head that will follow our entry
 */
static inline int list_length(struct list_head *list) {
    int cnt=0;
    while(list->next != list){
        cnt++;
    }
    return cnt;
}

#endif
