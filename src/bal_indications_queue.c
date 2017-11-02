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

#include "bal_indications_queue.h"

/*
 * Implements a circular queue whose size can grow upto MAX_QUEUE_SIZE defined.
 * Once MAX_QUEUE_SIZE is reached, newest nodes start replacing oldest nodes
 * in the circular queue.
 * We define two APIs below, one to get the node from the queue, another to
 * add node to the queue.
 */

list_node* get_bal_indication_node(void) {
   /*'bal_ind_queue_head' should be thread safe there could be simultaneous
     write from BAL and adapter trying to read from the 'bal_ind_queue_head'.
     So, protect it under a mutex(bal_ind_queue_lock), before using this MACRO*/
   list_node *node = NULL;
   if (num_of_nodes == 1) {
       node = bal_ind_queue_head;
       bal_ind_queue_head = NULL;
       bal_ind_queue_tail = NULL;
       num_of_nodes = 0;
   }
   else if(num_of_nodes > 1) {
       /*Process if there is at least one node in the queue*/
       node = bal_ind_queue_head;
       bal_ind_queue_head = bal_ind_queue_head->next;
       bal_ind_queue_tail->next = bal_ind_queue_head;
       --num_of_nodes;
   }

   return node;
}

void add_bal_indication_node(list_node *node) {
   /*'bal_ind_queue_head' should be thread safe there could be simultaneous
     write from BAL and adapter trying to read from the 'bal_ind_queue_head'.
     So, protect it under a mutex(bal_ind_queue_lock), before using this MACRO*/
   if (num_of_nodes == 0 && 
       bal_ind_queue_head == NULL && 
       bal_ind_queue_tail == NULL ) {
      /*Case where the queue is empty*/
      bal_ind_queue_head = node;
      bal_ind_queue_tail = node;
      bal_ind_queue_head->next = node;
      bal_ind_queue_tail->next = node;
      ++num_of_nodes;
   }
   else if (num_of_nodes == 1 && 
            bal_ind_queue_head != NULL && 
            bal_ind_queue_tail != NULL ) {
      bal_ind_queue_head->next = node;
      bal_ind_queue_tail = node;
      bal_ind_queue_tail->next = bal_ind_queue_head;
      ++num_of_nodes;
   }
   else if (num_of_nodes > 1 && 
            num_of_nodes < MAX_QUEUE_SIZE && 
            bal_ind_queue_head != NULL && 
            bal_ind_queue_tail != NULL ) {
      bal_ind_queue_tail->next = node;
      bal_ind_queue_tail = node;
      bal_ind_queue_tail->next = bal_ind_queue_head;
      ++num_of_nodes;
   }
   else if (num_of_nodes == MAX_QUEUE_SIZE && 
            bal_ind_queue_head != NULL && 
            bal_ind_queue_tail != NULL  ) {
      list_node *head_ref = bal_ind_queue_head;
      bal_ind_queue_tail->next = node;
      bal_ind_queue_tail = node;
      bal_ind_queue_head = bal_ind_queue_head->next;
      bal_ind_queue_tail->next = bal_ind_queue_head;
      bal_get_ind__free_mem(head_ref->bal_indication);
      free(head_ref);
   }
}
