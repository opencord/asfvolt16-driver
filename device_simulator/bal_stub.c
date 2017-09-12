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

char *voltha_ip_and_port = NULL;

void *stub_thread(void *v)
{
   int status;
   pthread_mutex_lock(&lock);
   pthread_cond_wait(&cv, &lock);
   while(NULL != shared_queue->front)
   {
      BalErr *output;
      BalObjId prevObjType;
      char vendor_id[20];
      char vendor_specific[20];
      struct QNode *front = deQueue(shared_queue);
      /* prepare and send rpc response */
      BalIndications balIndCfg;
      memset(&balIndCfg, 0, sizeof(BalIndications));
      bal_indications__init(&balIndCfg);
      balIndCfg.has_objtype = 1;
      balIndCfg.objtype = front->obj_type;
      balIndCfg.device_id = front->device_id;
      printf("Device Id = %s\n", front->device_id);
      switch(front->obj_type)
      {
         case BAL_OBJ_ID__BAL_OBJ_ID_ACCESS_TERMINAL:
            {
               printf("***************************************************\n");
               printf("Successful Indication sent for ACCESS_TERMINAL\n");
               printf("***************************************************\n");
               balIndCfg.u_case = BAL_INDICATIONS__U_ACCESS_TERM_IND;
               prevObjType = front->obj_type;
               BalAccessTerminalKey accTermKey;
               memset(&accTermKey, 0, sizeof(BalAccessTerminalKey));
               bal_access_terminal_key__init(&accTermKey);
               accTermKey.has_access_term_id = 1;
               accTermKey.access_term_id = 1;

               BalAccessTerminalIndData data;
               memset(&data, 0, sizeof(BalAccessTerminalIndData));
               bal_access_terminal_ind_data__init(&data);
               data.has_admin_state = 1;
               data.admin_state = BAL_STATE__BAL_STATE_UP;

               BalAccessTerminalInd access_term_ind;
               memset(&access_term_ind, 0, sizeof(BalAccessTerminalInd));
               bal_access_terminal_ind__init(&access_term_ind);
               access_term_ind.data = &data;
               access_term_ind.key = &accTermKey;
               balIndCfg.access_term_ind = &access_term_ind;
               status = bal_ind__bal_acc_term_ind(client, NULL, &balIndCfg, &output, NULL, 0);
            }
            break;
         case BAL_OBJ_ID__BAL_OBJ_ID_INTERFACE:
            {
               printf("***************************************************\n");
               printf("Successful Indication sent for PON INTERFACE UP\n");
               printf("***************************************************\n");
               balIndCfg.u_case = BAL_INDICATIONS__U_INTERFACE_IND;
               prevObjType = front->obj_type;
               status = bal_ind__bal_iface_ind(client, NULL, &balIndCfg, &output, NULL, 0);
            }
            break;
         case BAL_OBJ_ID__BAL_OBJ_ID_SUBSCRIBER_TERMINAL:
            {
               BalSubscriberTerminalKey subTermKey;
               memset(&subTermKey, 0, sizeof(BalSubscriberTerminalKey));
               bal_subscriber_terminal_key__init(&subTermKey);

               BalSerialNumber serial_number;
               memset(&serial_number, 0, sizeof(BalSerialNumber));
               bal_serial_number__init(&serial_number);

/*
               char vendor_id[20];
               memset(&vendor_id, 0, 20);
               strcpy(vendor_id,"4252434D");
               char vendor_specific[20];
               memset(&vendor_specific, 0, 20);
               strcpy(vendor_specific,"12345678");

               if(BAL_OBJ_ID__BAL_OBJ_ID_INTERFACE == prevObjType)
               {
                  balIndCfg.u_case = BAL_INDICATIONS__U_TERMINAL_DISC;
                  BalSubscriberTerminalSubTermDisc terminal_disc;
                  memset(&terminal_disc, 0, sizeof(BalSubscriberTerminalSubTermDisc));
                  bal_subscriber_terminal_sub_term_disc__init(&terminal_disc);
                  balIndCfg.terminal_disc = &terminal_disc;

                  balIndCfg.terminal_disc->key = &subTermKey;
                  balIndCfg.terminal_disc->key->has_sub_term_id = 1;
                  balIndCfg.terminal_disc->key->sub_term_id = front->onu_id;
                  balIndCfg.terminal_disc->key->has_intf_id = 1;
                  balIndCfg.terminal_disc->key->intf_id = front->intf_id;

                  BalSubscriberTerminalSubTermDiscData subTermCfgData;
                  memset(&subTermCfgData, 0, sizeof(BalSubscriberTerminalSubTermDiscData));
                  bal_subscriber_terminal_sub_term_disc_data__init(&subTermCfgData);
                  balIndCfg.terminal_disc->data = &subTermCfgData;
                  balIndCfg.terminal_disc->data->serial_number = &serial_number;
                  balIndCfg.terminal_disc->data->serial_number->vendor_id = vendor_id;
                  printf("\n***************************************************\n");
                  printf("Sending ONU discovery message\n");
                  printf("***************************************************\n");
                  status = bal_ind__bal_subs_term_discovery_ind(client, NULL, &balIndCfg, &output, NULL, 0);
               }
               else
               {
*/
                  balIndCfg.u_case = BAL_INDICATIONS__U_TERMINAL_DISC;
                  balIndCfg.has_sub_group = 1;
                  balIndCfg.sub_group = BAL_SUBSCRIBER_TERMINAL_AUTO_ID__BAL_SUBSCRIBER_TERMINAL_AUTO_ID_SUB_TERM_DISC;
                  BalSubscriberTerminalSubTermDisc terminal_disc;
                  memset(&terminal_disc, 0, sizeof(BalSubscriberTerminalSubTermDisc));
                  bal_subscriber_terminal_sub_term_disc__init(&terminal_disc);
                  balIndCfg.terminal_disc = &terminal_disc;

                  balIndCfg.terminal_disc->key = &subTermKey;
                  balIndCfg.terminal_disc->key->has_sub_term_id = 1;
                  balIndCfg.terminal_disc->key->sub_term_id = front->onu_id;
                  balIndCfg.terminal_disc->key->has_intf_id = 1;
                  balIndCfg.terminal_disc->key->intf_id = front->intf_id;

                  BalSubscriberTerminalSubTermDiscData subTermDiscCfgData;
                  memset(&subTermDiscCfgData, 0, sizeof(BalSubscriberTerminalSubTermDiscData));
                  bal_subscriber_terminal_sub_term_disc_data__init(&subTermDiscCfgData);
                  balIndCfg.terminal_disc->data = &subTermDiscCfgData;
                  balIndCfg.terminal_disc->data->serial_number = &serial_number;
                  balIndCfg.terminal_disc->data->serial_number->vendor_id = front->vendor_id;
                  balIndCfg.terminal_disc->data->serial_number->vendor_specific = front->vendor_specific;
                  printf("\n***************************************************\n");
                  printf("Sending ONU discovery message\n");
                  printf("***************************************************\n");
                  status = bal_ind__bal_subs_term_discovery_ind(client, NULL, &balIndCfg, &output, NULL, 0);


                  balIndCfg.u_case = BAL_INDICATIONS__U_TERMINAL_IND;
                  balIndCfg.has_sub_group = 1;
                  balIndCfg.sub_group = BAL_SUBSCRIBER_TERMINAL_AUTO_ID__BAL_SUBSCRIBER_TERMINAL_AUTO_ID_IND;
                  BalSubscriberTerminalInd terminal_ind;
                  memset(&terminal_ind, 0, sizeof(BalSubscriberTerminalInd));
                  bal_subscriber_terminal_ind__init(&terminal_ind);
                  balIndCfg.terminal_ind = &terminal_ind;

                  balIndCfg.terminal_ind->key = &subTermKey;
                  balIndCfg.terminal_ind->key->has_sub_term_id = 1;
                  balIndCfg.terminal_ind->key->sub_term_id = front->onu_id;
                  balIndCfg.terminal_ind->key->has_intf_id = 1;
                  balIndCfg.terminal_ind->key->intf_id = front->intf_id;

                  BalSubscriberTerminalIndData subTermCfgData;
                  memset(&subTermCfgData, 0, sizeof(BalSubscriberTerminalIndData));
                  bal_subscriber_terminal_ind_data__init(&subTermCfgData);
                  balIndCfg.terminal_ind->data = &subTermCfgData;
                  balIndCfg.terminal_ind->data->has_admin_state = 1;
                  balIndCfg.terminal_ind->data->admin_state = BAL_STATE__BAL_STATE_UP;
                  balIndCfg.terminal_ind->data->has_oper_status = 1;
                  balIndCfg.terminal_ind->data->oper_status = BAL_STATUS__BAL_STATUS_UP;
                  balIndCfg.terminal_ind->data->serial_number = &serial_number;
                  balIndCfg.terminal_ind->data->serial_number->vendor_id = front->vendor_id;
                  balIndCfg.terminal_ind->data->serial_number->vendor_specific = front->vendor_specific;
                  printf("***************************************************\n");
                  printf("ONU Activation Successful %s\n", balIndCfg.terminal_ind->data->serial_number->vendor_specific);
                  printf("***************************************************\n");
                  status = bal_ind__bal_subs_term_ind(client, NULL, &balIndCfg, &output, NULL, 0);
/*
               }
*/
               prevObjType = front->obj_type;
            }
            break;
         case BAL_OBJ_ID__BAL_OBJ_ID_PACKET:
            {
               balIndCfg.u_case = BAL_INDICATIONS__U_BAL_OMCI_RESP;
               BalPacketItuOmciChannelRx balomciresp;
               memset(&balomciresp, 0, sizeof(BalPacketItuOmciChannelRx));
               bal_packet_itu_omci_channel_rx__init(&balomciresp);

               BalPacketKey balomcirespkey;
               memset(&balomcirespkey, 0, sizeof(BalPacketKey));
               bal_packet_key__init(&balomcirespkey);
               balomciresp.key = &balomcirespkey;
               balIndCfg.balomciresp = &balomciresp;

               BalDest balomcirespkeydest;
               memset(&balomcirespkeydest, 0, sizeof(BalDest));
	       bal_dest__init(&balomcirespkeydest);
	       balomciresp.key->packet_send_dest = &balomcirespkeydest;
	       balomciresp.key->packet_send_dest->has_type = 1;
	       balomciresp.key->packet_send_dest->type = BAL_DEST_TYPE__BAL_DEST_TYPE_ITU_OMCI_CHANNEL;
	       balomciresp.key->packet_send_dest->u_case = BAL_DEST__U_ITU_OMCI_CHANNEL;

	       BalItuOmciChannel itu_omci_channel;
	       memset(&itu_omci_channel, 0, sizeof(BalItuOmciChannel));
	       bal_itu_omci_channel__init(&itu_omci_channel);
	       balomciresp.key->packet_send_dest->itu_omci_channel = &itu_omci_channel;
	       balomciresp.key->packet_send_dest->itu_omci_channel->has_sub_term_id = 1;
	       balomciresp.key->packet_send_dest->itu_omci_channel->sub_term_id = front->onu_id;
	       balomciresp.key->packet_send_dest->itu_omci_channel->has_intf_id = 1;
	       balomciresp.key->packet_send_dest->itu_omci_channel->intf_id = front->intf_id;
	       status = bal_ind__bal_pkt_omci_channel_rx_ind(client, NULL, &balIndCfg, &output, NULL, 0);
            }
            break;
         default:
            {
               balIndCfg.u_case = BAL_INDICATIONS__U__NOT_SET;
               prevObjType = front->obj_type;
            }
            break;
      }
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
struct QNode* newNode(int objKey, int status, char *device_id)
{
   struct QNode *temp = (struct QNode*)malloc(sizeof(struct QNode));
   temp->obj_type = objKey;
   temp->status = status;
   if(device_id != NULL)
   {
      memset(temp->device_id, 0, BAL_DEVICE_STR_LEN);
      memcpy(temp->device_id, device_id, strlen(device_id));
   }
   temp->next = NULL;
   return temp;
}

/* The function to add data to shared_queue - Add end of the queue */
void enQueue(int objKey, struct QNode *temp)
{
   /* Create a new LL node */

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

void stub_bal_init(BalInit *bal_init)
{
    char *ip_and_port = NULL;
    ip_and_port = bal_init->voltha_adapter_ip_port;
    client = grpc_c_client_init(ip_and_port, "bal_client", NULL);
}

void stub_bal_stats_get(BalInterfaceStatData *statData)
{
    printf("Bal Stub - Get Stats In BalStubs : Got all the statistics\n");
    statData->has_rx_bytes = 1;
    statData->has_rx_packets=1;
    statData->has_rx_ucast_packets=1;
    statData->has_rx_mcast_packets=1;
    statData->has_rx_bcast_packets=1;
    statData->has_rx_error_packets=1;
    statData->has_rx_unknown_protos=1;
    statData->has_tx_bytes = 1;
    statData->has_tx_packets=1;
    statData->has_tx_ucast_packets=1;
    statData->has_tx_mcast_packets=1;
    statData->has_tx_bcast_packets=1;
    statData->has_tx_error_packets=1;
    statData->has_rx_crc_errors=1;
    statData->has_bip_errors=1;

    statData->rx_bytes = 1000;          /**< RFC 2233 */
    statData->rx_packets = 100;        /**< RFC 1213 ucast + none-ucast */
    statData->rx_ucast_packets = 5;  /**< RFC 2233 */
    statData->rx_mcast_packets = 10;  /**< RFC 2233 */
    statData->rx_bcast_packets = 15;  /**< RFC 2233 */
    statData->rx_error_packets = 20;  /**< RFC 1213 */
    statData->rx_unknown_protos = 45; /**< RFC 1213 */
    statData->tx_bytes = 2000;          /**< RFC 2233 */
    statData->tx_packets = 190;        /**< RFC 1213 ucast + none-ucast */
    statData->tx_ucast_packets = 30;  /**< RFC 2233 */
    statData->tx_mcast_packets = 50;  /**< RFC 2233 */
    statData->tx_bcast_packets = 80;  /**< RFC 2233 */
    statData->tx_error_packets = 40;  /**< RFC 1213 */
    statData->rx_crc_errors = 5;     /**< Received packets with CRC error. */
    statData->bip_errors = 15;
}

#endif
