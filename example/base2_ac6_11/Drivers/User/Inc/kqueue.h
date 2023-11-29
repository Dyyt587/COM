#ifndef KQUEUE_H
#define KQUEUE_H

#include "klist.h"
#define QUEUE_CREATE(name) LIST_HEAD(name)
#define  queue_add( new, queue) {                               \
          list_add(new->list,queue);                             \
}

#define  queue_get(queue, type, member) list_empty(queue) ? 0:list_first_entry(queue, type, member)

#define  queue_del(queue) { \
          if(!list_empty(queue)) { \
             list_del(queue.next);               \
          }                 \
                                     \
}











#endif