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

/* Global varibles */
balCoreIpInfo coreIpPortInfo;

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
   ret_val = context->gcc_stream->write(context, &bal_reboot, 0);
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
   ret_val = context->gcc_stream->write(context, &bal_err, 0);
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

   system("shutdown -r now");
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

#ifndef BAL_STUB
    /* Interface Type, Interface ID
       stat_data - Statistics Data */
    asfvolt16_bal_stats_get(read_stats->intf_type, read_stats->intf_id, &stat_data);
#else
    stub_bal_stats_get(&stat_data);
    ASFVOLT_LOG(ASFVOLT_DEBUG, "Bal Server - Get Stats In BalStubs : Got all the statistics\n");
#endif

    get_stats.data = &stat_data;

    ret_val = context->gcc_stream->write(context, &get_stats, 0);
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

   ret_val = context->gcc_stream->write(context, &bal_err, 0);
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
   ret_val = context->gcc_stream->write(context, &bal_err, 0);
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


/*
 * This functions gets invoked whenever bal finish RPC gets called
 */
void bal__bal_api_finish_cb(grpc_c_context_t *context)
{
#if 0
   void *finish_init;

   /*
    * Read incoming message into set_cfg
    */
   if (context->gcc_payload) {
      context->gcc_stream->read(context, (void **)&finish_init);
   }
#endif
}

#if 0
/*
 * This functions gets invoked whenever bal finish RPC gets called
 */
void bal_ind__bal_ind_info_cb(grpc_c_context_t *context)
{
#if 0
   void *finish_init;

   /*
    * Read incoming message into set_cfg
    */
   if (context->gcc_payload) {
      context->gcc_stream->read(context, (void **)&finish_init);
   }
#endif
}
#endif

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

}
