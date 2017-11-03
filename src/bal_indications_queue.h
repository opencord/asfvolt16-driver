/*
** Copyright 2017-present Open Networking Foundation
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
** http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
*/

#ifndef __BAL_INDICATIONS_QUEUE_H__
#define __BAL_INDICATIONS_QUEUE_H__

#include "bal_indications.grpc-c.h"

#define MAX_QUEUE_SIZE 1000

typedef struct node {
    struct node *next;
    BalIndications *bal_indication;
}list_node;


extern pthread_mutex_t bal_ind_queue_lock;
extern list_node *bal_ind_queue_head;
extern list_node *bal_ind_queue_tail;
extern unsigned int num_of_nodes;

extern void bal_get_ind__free_mem(BalIndications *balIndCfg);
extern list_node* get_bal_indication_node(void);
extern void add_bal_indication_node(list_node *node);



#endif /*__BAL_INDICATIONS_QUEUE_H__*/
