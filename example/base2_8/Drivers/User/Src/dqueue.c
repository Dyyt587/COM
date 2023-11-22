#include "klist.h"
#include "dqueue.h"
#include "stdint.h"
#include <string.h>
#include <limits.h>

dqueue_t* __dqueue_get(dqueue_t* queue)
{
	if (!(queue))
        while (1) ;
    if((strcmp((queue)->name, "com_task_t") == 0) && (!list_empty(&((queue)->list)))) 
		{
			return list_first_entry(&(queue->list),dqueue_t,list);
		}else{
			return 0;
		}
}

