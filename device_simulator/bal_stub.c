#ifdef BAL_STUB
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include "bal_stub.h"
#include "bal_msg_type.grpc-c.h"
#include "bal_osmsg.grpc-c.h"
#include "bal_model_ids.grpc-c.h"
#include "bal_obj.grpc-c.h"
#include "bal_model_types.grpc-c.h"
#include "bal_errno.grpc-c.h"
#include "bal_indications.grpc-c.h"
#include "bal.grpc-c.h"

void *stub_thread(void *v) 
{
   int status;
   grpc_c_client_t *client = grpc_c_client_init("172.24.150.114:60001", "bal_client", NULL);
   pthread_mutex_lock(&lock);
   pthread_cond_wait(&cv, &lock);
   while(NULL != shared_queue->front)
   {
      BalObjId prevObjType;
      struct QNode *front = deQueue(shared_queue);
      /* prepare and send rpc response */
      BalIndications balIndCfg;
      memset(&balIndCfg, 0, sizeof(BalIndications));
      bal_indications__init(&balIndCfg);
      BalObjInd bal_obj_ind;
      memset(&bal_obj_ind, 0, sizeof(BalObjInd));
      bal_obj_ind__init(&bal_obj_ind);
      balIndCfg.u_case = BAL_INDICATIONS__U_BAL_OBJ_INFO;
      balIndCfg.balobjinfo = &bal_obj_ind;
      balIndCfg.balobjinfo->has_objtype = 1;
      balIndCfg.balobjinfo->objtype = front->obj_type;
      balIndCfg.balobjinfo->has_status = 1;
      balIndCfg.balobjinfo->status = front->status;
      char keystr;
      balIndCfg.balobjinfo->keystr = &keystr;
      switch(front->obj_type)
      {
         case BAL_OBJ_ID__BAL_OBJ_ID_ACCESS_TERMINAL:
            {
               printf("***************************************************\n");
               printf("Successful Indication sent for ACCESS_TERMINAL\n");
               printf("***************************************************\n");
               balIndCfg.balobjinfo->u_case = BAL_OBJ_IND__U__NOT_SET;
               prevObjType = front->obj_type;
            }
            break;
         case BAL_OBJ_ID__BAL_OBJ_ID_INTERFACE:
            {
               printf("***************************************************\n");
               printf("Successful Indication sent for PON INTERFACE UP\n");
               printf("***************************************************\n");
               balIndCfg.balobjinfo->u_case = BAL_OBJ_IND__U__NOT_SET;
               prevObjType = front->obj_type;
            }
            break;
         case BAL_OBJ_ID__BAL_OBJ_ID_SUBSCRIBER_TERMINAL:
            {
               balIndCfg.balobjinfo->u_case = BAL_OBJ_IND__U_ONU_DISCOVERY_INFO;
               BalSubscriberTerminalCfg onudiscoveryinfo;
               memset(&onudiscoveryinfo, 0, sizeof(BalSubscriberTerminalCfg));
               bal_subscriber_terminal_cfg__init(&onudiscoveryinfo);
               balIndCfg.balobjinfo->onudiscoveryinfo = &onudiscoveryinfo;

               BalSubscriberTerminalKey subTermKey;
               memset(&subTermKey, 0, sizeof(BalSubscriberTerminalKey));
               bal_subscriber_terminal_key__init(&subTermKey);
               balIndCfg.balobjinfo->onudiscoveryinfo->key = &subTermKey;
               balIndCfg.balobjinfo->onudiscoveryinfo->key->has_sub_term_id = 1;
               balIndCfg.balobjinfo->onudiscoveryinfo->key->sub_term_id = 65535;
               balIndCfg.balobjinfo->onudiscoveryinfo->key->has_intf_id = 1;
               balIndCfg.balobjinfo->onudiscoveryinfo->key->intf_id = 0;
               
               BalSubscriberTerminalCfgData subTermCfgData;
               memset(&subTermCfgData, 0, sizeof(BalSubscriberTerminalCfgData));
               bal_subscriber_terminal_cfg_data__init(&subTermCfgData);
               balIndCfg.balobjinfo->onudiscoveryinfo->data = &subTermCfgData;
               if(BAL_OBJ_ID__BAL_OBJ_ID_INTERFACE == prevObjType)
               {
                  balIndCfg.balobjinfo->onudiscoveryinfo->data->has_admin_state = 1;
                  balIndCfg.balobjinfo->onudiscoveryinfo->data->admin_state = BAL_STATE__BAL_STATE_DOWN;
                  balIndCfg.balobjinfo->onudiscoveryinfo->data->has_oper_status = 1;
                  balIndCfg.balobjinfo->onudiscoveryinfo->data->oper_status = BAL_STATUS__BAL_STATUS_DOWN;
                  printf("\n***************************************************\n");
                  printf("Sending ONU discovery message\n");
                  printf("***************************************************\n");
               }
               else
               {
                  balIndCfg.balobjinfo->onudiscoveryinfo->data->has_admin_state = 1;
                  balIndCfg.balobjinfo->onudiscoveryinfo->data->admin_state = BAL_STATE__BAL_STATE_UP;
                  balIndCfg.balobjinfo->onudiscoveryinfo->data->has_oper_status = 1;
                  balIndCfg.balobjinfo->onudiscoveryinfo->data->oper_status = BAL_STATUS__BAL_STATUS_UP;
                  printf("***************************************************\n");
                  printf("ONU Activation Successful\n");
                  printf("***************************************************\n");
               }
               BalSerialNumber serial_number;
               memset(&serial_number, 0, sizeof(BalSerialNumber));
               bal_serial_number__init(&serial_number);
               balIndCfg.balobjinfo->onudiscoveryinfo->data->serial_number = &serial_number;
               char vendor_id[20];
               memset(&vendor_id, 0, 20);
               strcpy(vendor_id,"4252434D");
               balIndCfg.balobjinfo->onudiscoveryinfo->data->serial_number->vendor_id = vendor_id;
               char vendor_specific[20];
               memset(&vendor_specific, 0, 20);
               strcpy(vendor_specific,"12345678");
               balIndCfg.balobjinfo->onudiscoveryinfo->data->serial_number->vendor_specific = vendor_specific;

               prevObjType = front->obj_type;
            }
            break;
         case BAL_OBJ_ID__BAL_OBJ_ID_PACKET:
            {
               balIndCfg.balobjinfo->u_case = BAL_OBJ_IND__U_PKT_DATA;
            }
            break;
         default:
            {
               balIndCfg.balobjinfo->u_case = BAL_OBJ_IND__U__NOT_SET;
               prevObjType = front->obj_type;
            }
            break;
      }
      BalErr *output;
      status = bal_ind__bal_ind_info(client, NULL, &balIndCfg, &output, NULL, 0);
      free(front);
      pthread_mutex_unlock(&lock);
      pthread_mutex_lock(&lock);
      pthread_cond_wait(&cv, &lock);
   }
   return NULL;
}

void create_stub_thread() 
{
   pthread_t threadId = 0;

   /* create shared queue */
   shared_queue = createQueue();

   pthread_create(&threadId, NULL, stub_thread, NULL);      

}

/* A utility function to create an empty queue */
bal_queue *createQueue()
{
   shared_queue = (struct Queue*)malloc(sizeof(struct Queue));
   shared_queue->front = shared_queue->rear = NULL;
   return shared_queue;
}

/* A utility function to create a new linked list node */
struct QNode* newNode(int objKey, int status)
{
   struct QNode *temp = (struct QNode*)malloc(sizeof(struct QNode));
   temp->obj_type = objKey;
   temp->status = status;
   temp->next = NULL;
   return temp; 
}

/* The function to add data to shared_queue - Add end of the queue */
void enQueue(int objKey, int status)
{
   /* Create a new LL node */
   struct QNode *temp = newNode(objKey, status);

   /* If queue is empty, then new node is front and rear both */
   if (shared_queue->rear == NULL)
   {
      shared_queue->front = shared_queue->rear = temp;
      return;
   }

   /* Add the new node at the end of queue and change rear */
   shared_queue->rear->next = temp;
   shared_queue->rear = temp;
}

/* Function to remove data from shared_queue - FIFO */
struct QNode *deQueue()
{
   /* If queue is empty, return NULL */
   if (shared_queue->front == NULL)
   {
      return NULL;
   }

   /* Store previous front and move front one node ahead */
   struct QNode *temp = shared_queue->front;
   shared_queue->front = shared_queue->front->next;

   /* If front becomes NULL, then change rear also as NULL */
   if (shared_queue->front == NULL)
   {
      shared_queue->rear = NULL;
   }

   return temp;
}

#endif
