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
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include "bal_msg_type.grpc-c.h"
#include "bal_osmsg.grpc-c.h"
#include "bal_model_ids.grpc-c.h"
#include "bal_obj.grpc-c.h"
#include "bal_model_types.grpc-c.h"
#include "bal_errno.grpc-c.h"
#include "bal.grpc-c.h"

#ifdef BAL_STUB
#include "bal_stub.h"
#else
#include "asfvolt16_driver.h"
#endif

#include <unistd.h>
#include <sys/reboot.h>
#include "bal_indications_queue.h"

/* Global varibles */
balCoreIpInfo coreIpPortInfo;

/* extern variables*/
list_node *bal_ind_queue_tail = NULL;
list_node *bal_ind_queue_head = NULL;
pthread_mutex_t bal_ind_queue_lock;
unsigned int num_of_nodes = 0;

/* static variables*/
static grpc_c_server_t *test_server;
static void sigint_handler (int x) {
   grpc_c_server_destroy(test_server);
   exit(0);
}

void is_grpc_write_pending(int return_value)
{
   if (return_value != GRPC_C_WRITE_OK)
   {
      if(return_value == GRPC_C_WRITE_PENDING)
      {
         /* TODO: Register call back with grpc-c which will give an indication whenever write was succussful */
         ASFVOLT_LOG(ASFVOLT_INFO, "write(%d) is pending, sleep for 5 sec\n", return_value);
         sleep(5);
      }
      else
      {
         ASFVOLT_LOG(ASFVOLT_ERROR, "Failed to write %d \n", return_value);
      }
   }

}

/*
 * This functions gets invoked whenever bal Heartbeat RPC gets called
 */
void bal__bal_api_heartbeat_cb(grpc_c_context_t *context)
{
   BalHeartbeat *bal_hb;
   BalRebootState bal_reboot;
   int ret_val;
   /*
    * Read incoming message into set_cfg
    */
   if (context->gcc_payload) {
	context->gcc_stream->read(context, (void **)&bal_hb, 0);
   }

   bal_reboot_state__init(&bal_reboot);

   bal_reboot.has_is_reboot = 1;
#ifndef BAL_STUB
   bal_reboot.is_reboot = is_reboot;
#else
   bal_reboot.is_reboot = is_stub_reboot;
#endif

   /*
    * Write reply back to the client
    */
   ret_val = context->gcc_stream->write(context, &bal_reboot, -1);
   is_grpc_write_pending(ret_val);

   grpc_c_status_t status;
   status.gcs_code = 0;

    /*
     * Finish response for RPC
     */
    if (context->gcc_stream->finish(context, &status))
    {
        ASFVOLT_LOG(ASFVOLT_ERROR, "Failed to write status\n");
    }
}

/*
 * This functions gets invoked whenever Bal reboot gets called
 */
void bal__bal_api_reboot_cb(grpc_c_context_t *context)
{
   BalReboot *read_device;
   BalErr bal_err;
   int ret_val;

   /*
    * Read incoming message into get_cfg
    */
   if (context->gcc_payload)
   {
      context->gcc_stream->read(context, (void **)&read_device, 0);
   }

   ASFVOLT_LOG(ASFVOLT_INFO, "Bal Server - Reboot : ======Entering Function Reboot ==============================\n");
   ASFVOLT_LOG(ASFVOLT_INFO, "Bal Server - Reboot : Device ID is %s\n",read_device->device_id);

   /*
    * send it to BAL
    */

   bal_err__init(&bal_err);

   bal_err.err= 0;

   /*
    * Write reply back to the client
    */
   ret_val = context->gcc_stream->write(context, &bal_err, -1);
   is_grpc_write_pending(ret_val);

   grpc_c_status_t status;
   status.gcs_code = 0;

    /*
    * Finish response for RPC
    */
    if (context->gcc_stream->finish(context, &status))
    {
        ASFVOLT_LOG(ASFVOLT_ERROR, "Failed to write status\n");
    }

   ret_val = system("shutdown -r now");
   sleep(30);  /* allow system to shutdown gracefully */
   sync();  /* force shutdown if graceful did not work */
   reboot(RB_AUTOBOOT);
}

/*
 * This functions gets invoked whenever Bal Stats gets called
 */
void bal__bal_cfg_stat_get_cb(grpc_c_context_t *context)
{
    BalInterfaceKey *read_stats;
    int ret_val;

    /*
    * Read incoming message into get_cfg
    */
    if (context->gcc_payload) {
       context->gcc_stream->read(context, (void **)&read_stats, 0);
    }

    ASFVOLT_LOG(ASFVOLT_DEBUG, "Bal Server - Get Stats :======Entering Function Get Stats ============\n");
    ASFVOLT_LOG(ASFVOLT_DEBUG, "Bal Server - Get Stats :NNI port is %d\n",read_stats->intf_id);

    BalInterfaceStat get_stats;
    memset(&get_stats, 0, sizeof(BalInterfaceStat));
    bal_interface_stat__init(&get_stats);

    BalInterfaceStatData stat_data;
    memset(&stat_data, 0, sizeof(BalInterfaceStatData));
    bal_interface_stat_data__init(&stat_data);

    BalInterfaceKey stat_key;
    memset(&stat_key, 0, sizeof(BalInterfaceKey));
    bal_interface_key__init(&stat_key);

#ifndef BAL_STUB
    /* Interface Type, Interface ID
       stat_data - Statistics Data */
    asfvolt16_bal_stats_get(read_stats->intf_type, read_stats->intf_id, &stat_data, &stat_key);
#else
    stub_bal_stats_get(&stat_data);
    ASFVOLT_LOG(ASFVOLT_DEBUG, "Bal Server - Get Stats In BalStubs : Got all the statistics\n");
#endif

    get_stats.data = &stat_data;
    get_stats.key = &stat_key;

    ret_val = context->gcc_stream->write(context, &get_stats, -1);
    is_grpc_write_pending(ret_val);

    grpc_c_status_t status;
    status.gcs_code = 0;

    /*
    * Finish response for RPC
    */
    if (context->gcc_stream->finish(context, &status))
    {
        ASFVOLT_LOG(ASFVOLT_ERROR, "Failed to write status\n");
    }
}

/*
 * This functions gets invoked whenever bal RPC gets called
 */
void bal__bal_cfg_get_cb(grpc_c_context_t *context)
{
   BalCfg *get_cfg;

   /*
    * Read incoming message into get_cfg
    */
   if (context->gcc_payload) {
      context->gcc_stream->read(context, (void **)&get_cfg, 0);
   }

#ifndef BAL_STUB
   //asfvolt16_bal_cfg_get(key, get_cfg);
#endif
}

/*
 * This functions gets invoked whenever bal RPC gets called
 */
void bal__bal_cfg_set_cb(grpc_c_context_t *context)
{
   BalCfg *set_cfg;
   BalErr bal_err;
   int ret_val = 0;

   /*
    * Read incoming message into set_cfg
    */
   if (context->gcc_payload) {
      context->gcc_stream->read(context, (void **)&set_cfg, 0);
   }

   /*
    * send it to BAL
    */

   bal_err__init(&bal_err);

   bal_err.err= 0;

   /*
    * Write reply back to the client
    */

   ret_val = context->gcc_stream->write(context, &bal_err, -1);
   is_grpc_write_pending(ret_val);

   grpc_c_status_t status;
   status.gcs_code = 0;

   /*
    * Finish response for RPC
    */
   if (context->gcc_stream->finish(context, &status))
   {
      ASFVOLT_LOG(ASFVOLT_ERROR, "Failed to write status\n");
   }

#ifdef BAL_STUB
   pthread_mutex_lock(&lock);

   struct QNode *temp = newNode(set_cfg->hdr->obj_type,
         BAL_ERRNO__BAL_ERR_OK,
         set_cfg->device_id);

   switch(set_cfg->hdr->obj_type)
   {
      case BAL_OBJ_ID__BAL_OBJ_ID_ACCESS_TERMINAL:
         {
            ASFVOLT_LOG(ASFVOLT_INFO, "\n***************************************************\n");
            ASFVOLT_LOG(ASFVOLT_INFO, "Received Access Terminal Configuration msg\n");
            ASFVOLT_LOG(ASFVOLT_INFO, "***************************************************\n");
         }
         break;
      case BAL_OBJ_ID__BAL_OBJ_ID_INTERFACE:
         {
            ASFVOLT_LOG(ASFVOLT_INFO, "\n***************************************************\n");
            ASFVOLT_LOG(ASFVOLT_INFO, "Received PON Interface Configuration msg\n");
            ASFVOLT_LOG(ASFVOLT_INFO, "***************************************************\n");
            temp->intf_id = set_cfg->interface->key->intf_id;
            ASFVOLT_LOG(ASFVOLT_INFO, "Pon ID = %d\n", temp->intf_id);
         }
         break;
      case BAL_OBJ_ID__BAL_OBJ_ID_SUBSCRIBER_TERMINAL:
         {
            ASFVOLT_LOG(ASFVOLT_INFO, "\n*****************************************************\n");
            ASFVOLT_LOG(ASFVOLT_INFO, "Received ONU Activation msg\n");
            ASFVOLT_LOG(ASFVOLT_INFO, "*****************************************************\n");
            temp->intf_id = set_cfg->terminal->key->intf_id;
            temp->onu_id = set_cfg->terminal->key->sub_term_id;
            memset(temp->vendor_id, 0, BAL_DEVICE_STR_LEN);
            memcpy(temp->vendor_id,
                   set_cfg->terminal->data->serial_number->vendor_id,
                   strlen(set_cfg->terminal->data->serial_number->vendor_id));
            memset(temp->vendor_specific, 0, BAL_DEVICE_STR_LEN);
            memcpy(temp->vendor_specific,
                   set_cfg->terminal->data->serial_number->vendor_specific,
                   strlen(set_cfg->terminal->data->serial_number->vendor_specific));
         }
         break;
      case BAL_OBJ_ID__BAL_OBJ_ID_TM_SCHED:
         {
            ASFVOLT_LOG(ASFVOLT_INFO, "\n*****************************************************\n");
            ASFVOLT_LOG(ASFVOLT_INFO, "Received TM schedule msg\n");
            ASFVOLT_LOG(ASFVOLT_INFO, "*****************************************************\n");
         }
         break;
    case BAL_OBJ_ID__BAL_OBJ_ID_PACKET:
       {
          switch(set_cfg->packet->key->packet_send_dest->type)
          {
             case BAL_DEST_TYPE__BAL_DEST_TYPE_ITU_OMCI_CHANNEL:
                {
                   ASFVOLT_LOG(ASFVOLT_INFO, "\n*****************************************************\n");
                   ASFVOLT_LOG(ASFVOLT_INFO, "Received OMCI msg\n");
                   ASFVOLT_LOG(ASFVOLT_INFO, "*****************************************************\n");
                   temp->intf_id = set_cfg->terminal->key->intf_id;
                   temp->onu_id = set_cfg->terminal->key->sub_term_id;
                }
                break;
             default:
                {
                   ASFVOLT_LOG(ASFVOLT_INFO, "\n*****************************************************\n");
                   ASFVOLT_LOG(ASFVOLT_INFO, "Dest type invalid\n");
                   ASFVOLT_LOG(ASFVOLT_INFO, "*****************************************************\n");
                }
                break;
          }
       }
       break;
    default:
       {
          ASFVOLT_LOG(ASFVOLT_INFO, "\n*****************************************************\n");
          ASFVOLT_LOG(ASFVOLT_INFO, "Received Invalid msg type === %d \n", set_cfg->hdr->obj_type);
          ASFVOLT_LOG(ASFVOLT_INFO, "*****************************************************\n");
          pthread_mutex_unlock(&lock);
          return;
       }
       break;
   }
   enQueue(set_cfg->hdr->obj_type, temp);
   pthread_mutex_unlock(&lock);
   sleep(2);
   pthread_cond_signal(&cv);
#else
   if(BAL_OBJ_ID__BAL_OBJ_ID_ACCESS_TERMINAL == set_cfg->hdr->obj_type)
   {
      sleep(5); /* enable this if running with gdb */
   }
   asfvolt16_bal_cfg_set(set_cfg);
#endif
}


/*
 * This functions gets invoked whenever bal clear RPC gets called
 */
void bal__bal_cfg_clear_cb(grpc_c_context_t *context)
{
   BalKey *clear_key;

   /*
    * Read incoming message into clear_key
    */
   if (context->gcc_payload) {
      context->gcc_stream->read(context, (void **)&clear_key, 0);
   }

#ifndef BAL_STUB
   asfvolt16_bal_cfg_clear(clear_key);
#endif
}


/*
 * This functions gets invoked whenever bal Init RPC gets called
 */
void bal__bal_api_init_cb(grpc_c_context_t *context)
{
   BalInit *bal_init;
   BalErr bal_err;
   int ret_val;

   /*
    * Read incoming message into set_cfg
    */
   if (context->gcc_payload)
   {
      context->gcc_stream->read(context, (void **)&bal_init, 0);
   }

   /*
    * send it to BAL
    */

   ASFVOLT_LOG(ASFVOLT_INFO, "\n*****************************************************\n");
   ASFVOLT_LOG(ASFVOLT_INFO, "Received API Init msg\n");
   ASFVOLT_LOG(ASFVOLT_INFO, "*****************************************************\n");

   bal_err__init(&bal_err);

   bal_err.err= 0;

   /*
    * Write reply back to the client
    */
   ret_val = context->gcc_stream->write(context, &bal_err, -1);
   is_grpc_write_pending(ret_val);

   grpc_c_status_t status;
   status.gcs_code = 0;

   /*
    * Finish response for RPC
    */
   if (context->gcc_stream->finish(context, &status))
   {
      ASFVOLT_LOG(ASFVOLT_ERROR, "Failed to write status\n");
   }

#ifndef BAL_STUB
   asfvolt16_bal_init(bal_init, &coreIpPortInfo);
#else
   ASFVOLT_LOG(ASFVOLT_INFO, "\nRecevied IP Address == %s \n", bal_init->voltha_adapter_ip_port);
   stub_bal_init(bal_init);
#endif

}

void bal_get_ind__free_mem_access_term_ind(BalIndications *balIndCfg)
{
   free(balIndCfg->access_term_ind->data->sw_version);
   free(balIndCfg->access_term_ind->data->topology);
   free(balIndCfg->access_term_ind->data);
   free(balIndCfg->access_term_ind->key);
   free(balIndCfg->access_term_ind->hdr);
   free(balIndCfg->access_term_ind);
   free(balIndCfg);
}

void bal_get_ind__free_mem_access_term_ind_op_state(BalIndications *balIndCfg)
{
   free(balIndCfg->access_term_ind_op_state->data);
   free(balIndCfg->access_term_ind_op_state->key);
   free(balIndCfg->access_term_ind_op_state->hdr);
   free(balIndCfg->access_term_ind_op_state);
   free(balIndCfg);
}

void bal_get_ind__free_mem_flow_op_state(BalIndications *balIndCfg)
{
   free(balIndCfg->flow_op_state->data);
   free(balIndCfg->flow_op_state->key);
   free(balIndCfg->flow_op_state->hdr);
   free(balIndCfg->flow_op_state);
   free(balIndCfg);
}

void bal_get_ind__free_mem_flow_ind(BalIndications *balIndCfg)
{
   free(balIndCfg->flow_ind->data->action);
   free(balIndCfg->flow_ind->data->classifier->src_mac.data);
   free(balIndCfg->flow_ind->data->classifier->dst_mac.data);
   free(balIndCfg->flow_ind->data->classifier);
   free(balIndCfg->flow_ind->data);
   free(balIndCfg->flow_ind->key);
   free(balIndCfg->flow_ind->hdr);
   free(balIndCfg->flow_ind);
   free(balIndCfg);
}

void bal_get_ind__free_mem_group_ind(BalIndications *balIndCfg)
{
   unsigned int i = 0;
   free(balIndCfg->group_ind->data->flows->val);
   free(balIndCfg->group_ind->data->flows);
   for (i = 0; i < balIndCfg->group_ind->data->members->n_val; i++)
   {
      free(balIndCfg->group_ind->data->members->val[i]->queue);
      free(balIndCfg->group_ind->data->members->val[i]->action);
      free(balIndCfg->group_ind->data->members->val[i]);
   }
   free(balIndCfg->group_ind->data->members);
   free(balIndCfg->group_ind->data);
   free(balIndCfg->group_ind->key);
   free(balIndCfg->group_ind->hdr);
   free(balIndCfg->group_ind);
   free(balIndCfg);
}

void bal_get_ind__free_mem_interface_op_state(BalIndications *balIndCfg)
{
   free(balIndCfg->interface_op_state->data);
   free(balIndCfg->interface_op_state->key);
   free(balIndCfg->interface_op_state->hdr);
   free(balIndCfg->interface_op_state);
   free(balIndCfg);
}

void bal_get_ind__free_mem_interface_los(BalIndications *balIndCfg)
{
   free(balIndCfg->interface_los->data);
   free(balIndCfg->interface_los->hdr);
   free(balIndCfg->interface_los);
   free(balIndCfg);
}

void bal_get_ind__free_mem_interface_ind(BalIndications *balIndCfg)
{
   free(balIndCfg->interface_ind->data->sub_term_id_list->val);
   free(balIndCfg->interface_ind->data->sub_term_id_list);
   free(balIndCfg->interface_ind->data);
   free(balIndCfg->interface_ind->key);
   free(balIndCfg->interface_ind->hdr);
   free(balIndCfg->interface_ind);
   free(balIndCfg);
}

void bal_get_ind__free_mem_terminal_op_state(BalIndications *balIndCfg)
{
   free(balIndCfg->terminal_op_state->data);
   free(balIndCfg->terminal_op_state->key);
   free(balIndCfg->terminal_op_state->hdr);
   free(balIndCfg->terminal_op_state);
   free(balIndCfg);
}

void bal_get_ind__free_mem_terminal_disc(BalIndications *balIndCfg)
{
   free(balIndCfg->terminal_disc->data->serial_number->vendor_specific);
   free(balIndCfg->terminal_disc->data->serial_number->vendor_id);
   free(balIndCfg->terminal_disc->data->serial_number);
   free(balIndCfg->terminal_disc->data);
   free(balIndCfg->terminal_disc->key);
   free(balIndCfg->terminal_disc->hdr);
   free(balIndCfg->terminal_disc);
   free(balIndCfg);
}

void bal_get_ind__free_mem_terminal_alarm(BalIndications *balIndCfg)
{
   free(balIndCfg->terminal_alarm->data->alarm);
   free(balIndCfg->terminal_alarm->data);
   free(balIndCfg->terminal_alarm->key);
   free(balIndCfg->terminal_alarm->hdr);
   free(balIndCfg->terminal_alarm);
   free(balIndCfg);
}

void bal_get_ind__free_mem_terminal_dgi(BalIndications *balIndCfg)
{
   free(balIndCfg->terminal_dgi->data);
   free(balIndCfg->terminal_dgi->key);
   free(balIndCfg->terminal_dgi->hdr);
   free(balIndCfg->terminal_dgi);
   free(balIndCfg);
}

void bal_get_ind__free_mem_terminal_ind(BalIndications *balIndCfg)
{
   free(balIndCfg->terminal_ind->data->agg_port_id_list);
   free(balIndCfg->terminal_ind->data->serial_number->vendor_specific);
   free(balIndCfg->terminal_ind->data->serial_number->vendor_id);
   free(balIndCfg->terminal_ind->data->serial_number);
   free(balIndCfg->terminal_ind->data->registration_id);
   free(balIndCfg->terminal_ind->data->password);
   free(balIndCfg->terminal_ind->data);
   free(balIndCfg->terminal_ind->key);
   free(balIndCfg->terminal_ind->hdr);
   free(balIndCfg->terminal_ind);
   free(balIndCfg);
}

void bal_get_ind__free_mem_tm_queue_ind(BalIndications *balIndCfg)
{
   switch (balIndCfg->tm_queue_ind->data->bac->type)
   {
      case BAL_TM_BAC_TYPE__BAL_TM_BAC_TYPE_TAILDROP:
        free(balIndCfg->tm_queue_ind->data->bac->taildrop);
	break;
      case BAL_TM_BAC_TYPE__BAL_TM_BAC_TYPE_WTAILDROP:
        /*Nothing to do*/
        break;
      case BAL_TM_BAC_TYPE__BAL_TM_BAC_TYPE_RED:
        free(balIndCfg->tm_queue_ind->data->bac->red->red);
        free(balIndCfg->tm_queue_ind->data->bac->red);
	break;
      case BAL_TM_BAC_TYPE__BAL_TM_BAC_TYPE_WRED:
        free(balIndCfg->tm_queue_ind->data->bac->wred->red);
	free(balIndCfg->tm_queue_ind->data->bac->wred->yellow);
	free(balIndCfg->tm_queue_ind->data->bac->wred->green);
	free(balIndCfg->tm_queue_ind->data->bac->wred);
      default:
        /*Nothing to do*/
         break;
   }
   free(balIndCfg->tm_queue_ind->data->bac);
   free(balIndCfg->tm_queue_ind->data->rate);
   free(balIndCfg->tm_queue_ind->data);
   free(balIndCfg->tm_queue_ind->key);
   free(balIndCfg->tm_queue_ind);
   free(balIndCfg);
}

void bal_get_ind__free_mem_u_tm_sched_ind(BalIndications *balIndCfg)
{
   free(balIndCfg->tm_sched_ind->data);
   free(balIndCfg->tm_sched_ind->key);
   free(balIndCfg->tm_sched_ind);
   free(balIndCfg);
}

void bal_get_ind__free_mem_u_pkt_data(BalIndications *balIndCfg)
{
   free(balIndCfg->pktdata->data->pkt.data);
   free(balIndCfg->pktdata->data);
   switch(balIndCfg->pktdata->key->packet_send_dest->type)
   {
      case BAL_DEST_TYPE__BAL_DEST_TYPE_NNI:
         free(balIndCfg->pktdata->key->packet_send_dest->nni);
         break;
      case BAL_DEST_TYPE__BAL_DEST_TYPE_SUB_TERM:
         free(balIndCfg->pktdata->key->packet_send_dest->sub_term);
	 break;
      case BAL_DEST_TYPE__BAL_DEST_TYPE_SVC_PORT:
         free(balIndCfg->pktdata->key->packet_send_dest->svc_port);
	 break;
      default:
         /*Nothing to do*/
         break;
   }
   free(balIndCfg->pktdata->key->packet_send_dest);
   free(balIndCfg->pktdata->key);
   free(balIndCfg->pktdata->hdr);
   free(balIndCfg->pktdata);
   free(balIndCfg);
}

void bal_get_ind__free_mem_u_bal_omci_resp(BalIndications *balIndCfg)
{
   switch(balIndCfg->balomciresp->key->packet_send_dest->type)
   {
      case BAL_DEST_TYPE__BAL_DEST_TYPE_ITU_OMCI_CHANNEL:
         free(balIndCfg->balomciresp->key->packet_send_dest->itu_omci_channel);
         break;
      default:
         /*Nothing to do*/
         break;
   }
   free(balIndCfg->balomciresp->key->packet_send_dest);
   free(balIndCfg->balomciresp->data->pkt.data);
   free(balIndCfg->balomciresp->data);
   free(balIndCfg->balomciresp->key);
   free(balIndCfg->balomciresp->hdr);
   free(balIndCfg->balomciresp);
   free(balIndCfg);
}

void bal_get_ind__free_mem_u_bal_oam_resp(BalIndications *balIndCfg)
{
   free(balIndCfg->baloamresp->data->pkt.data);
   free(balIndCfg->baloamresp->data);
   switch(balIndCfg->baloamresp->key->packet_send_dest->type)
   {
      case BAL_DEST_TYPE__BAL_DEST_TYPE_IEEE_OAM_CHANNEL:
         free(balIndCfg->baloamresp->key->packet_send_dest->ieee_oam_channel);
         break;
      default:
         /*Nothing to do*/
         break;
   }
   free(balIndCfg->baloamresp->key->packet_send_dest);
   free(balIndCfg->baloamresp->key);
   free(balIndCfg->baloamresp->hdr);
   free(balIndCfg->baloamresp);
   free(balIndCfg);
}

void bal_get_ind__free_mem(BalIndications *balIndCfg)
{
    switch (balIndCfg->u_case)
    {
       case BAL_INDICATIONS__U_ACCESS_TERM_IND:
          bal_get_ind__free_mem_access_term_ind(balIndCfg);
          break;

       case BAL_INDICATIONS__U_ACCESS_TERM_IND_OP_STATE:
          bal_get_ind__free_mem_access_term_ind_op_state(balIndCfg);
          break;

       case BAL_INDICATIONS__U_FLOW_OP_STATE:
          bal_get_ind__free_mem_flow_op_state(balIndCfg);
          break;

       case BAL_INDICATIONS__U_FLOW_IND:
          bal_get_ind__free_mem_flow_ind(balIndCfg);
          break;

       case BAL_INDICATIONS__U_GROUP_IND:
          bal_get_ind__free_mem_group_ind(balIndCfg);
          break;

       case BAL_INDICATIONS__U_INTERFACE_OP_STATE:
          bal_get_ind__free_mem_interface_op_state(balIndCfg);
          break;

       case BAL_INDICATIONS__U_INTERFACE_LOS:
          bal_get_ind__free_mem_interface_los(balIndCfg);
          break;

       case BAL_INDICATIONS__U_INTERFACE_IND:
          bal_get_ind__free_mem_interface_ind(balIndCfg);
          break;

       case BAL_INDICATIONS__U_TERMINAL_OP_STATE:
          bal_get_ind__free_mem_terminal_op_state(balIndCfg);
          break;

       case BAL_INDICATIONS__U_TERMINAL_DISC:
          bal_get_ind__free_mem_terminal_disc(balIndCfg);
          break;

       case BAL_INDICATIONS__U_TERMINAL_ALARM:
          bal_get_ind__free_mem_terminal_alarm(balIndCfg);
          break;

       case BAL_INDICATIONS__U_TERMINAL_DGI:
          bal_get_ind__free_mem_terminal_dgi(balIndCfg);
          break;

       case BAL_INDICATIONS__U_TERMINAL_IND:
          bal_get_ind__free_mem_terminal_ind(balIndCfg);
          break;

       case BAL_INDICATIONS__U_TM_QUEUE__IND:
          bal_get_ind__free_mem_tm_queue_ind(balIndCfg);
          break;

       case BAL_INDICATIONS__U_TM_SCHED__IND:
          bal_get_ind__free_mem_u_tm_sched_ind(balIndCfg);
          break;

       case BAL_INDICATIONS__U_PKT_DATA:
          bal_get_ind__free_mem_u_pkt_data(balIndCfg);
          break;

       case BAL_INDICATIONS__U_BAL_OMCI_RESP:
          bal_get_ind__free_mem_u_bal_omci_resp(balIndCfg);
          break;

       case BAL_INDICATIONS__U_BAL_OAM_RESP:
          bal_get_ind__free_mem_u_bal_oam_resp(balIndCfg);
          break;

       default:
          ASFVOLT_LOG(ASFVOLT_ERROR, "Unknown case %u\n", balIndCfg->u_case);
          break;
    }
}

void bal_get_ind__bal_get_ind_from_device_cb(grpc_c_context_t *context)
{
   BalDefault *dummy;
   BalIndications *bal_indication;
   list_node *node = NULL;
   int ret_val = 0;

   /*
    * Read incoming message into set_cfg
    */
   if (context->gcc_payload) {
      context->gcc_stream->read(context, (void **)&dummy, 0);
   }

   /*
    * send it to BAL
    */

   pthread_mutex_lock(&bal_ind_queue_lock);
   node = get_bal_indication_node();
   pthread_mutex_unlock(&bal_ind_queue_lock);

   if(node != NULL)
   {
      bal_indication = node->bal_indication;
      bal_indication->ind_present = true;
      bal_indication->has_ind_present = true;
   }
   else
   {
      bal_indication = malloc(sizeof(BalIndications));
      memset(bal_indication, 0, sizeof(BalIndications));
      bal_indications__init(bal_indication);
      bal_indication->ind_present = false;
      bal_indication->has_ind_present = true;
   }

   /*
    * Write reply back to the client
    */

   ret_val = context->gcc_stream->write(context, bal_indication, -1);
   is_grpc_write_pending(ret_val);

   grpc_c_status_t status;
   status.gcs_code = 0;

   /*
    * Finish response for RPC
    */
   if (context->gcc_stream->finish(context, &status))
   {
      ASFVOLT_LOG(ASFVOLT_ERROR, "Failed to write status\n");
   }

   /*Free memory for 'bal_indication' and 'node'*/
   if (bal_indication->ind_present)
   {
      bal_get_ind__free_mem(bal_indication);
      free(node);
   }
   else
   {
      free(bal_indication);
   }
}

/*
 * This functions gets invoked whenever bal finish RPC gets called
 */
void bal__bal_api_finish_cb(grpc_c_context_t *context)
{

}

void bal_ind__bal_acc_term_ind_cb(grpc_c_context_t *context)
{
}
void bal_ind__bal_acc_term_oper_sts_cng_ind_cb(grpc_c_context_t *context)
{
}
void bal_ind__bal_flow_oper_sts_cng_cb(grpc_c_context_t *context)
{
}
void bal_ind__bal_flow_ind_cb(grpc_c_context_t *context)
{
}
void bal_ind__bal_group_ind_cb(grpc_c_context_t *context)
{
}
void bal_ind__bal_iface_oper_sts_cng_cb(grpc_c_context_t *context)
{
}
void bal_ind__bal_iface_los_cb(grpc_c_context_t *context)
{
}
void bal_ind__bal_iface_ind_cb(grpc_c_context_t *context)
{
}
void bal_ind__bal_iface_stat_cb(grpc_c_context_t *context)
{
}
void bal_ind__bal_subs_term_oper_sts_cng_cb(grpc_c_context_t *context)
{
}
void bal_ind__bal_subs_term_discovery_ind_cb(grpc_c_context_t *context)
{
}
void bal_ind__bal_subs_term_alarm_ind_cb(grpc_c_context_t *context)
{
}
void bal_ind__bal_subs_term_dgi_ind_cb(grpc_c_context_t *context)
{
}
void bal_ind__bal_subs_term_ind_cb(grpc_c_context_t *context)
{
}
void bal_ind__bal_tm_queue_ind_info_cb(grpc_c_context_t *context)
{
}
void bal_ind__bal_tm_sched_ind_info_cb(grpc_c_context_t *context)
{
}
void bal_ind__bal_pkt_bearer_channel_rx_ind_cb(grpc_c_context_t *context)
{
}
void bal_ind__bal_pkt_omci_channel_rx_ind_cb(grpc_c_context_t *context)
{
}
void bal_ind__bal_pkt_ieee_oam_channel_rx_ind_cb(grpc_c_context_t *context)
{
}


/*
 * Takes socket path as argument
 */
int main (int argc, char **argv)
{
   int i = 0;
   grpc_c_server_t *server = NULL;

   if (argc < 6)
   {
      fprintf(stderr, "Missing socket path argument\n");
      exit(1);
   }

   strcpy(coreIpPortInfo.bal_core_arg1, argv[2] /*, strlen(argv[2])*/);
   strcpy(coreIpPortInfo.bal_core_ip_port, argv[3]/*, strlen(argv[3])*/);
   strcpy(coreIpPortInfo.bal_core_arg2, argv[4]/*, strlen(argv[4])*/);
   strcpy(coreIpPortInfo.bal_shared_lib_ip_port, argv[5]/*, strlen(argv[5])*/);

   signal(SIGINT, sigint_handler);
   /*
    * Initialize grpc-c library to be used with vanilla gRPC
    */
   grpc_c_init(GRPC_THREADS, NULL);

   /*
    * Create server object
    */
   test_server = grpc_c_server_create(argv[1]);

   if (test_server == NULL)
   {
      ASFVOLT_LOG(ASFVOLT_ERROR, "Failed to create server\n");
      exit(1);
   }

   /*
    * Initialize greeter service
    */
   ASFVOLT_LOG(ASFVOLT_INFO, "\nvoltha_bal_driver running.....\n");
   bal__service_init(test_server);
   bal_get_ind__service_init(test_server);

   /*
    * Start server
    */
   grpc_c_server_start(test_server);

#ifdef BAL_STUB
   create_stub_thread();
#endif

   /*
    * Blocks server to wait to completion
    */
   grpc_c_server_wait(test_server);

   /* code added for example Makefile to compile grpc-c along with edgecore driver */
   bal__service_init(server);
   bal_get_ind__service_init(server);
}
