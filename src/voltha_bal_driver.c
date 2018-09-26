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
#include <string.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/time.h>
#include "bal_msg_type.grpc-c.h"
#include "bal_osmsg.grpc-c.h"
#include "bal_model_ids.grpc-c.h"
#include "bal_obj.grpc-c.h"
#include "bal_model_types.grpc-c.h"
#include "bal_errno.grpc-c.h"
#include "bal.grpc-c.h"
#include "asfvolt.grpc-c.h"

#include "asfvolt16_driver.h"

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
static bool is_init_done = false;

/*MACRO Definitions*/
#define ASFVOLT_FIELD_LEN 200

void is_grpc_write_pending(int return_value)
{
   if (return_value != GRPC_C_WRITE_OK)
   {
      if(return_value == GRPC_C_WRITE_PENDING)
      {
         /* TODO: Register call back with grpc-c which will give an indication whenever write was succussful */
         ASFVOLT_LOG(ASFVOLT_INFO, "write(%d) is pending, sleep for 5 sec", return_value);
         sleep(5);
      }
      else
      {
         ASFVOLT_LOG(ASFVOLT_ERROR, "Failed to write %d", return_value);
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
   bal_reboot.is_reboot = is_reboot;

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
        ASFVOLT_LOG(ASFVOLT_ERROR, "Failed to write status");
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
      ASFVOLT_LOG(ASFVOLT_INFO, "Bal Server - Reboot : Device ID is %s",read_device->device_id);
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
        ASFVOLT_LOG(ASFVOLT_ERROR, "Failed to write status");
    }

   ret_val = system("shutdown -r now");
   sleep(30);  /* allow system to shutdown gracefully */
   sync();  /* force shutdown if graceful did not work */
   reboot(RB_AUTOBOOT);
}

/*
This function reads the specified field from the 'onldump -o' command.
If the field was successfully read, it returns the field value.
If it failed to read the field, then null charecter is returned
*/
char* asfvolt_read_sysinfo(char* field_name, char* field_val)
{
   FILE *fp;
   /* Prepare the command*/
   char command[150];

   snprintf(command, sizeof command, "bash -l -c \"onlpdump -s\" | perl -ne 'print $1 if /%s: (\\S+)/'", field_name);
   /* Open the command for reading. */
   fp = popen(command, "r");
   if (fp == NULL) {
       /*The client has to check for a Null mac address in this case*/
       ASFVOLT_LOG(ASFVOLT_ERROR, "Failed to query the mac address");
       return field_val;
   }

   /*Read the field value*/
   if (fp) {
       fread(field_val, ASFVOLT_FIELD_LEN, 1, fp);
       pclose(fp);
   }
   return field_val;
}

/*
 * This functions gets invoked whenever AsfvoltGetSystemInfo RPC gets called
 */
void asfvolt__asfvolt_get_system_info_cb(grpc_c_context_t *context)
{
   BalDefault *dummy;
   AsfSystemInfo asf_system_info;
   char product_name[ASFVOLT_FIELD_LEN] = "\0";
   char part_num[ASFVOLT_FIELD_LEN] = "\0";
   char serial_num[ASFVOLT_FIELD_LEN] = "\0";
   char mac_address[ASFVOLT_FIELD_LEN] = "\0";
   char mac_range[ASFVOLT_FIELD_LEN] = "\0";
   char manufacturer[ASFVOLT_FIELD_LEN] = "\0";
   char manufacturer_date[ASFVOLT_FIELD_LEN] = "\0";
   char vendor[ASFVOLT_FIELD_LEN] = "\0";
   char platform_name[ASFVOLT_FIELD_LEN] = "\0";
   char label_revision[ASFVOLT_FIELD_LEN] = "\0";
   char coutry_code[ASFVOLT_FIELD_LEN] = "\0";
   char diag_version[ASFVOLT_FIELD_LEN] = "\0";
   char onie_version[ASFVOLT_FIELD_LEN] = "\0";
   int ret_val;

   if (context->gcc_payload) {
	context->gcc_stream->read(context, (void **)&dummy, 0);
   }

   asf_system_info__init(&asf_system_info);

   asf_system_info.product_name = asfvolt_read_sysinfo("Product Name", product_name);
   asf_system_info.part_num = asfvolt_read_sysinfo("Part Number", part_num);
   asf_system_info.serial_num = asfvolt_read_sysinfo("Serial Number", serial_num);
   asf_system_info.mac_address = asfvolt_read_sysinfo("MAC", mac_address);
   asf_system_info.mac_range = asfvolt_read_sysinfo("MAC Range", mac_range);
   asf_system_info.manufacturer = asfvolt_read_sysinfo("Manufacturer", manufacturer);
   asf_system_info.manufacture_date = asfvolt_read_sysinfo("Manufacture Date", manufacturer_date);
   asf_system_info.vendor = asfvolt_read_sysinfo("Vendor", vendor);
   asf_system_info.platform_name = asfvolt_read_sysinfo("Platform Name", platform_name);
   asf_system_info.label_revision = asfvolt_read_sysinfo("Label Revision", label_revision);
   asf_system_info.country_code = asfvolt_read_sysinfo("Country Code", coutry_code);
   asf_system_info.diag_version = asfvolt_read_sysinfo("Diag Version", diag_version);
   asf_system_info.onie_version = asfvolt_read_sysinfo("ONIE Version", onie_version);

   /*
    * Write reply back to the client
    */
   ret_val = context->gcc_stream->write(context, &asf_system_info, -1);
   is_grpc_write_pending(ret_val);

   grpc_c_status_t status;
   status.gcs_code = 0;

   /*
     * Finish response for RPC
   */
   if (context->gcc_stream->finish(context, &status))
   {
       ASFVOLT_LOG(ASFVOLT_ERROR, "Failed to write status");
   }
}

/*
This function reads the SFP presence map information from 'onlpdump -p' system command
and returns a bitmap with the presence information.
*/
unsigned int asfvolt_read_sfp_presence_bitmap(void)
{
   char command[150];
   char sfp_presence_output[ASFVOLT_FIELD_LEN];
   unsigned int sfp_presence_bitmap = 0;
   char delim[2] = " ";
   char  *field = NULL;
   FILE *fp;
   snprintf(command, sizeof command, "bash -l -c \"onlpdump -p\" | perl -ne 'print $1 if /Presence: ([0-9 ]+)/'");
   /* Open the command for reading. */
   fp = popen(command, "r");
   if (fp == NULL) {
       /*The client has to check for a Null mac address in this case*/
       ASFVOLT_LOG(ASFVOLT_ERROR, "Failed to query the sfp presence map");
       return sfp_presence_bitmap;
   }

   /*Read the field value*/
   fread(sfp_presence_output, ASFVOLT_FIELD_LEN, 1, fp);
   pclose(fp);

   field = strtok(sfp_presence_output, delim);
   while (field != NULL) {
      sfp_presence_bitmap |= (1 << atoi(field));
      field = strtok(NULL, delim);
   }

   return sfp_presence_bitmap;
}

/*
 * This functions gets invoked whenever AsfvoltGetSfpPresenceBitmap RPC gets called
 */
void asfvolt__asfvolt_get_sfp_presence_bitmap_cb(grpc_c_context_t *context)
{
   BalDefault *dummy;
   AsfSfpPresenceBitmap sfp_presence_bitmap;
   int ret_val;

   if (context->gcc_payload) {
	context->gcc_stream->read(context, (void **)&dummy, 0);
   }

   asf_sfp_presence_bitmap__init(&sfp_presence_bitmap);

   sfp_presence_bitmap.bitmap = asfvolt_read_sfp_presence_bitmap();
   sfp_presence_bitmap.has_bitmap = true;

   /*
    * Write reply back to the client
    */
   ret_val = context->gcc_stream->write(context, &sfp_presence_bitmap, -1);
   is_grpc_write_pending(ret_val);

   grpc_c_status_t status;
   status.gcs_code = 0;

   /*
     * Finish response for RPC
   */
   if (context->gcc_stream->finish(context, &status))
   {
       ASFVOLT_LOG(ASFVOLT_ERROR, "Failed to write status");
   }
}

/*
 * This functions gets invoked whenever Bal Stats gets called
 */
void bal__bal_cfg_stat_get_cb(grpc_c_context_t *context)
{
    BalInterfaceKey *read_stats = 0;
    int ret_val;

    /*
    * Read incoming message into get_cfg
    */
    if (context->gcc_payload) {
       context->gcc_stream->read(context, (void **)&read_stats, 0);
       ASFVOLT_LOG(ASFVOLT_DEBUG, "Bal Server - Get Stats :NNI port is %d",read_stats->intf_id);
    }


    BalInterfaceStat get_stats;
    memset(&get_stats, 0, sizeof(BalInterfaceStat));
    bal_interface_stat__init(&get_stats);

    BalInterfaceStatData stat_data;
    memset(&stat_data, 0, sizeof(BalInterfaceStatData));
    bal_interface_stat_data__init(&stat_data);

    BalInterfaceKey stat_key;
    memset(&stat_key, 0, sizeof(BalInterfaceKey));
    bal_interface_key__init(&stat_key);

    /* Interface Type, Interface ID
       stat_data - Statistics Data */
    if ((read_stats) && (is_init_done == true)) {
       asfvolt16_bal_stats_get(read_stats->intf_type, read_stats->intf_id, &stat_data, &stat_key);
    }

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
        ASFVOLT_LOG(ASFVOLT_ERROR, "Failed to write status");
    }
}

void bal_cfg_get__free_mem_sub_term_cfg_get(BalCfg *balCfgRsp)
{
   free(balCfgRsp->terminal->data->password);
   free(balCfgRsp->terminal->data->registration_id);
   free(balCfgRsp->terminal->data->serial_number->vendor_id);
   free(balCfgRsp->terminal->data->serial_number->vendor_specific);
   free(balCfgRsp->terminal->data->serial_number);
}

void bal_cfg_get__free_mem(BalCfg *balCfgRsp)
{
    switch (balCfgRsp->hdr->obj_type)
	{
	    case BAL_OBJ_ID__BAL_OBJ_ID_ACCESS_TERMINAL:
               break;

	    case BAL_OBJ_ID__BAL_OBJ_ID_INTERFACE:
	       break;

	    case BAL_OBJ_ID__BAL_OBJ_ID_SUBSCRIBER_TERMINAL:
               bal_cfg_get__free_mem_sub_term_cfg_get(balCfgRsp);
	       break;

	    default:
	       break;
	}
}

/*
 * This functions gets invoked whenever bal RPC gets called
 */
void bal__bal_cfg_get_cb(grpc_c_context_t *context)
{
   BalKey *bal_key = 0;
   int ret_val;

   /*
    * Read incoming message into bal_key
    */
   if (context->gcc_payload) {
      context->gcc_stream->read(context, (void **)&bal_key, 0);
   }

   /*
    * send it to BAL
    */
   BalCfg get_cfg_resp;
   memset(&get_cfg_resp, 0, sizeof(BalCfg));
   bal_cfg__init(&get_cfg_resp);

   BalObj hdr_cfg;
   memset(&hdr_cfg, 0, sizeof(BalObj));
   bal_obj__init(&hdr_cfg);
   get_cfg_resp.hdr = &hdr_cfg;

   BalCommMsgHdr comm_hdr;
   memset(&comm_hdr, 0, sizeof(BalCommMsgHdr));
   bal_comm_msg_hdr__init(&comm_hdr);
   get_cfg_resp.hdr->comm_hdr = &comm_hdr;

   if (bal_key) {
      switch(bal_key->hdr->obj_type)
      {
	 case BAL_OBJ_ID__BAL_OBJ_ID_ACCESS_TERMINAL:
	    {
	       ASFVOLT_LOG(ASFVOLT_ERROR, "Bal Get For Access Terminal Received\n");

	       get_cfg_resp.obj_case = BAL_CFG__OBJ_CFG;

	       get_cfg_resp.hdr->obj_type = BAL_OBJ_ID__BAL_OBJ_ID_ACCESS_TERMINAL;

	       BalAccessTerminalCfg access_term_cfg;
	       memset(&access_term_cfg, 0, sizeof(BalAccessTerminalCfg));
	       bal_access_terminal_cfg__init(&access_term_cfg);
	       get_cfg_resp.cfg = &access_term_cfg;

	       BalAccessTerminalCfgData access_term_cfg_data;
	       memset(&access_term_cfg_data, 0, sizeof(BalAccessTerminalCfgData));
	       bal_access_terminal_cfg_data__init(&access_term_cfg_data);
	       get_cfg_resp.cfg->data = &access_term_cfg_data;

	       BalTopology access_term_cfg_data_topology;
	       memset(&access_term_cfg_data_topology, 0, sizeof(BalTopology));
	       bal_topology__init(&access_term_cfg_data_topology);
	       get_cfg_resp.cfg->data->topology = &access_term_cfg_data_topology;

	       BalSwVersion access_term_cfg_data_sw_version;
	       memset(&access_term_cfg_data_sw_version, 0, sizeof(BalSwVersion));
	       bal_sw_version__init(&access_term_cfg_data_sw_version);
	       get_cfg_resp.cfg->data->sw_version = &access_term_cfg_data_sw_version;

	       BalAccessTerminalKey access_term_key;
	       memset(&access_term_key, 0, sizeof(BalAccessTerminalKey));
	       bal_access_terminal_key__init(&access_term_key);
	       get_cfg_resp.cfg->key = &access_term_key;

	       if (is_init_done == true) {
		  asfvolt16_bal_cfg_get(&get_cfg_resp);
	       }
	    }
	    break;

	 case BAL_OBJ_ID__BAL_OBJ_ID_INTERFACE:
	    {
	       BalInterfaceCfg intf_cfg;
	       memset(&intf_cfg, 0, sizeof(BalInterfaceCfg));
	       bal_interface_cfg__init(&intf_cfg);
	       get_cfg_resp.interface = &intf_cfg;

	       BalInterfaceCfgData intf_cfg_data;
	       memset(&intf_cfg_data, 0, sizeof(BalInterfaceCfgData));
	       bal_interface_cfg_data__init(&intf_cfg_data);
	       get_cfg_resp.interface->data = &intf_cfg_data;

	       BalInterfaceKey intf_key;
	       memset(&intf_key, 0, sizeof(BalInterfaceKey));
	       bal_interface_key__init(&intf_key);
	       get_cfg_resp.interface->key = &intf_key;

	       if (is_init_done == true) {
		  asfvolt16_bal_cfg_get(&get_cfg_resp);
	       }
	    }
	    break;

	 case BAL_OBJ_ID__BAL_OBJ_ID_SUBSCRIBER_TERMINAL:
	    {
	       ASFVOLT_LOG(ASFVOLT_ERROR, "Bal Get For Subscriber Terminal Received\n");

	       get_cfg_resp.obj_case = BAL_CFG__OBJ_TERMINAL;

	       get_cfg_resp.hdr->obj_type = BAL_OBJ_ID__BAL_OBJ_ID_SUBSCRIBER_TERMINAL;

	       BalSubscriberTerminalCfg sub_term_cfg;
	       memset(&sub_term_cfg, 0, sizeof(BalSubscriberTerminalCfg));
	       bal_subscriber_terminal_cfg__init(&sub_term_cfg);
	       get_cfg_resp.terminal = &sub_term_cfg;

	       BalSubscriberTerminalCfgData sub_term_cfg_data;
	       memset(&sub_term_cfg_data, 0, sizeof(BalSubscriberTerminalCfgData));
	       bal_subscriber_terminal_cfg_data__init(&sub_term_cfg_data);
	       get_cfg_resp.terminal->data = &sub_term_cfg_data;

	       BalSerialNumber serial_number;
	       memset(&serial_number, 0, sizeof(BalSerialNumber));
	       bal_serial_number__init(&serial_number);
	       get_cfg_resp.terminal->data->serial_number = &serial_number;

	       BalSubscriberTerminalKey sub_term_key;
	       memset(&sub_term_key, 0, sizeof(BalSubscriberTerminalKey));
	       bal_subscriber_terminal_key__init(&sub_term_key);
	       get_cfg_resp.terminal->key = &sub_term_key;

	       get_cfg_resp.terminal->key->has_sub_term_id =  true;
	       get_cfg_resp.terminal->key->sub_term_id =  bal_key->terminal_key->sub_term_id;

	       get_cfg_resp.terminal->key->has_intf_id =  true;
	       get_cfg_resp.terminal->key->intf_id =  bal_key->terminal_key->intf_id;

	       if (is_init_done == true) { 
		  asfvolt16_bal_cfg_get(&get_cfg_resp);
	       }
	    }
	    break;
	 default:
	    {
	       ASFVOLT_LOG(ASFVOLT_ERROR, "Bal Get not implemented for %d object type", bal_key->hdr->obj_type);
	    }
	    break;
      } 
   }

   /*
	* Write reply back to the client
	*/
   ret_val = context->gcc_stream->write(context, &get_cfg_resp, -1);
   is_grpc_write_pending(ret_val);

   grpc_c_status_t status;
   status.gcs_code = 0;

   /*
    * Finish response for RPC
    */
   if (context->gcc_stream->finish(context, &status))
   {
      ASFVOLT_LOG(ASFVOLT_ERROR, "Failed to write status");
   }
   bal_cfg_get__free_mem(&get_cfg_resp);
}

/*
 * This functions gets invoked whenever bal RPC gets called
 */
void bal__bal_cfg_set_cb(grpc_c_context_t *context)
{
   BalCfg *set_cfg = 0;
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
      ASFVOLT_LOG(ASFVOLT_ERROR, "Failed to write status");
   }

   if (set_cfg) {

      if(BAL_OBJ_ID__BAL_OBJ_ID_ACCESS_TERMINAL == set_cfg->hdr->obj_type)
      {
         sleep(5); /* enable this if running with gdb */
      }
   

      asfvolt16_bal_cfg_set(set_cfg);
   }
}


/*
 * This functions gets invoked whenever bal clear RPC gets called
 */
void bal__bal_cfg_clear_cb(grpc_c_context_t *context)
{
   BalKey *clear_key = 0;
   BalErr bal_err;
   int ret_val = 0;

   /*
    * Read incoming message into clear_key
    */
   if (context->gcc_payload) {
      context->gcc_stream->read(context, (void **)&clear_key, 0);
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
      ASFVOLT_LOG(ASFVOLT_ERROR, "Failed to write status");
   }

   if (clear_key) {
       asfvolt16_bal_cfg_clear(clear_key);
   }
}


/*
 * This functions gets invoked whenever bal Init RPC gets called
 */
void bal__bal_api_init_cb(grpc_c_context_t *context)
{
   BalInit *bal_init = 0;
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

   ASFVOLT_LOG(ASFVOLT_INFO, "Received API Init msg");

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
      ASFVOLT_LOG(ASFVOLT_ERROR, "Failed to write status");
   }

   if (bal_init) {
       asfvolt16_bal_init(bal_init, &coreIpPortInfo);
   }
   is_init_done = true;
}


void bal_get_ind__free_mem_access_term_ind_op_state(BalIndications *balIndCfg)
{
   free(balIndCfg->access_term_ind_op_state->data);
   free(balIndCfg->access_term_ind_op_state->key);
   free(balIndCfg->access_term_ind_op_state);
   free(balIndCfg);
}

void bal_get_ind__free_mem_access_term_proc_err(BalIndications *balIndCfg)
{
   free(balIndCfg->access_term_proc_err->key);
   free(balIndCfg->access_term_proc_err);
   free(balIndCfg);
}

void bal_get_ind__free_mem_flow_op_state(BalIndications *balIndCfg)
{
   free(balIndCfg->flow_op_state->data);
   free(balIndCfg->flow_op_state->key);
   free(balIndCfg->flow_op_state);
   free(balIndCfg);
}

void bal_get_ind__free_mem_flow_proc_err(BalIndications *balIndCfg)
{
   free(balIndCfg->flow_proc_err->key);
   free(balIndCfg->flow_proc_err);
   free(balIndCfg);
}

void bal_get_ind__free_mem_interface_op_state(BalIndications *balIndCfg)
{
   free(balIndCfg->interface_op_state->data);
   free(balIndCfg->interface_op_state->key);
   free(balIndCfg->interface_op_state);
   free(balIndCfg);
}

void bal_get_ind__free_mem_interface_los(BalIndications *balIndCfg)
{
   free(balIndCfg->interface_los->data);
   free(balIndCfg->interface_los);
   free(balIndCfg);
}

void bal_get_ind__free_mem_terminal_op_state(BalIndications *balIndCfg)
{
   free(balIndCfg->terminal_op_state->data);
   free(balIndCfg->terminal_op_state->key);
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
   free(balIndCfg->terminal_disc);
   free(balIndCfg);
}

void bal_get_ind__free_mem_terminal_alarm(BalIndications *balIndCfg)
{
   free(balIndCfg->terminal_alarm->data->alarm);
   free(balIndCfg->terminal_alarm->data);
   free(balIndCfg->terminal_alarm->key);
   free(balIndCfg->terminal_alarm);
   free(balIndCfg);
}

void bal_get_ind__free_mem_terminal_dgi(BalIndications *balIndCfg)
{
   free(balIndCfg->terminal_dgi->data);
   free(balIndCfg->terminal_dgi->key);
   free(balIndCfg->terminal_dgi);
   free(balIndCfg);
}

void bal_get_ind__free_mem_terminal_dowi(BalIndications *balIndCfg)
{
   free(balIndCfg->terminal_dowi->data);
   free(balIndCfg->terminal_dowi->key);
   free(balIndCfg->terminal_dowi);
   free(balIndCfg);
}

void bal_get_ind__free_mem_terminal_looci(BalIndications *balIndCfg)
{
   free(balIndCfg->terminal_looci->data);
   free(balIndCfg->terminal_looci->key);
   free(balIndCfg->terminal_looci);
   free(balIndCfg);
}

void bal_get_ind__free_mem_terminal_proc_err(BalIndications *balIndCfg)
{
   free(balIndCfg->terminal_proc_err->key);
   free(balIndCfg->terminal_proc_err);
   free(balIndCfg);
}

void bal_get_ind__free_mem_terminal_sdi(BalIndications *balIndCfg)
{
   free(balIndCfg->terminal_sdi->data);
   free(balIndCfg->terminal_sdi->key);
   free(balIndCfg->terminal_sdi);
   free(balIndCfg);
}

void bal_get_ind__free_mem_terminal_sfi(BalIndications *balIndCfg)
{
   free(balIndCfg->terminal_sfi->data);
   free(balIndCfg->terminal_sfi->key);
   free(balIndCfg->terminal_sfi);
   free(balIndCfg);
}

void bal_get_ind__free_mem_terminal_sufi(BalIndications *balIndCfg)
{
   free(balIndCfg->terminal_sufi->data);
   free(balIndCfg->terminal_sufi->key);
   free(balIndCfg->terminal_sufi);
   free(balIndCfg);
}

void bal_get_ind__free_mem_terminal_sub_term_act_fail(BalIndications *balIndCfg)
{
   free(balIndCfg->terminal_sub_term_act_fail->data);
   free(balIndCfg->terminal_sub_term_act_fail->key);
   free(balIndCfg->terminal_sub_term_act_fail);
   free(balIndCfg);
}

void bal_get_ind__free_mem_terminal_tiwi(BalIndications *balIndCfg)
{
   free(balIndCfg->terminal_tiwi->data);
   free(balIndCfg->terminal_tiwi->key);
   free(balIndCfg->terminal_tiwi);
   free(balIndCfg);
}

void bal_get_ind__free_mem_tm_sched_osc(BalIndications *balIndCfg)
{
   free(balIndCfg->tm_sched_oper_stats_change->data);
   free(balIndCfg->tm_sched_oper_stats_change->key);
   free(balIndCfg->tm_sched_oper_stats_change);
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
   free(balIndCfg->baloamresp);
   free(balIndCfg);
}

void bal_get_ind__free_mem(BalIndications *balIndCfg)
{
    switch (balIndCfg->u_case)
    {
       case BAL_INDICATIONS__U_ACCESS_TERM_IND_OP_STATE:
          bal_get_ind__free_mem_access_term_ind_op_state(balIndCfg);
          break;

       case BAL_INDICATIONS__U_ACCESS_TERM_PROC_ERR:
          bal_get_ind__free_mem_access_term_proc_err(balIndCfg);
          break;

       case BAL_INDICATIONS__U_FLOW_OP_STATE:
          bal_get_ind__free_mem_flow_op_state(balIndCfg);
          break;

       case BAL_INDICATIONS__U_FLOW_PROC_ERR:
          bal_get_ind__free_mem_flow_proc_err(balIndCfg);
          break;

       case BAL_INDICATIONS__U_INTERFACE_OP_STATE:
          bal_get_ind__free_mem_interface_op_state(balIndCfg);
          break;

       case BAL_INDICATIONS__U_INTERFACE_LOS:
          bal_get_ind__free_mem_interface_los(balIndCfg);
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

       case BAL_INDICATIONS__U_TERMINAL_DOWI:
          bal_get_ind__free_mem_terminal_dowi(balIndCfg);
          break;

       case BAL_INDICATIONS__U_TERMINAL_LOOCI:
          bal_get_ind__free_mem_terminal_looci(balIndCfg);
          break;

       case BAL_INDICATIONS__U_TERMINAL_PROC_ERR:
          bal_get_ind__free_mem_terminal_proc_err(balIndCfg);
          break;

       case BAL_INDICATIONS__U_TERMINAL_SDI:
          bal_get_ind__free_mem_terminal_sdi(balIndCfg);
          break;

       case BAL_INDICATIONS__U_TERMINAL_SFI:
          bal_get_ind__free_mem_terminal_sfi(balIndCfg);
          break;

       case BAL_INDICATIONS__U_TERMINAL_SUFI:
          bal_get_ind__free_mem_terminal_sufi(balIndCfg);
          break;

       case BAL_INDICATIONS__U_TERMINAL_SUB_TERM_ACT_FAIL:
          bal_get_ind__free_mem_terminal_sub_term_act_fail(balIndCfg);
	      break;

       case BAL_INDICATIONS__U_TERMINAL_TIWI:
          bal_get_ind__free_mem_terminal_tiwi(balIndCfg);
          break;

       case BAL_INDICATIONS__U_TM_SCHED_OPER_STATS_CHANGE:
          bal_get_ind__free_mem_tm_sched_osc(balIndCfg);
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
          ASFVOLT_LOG(ASFVOLT_ERROR, "Unknown BAL indication %u", balIndCfg->u_case);
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
      ASFVOLT_LOG(ASFVOLT_ERROR, "Failed to write status");
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

void bal_ind__bal_acc_term_oper_sts_cng_ind_cb(grpc_c_context_t *context)
{
}
void bal_ind__bal_acc_term_cfg_cb(grpc_c_context_t *context)
{
}
void bal_ind__bal_acc_term_proc_err_cb(grpc_c_context_t *context)
{
}
void bal_ind__bal_flow_oper_sts_cng_cb(grpc_c_context_t *context)
{
}
void bal_ind__bal_flow_proc_err_cb(grpc_c_context_t *context)
{
}
void bal_ind__bal_iface_oper_sts_cng_cb(grpc_c_context_t *context)
{
}
void bal_ind__bal_iface_los_cb(grpc_c_context_t *context)
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
void bal_ind__bal_subs_term_proc_err_cb(grpc_c_context_t *context)
{
}
void bal_ind__bal_subs_term_dowi_ind_cb(grpc_c_context_t *context)
{
}
void bal_ind__bal_subs_term_looci_ind_cb(grpc_c_context_t *context)
{
}
void bal_ind__bal_subs_term_sdi_ind_cb(grpc_c_context_t *context)
{
}
void bal_ind__bal_subs_term_sfi_ind_cb(grpc_c_context_t *context)
{
}
void bal_ind__bal_subs_term_sufi_ind_cb(grpc_c_context_t *context)
{
}
void bal_ind__bal_subs_term_tiwi_ind_cb(grpc_c_context_t *context)
{
}
void bal_ind__bal_sub_term_act_fail_ind_cb(grpc_c_context_t *context)
{
}
void bal_ind__bal_tm_sched_oper_stats_change_cb(grpc_c_context_t *context)
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
void bal_ind__bal_sys_profile_ind_cb(grpc_c_context_t *context)
{
}

/*
**    ASFVOLT_HEX2LOG(ASFVOLT_DEBUG, "OMCI Response with %zd bytes is",
**       balIndCfg.balomciresp->data->pkt.data, balIndCfg.balomciresp->data->pkt.len);
*/
#define SYSLOG_MAX_SIZE 1024
void asfvolt_hexlog2(int prio, const char *_file_loc, int _file_line, const char *pFormat, void* pData, size_t len)
{
      unsigned char *inpPtr = pData;
      char tempBuf[SYSLOG_MAX_SIZE];
      size_t idx = 0;

      idx += snprintf(tempBuf+idx, SYSLOG_MAX_SIZE-1, pFormat, len);
      while (len-- > 0 && idx < SYSLOG_MAX_SIZE)
      {
         idx += snprintf(tempBuf+idx, SYSLOG_MAX_SIZE-idx, "%02x", *inpPtr++);
      }
      ASFVOLT_LOG_LOC(prio, _file_loc, _file_line, "%s", tempBuf);
}

/*
 * Takes socket path as argument
 */
int main (int argc, char **argv)
{
   int i = 0;
   grpc_c_server_t *server = NULL;

   if (argc < 4)
   {
      fprintf(stderr, "Missing arguments\n");
      exit(1);
   }

   strcpy(coreIpPortInfo.bal_core_arg1, argv[2] /*, strlen(argv[2])*/);
   strcpy(coreIpPortInfo.bal_core_ip_port, argv[3]/*, strlen(argv[3])*/);

   signal(SIGINT, sigint_handler);

   /*
   ** syslog initialization
   */
   //setlogmask (LOG_UPTO (LOG_NOTICE));

   openlog (ASFVOLT_SYSLOG_NAME, ASFVOLT_SYSLOG_MODE, LOG_LOCAL1);

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
      ASFVOLT_LOG(ASFVOLT_ERROR, "Failed to create server");
      exit(1);
   }

   /*
    * Initialize greeter service
    */
   ASFVOLT_LOG(ASFVOLT_INFO, "voltha_bal_driver running.....");
   bal__service_init(test_server);
   bal_get_ind__service_init(test_server);
   asfvolt__service_init(test_server);

   /*
    * Start server
    */
   grpc_c_server_start(test_server);

   /*
    * Blocks server to wait to completion
    */
   grpc_c_server_wait(test_server);

   /* code added for example Makefile to compile grpc-c along with edgecore driver */
   bal__service_init(server);
   bal_get_ind__service_init(server);
   asfvolt__service_init(server);
}
