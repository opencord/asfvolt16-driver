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

#include "bal_indications_hdlr.h"
#include "bal_indications_queue.h"

/*global variables*/

/*extern variables*/


/********************************************************************\
 * Function    : bal_acc_term_osc_indication_cb                     *
 * Description : This function will handle the indications for      *
 *               Access Terminal Operational State Change           *
 *                                                                  *
 ********************************************************************/
bcmos_errno bal_acc_term_osc_indication_cb(bcmbal_obj *obj)
{
   bcmos_errno result = BCM_ERR_OK;

   if(BCMBAL_OBJ_ID_ACCESS_TERMINAL != obj->obj_type ||
      bcmbal_access_terminal_auto_id_oper_status_change != obj->subgroup)
   {
      ASFVOLT_LOG(ASFVOLT_ERROR, "Processing BAL API '%s' IND callback (status is %s)",
				  bcmbal_objtype_str(obj->obj_type), bcmos_strerror(obj->status));
      result = BCM_ERR_INTERNAL;
   }
   else
   {
      BalIndications *balIndCfg;
      balIndCfg = malloc(sizeof(BalIndications));
      memset(balIndCfg, 0, sizeof(BalIndications));
      bal_indications__init(balIndCfg);
      balIndCfg->has_objtype = BAL_GRPC_PRES;
      balIndCfg->objtype = obj->obj_type;
      balIndCfg->has_sub_group = BAL_GRPC_PRES;
      balIndCfg->sub_group = obj->subgroup;
      balIndCfg->u_case = BAL_INDICATIONS__U_ACCESS_TERM_IND_OP_STATE;
      balIndCfg->device_id = voltha_device_id;

      bcmbal_access_terminal_oper_status_change *acc_term_osc =
                                  (bcmbal_access_terminal_oper_status_change *)obj;

      BalAccessTerminalOperStatusChange *acessTermOSC;
      acessTermOSC = malloc(sizeof(BalAccessTerminalOperStatusChange));
      memset(acessTermOSC, 0, sizeof(BalAccessTerminalOperStatusChange));
      bal_access_terminal_oper_status_change__init(acessTermOSC);
      balIndCfg->access_term_ind_op_state = acessTermOSC;

	  /*'hdr' field is not parsed by voltha adapter, hence not filled */

      BalAccessTerminalKey *accessTermkey;
      accessTermkey = malloc(sizeof(BalAccessTerminalKey));
      memset(accessTermkey, 0, sizeof(BalAccessTerminalKey));
      bal_access_terminal_key__init(accessTermkey);
      balIndCfg->access_term_ind_op_state->key = accessTermkey;
      balIndCfg->access_term_ind_op_state->key->has_access_term_id = BAL_GRPC_PRES;
      balIndCfg->access_term_ind_op_state->key->access_term_id =
                                                  acc_term_osc->key.access_term_id;

      BalAccessTerminalOperStatusChangeData *accessTermOscData;
      accessTermOscData = malloc(sizeof(BalAccessTerminalOperStatusChangeData));
      memset(accessTermOscData, 0, sizeof(BalAccessTerminalOperStatusChangeData));
      bal_access_terminal_oper_status_change_data__init(accessTermOscData);
      balIndCfg->access_term_ind_op_state->data = accessTermOscData;
      balIndCfg->access_term_ind_op_state->data->has_new_oper_status = BAL_GRPC_PRES;
      balIndCfg->access_term_ind_op_state->data->new_oper_status =
                                                 acc_term_osc->data.new_oper_status;
      balIndCfg->access_term_ind_op_state->data->has_old_oper_status = BAL_GRPC_PRES;
      balIndCfg->access_term_ind_op_state->data->old_oper_status =
                                                 acc_term_osc->data.old_oper_status;
      balIndCfg->access_term_ind_op_state->data->has_admin_state = BAL_GRPC_PRES;
      balIndCfg->access_term_ind_op_state->data->admin_state =
                                                 acc_term_osc->data.admin_state;

      bal_register_indication_cbs();

      list_node *bal_indication_node = malloc(sizeof(list_node));
      bal_indication_node->bal_indication = balIndCfg;

      pthread_mutex_lock(&bal_ind_queue_lock);
      add_bal_indication_node(bal_indication_node);
      pthread_mutex_unlock(&bal_ind_queue_lock);
      is_reboot = BAL_REBOOT_STATUS__BAL_OLT_UP_AFTER_ACTIVATION;
   }

   return result;
}

/********************************************************************\
 * Function    : bal_acc_term_processing_error_indication_cb        *
 * Description : This function will handle the error indications    *
 *               for Access Terminal                                *
 *                                                                  *
 ********************************************************************/
bcmos_errno bal_acc_term_processing_error_indication_cb(bcmbal_obj *obj)
{
   bcmos_errno result = BCM_ERR_OK;

   if(BCMBAL_OBJ_ID_ACCESS_TERMINAL != obj->obj_type ||
      bcmbal_access_terminal_auto_id_processing_error != obj->subgroup)
   {
      ASFVOLT_LOG(ASFVOLT_ERROR, "Processing BAL API '%s' IND callback (status is %s)",
				  bcmbal_objtype_str(obj->obj_type), bcmos_strerror(obj->status));
      result = BCM_ERR_INTERNAL;
   }
   else
   {
      BalIndications *balIndCfg;
      balIndCfg = malloc(sizeof(BalIndications));
      memset(balIndCfg, 0, sizeof(BalIndications));
      bal_indications__init(balIndCfg);
      balIndCfg->has_objtype = BAL_GRPC_PRES;
      balIndCfg->objtype = obj->obj_type;
      balIndCfg->has_sub_group = BAL_GRPC_PRES;
      balIndCfg->sub_group = obj->subgroup;
      balIndCfg->u_case = BAL_INDICATIONS__U_ACCESS_TERM_PROC_ERR;
      balIndCfg->device_id = voltha_device_id;

      bcmbal_access_terminal_processing_error *acc_term_err =
                                  (bcmbal_access_terminal_processing_error *)obj;

      BalAccessTerminalProcessingError *acessTermError;
      acessTermError = malloc(sizeof(BalAccessTerminalProcessingError));
      memset(acessTermError, 0, sizeof(BalAccessTerminalProcessingError));
      bal_access_terminal_processing_error__init(acessTermError);
      balIndCfg->access_term_proc_err = acessTermError;

	  /*'hdr' field is not parsed by voltha adapter, hence not filled */

      BalAccessTerminalKey *accessTermkey;
      accessTermkey = malloc(sizeof(BalAccessTerminalKey));
      memset(accessTermkey, 0, sizeof(BalAccessTerminalKey));
      bal_access_terminal_key__init(accessTermkey);
      balIndCfg->access_term_proc_err->key = accessTermkey;
      balIndCfg->access_term_proc_err->key->has_access_term_id = BAL_GRPC_PRES;
      balIndCfg->access_term_proc_err->key->access_term_id =
                                              acc_term_err->key.access_term_id;

      list_node *bal_indication_node = malloc(sizeof(list_node));
      bal_indication_node->bal_indication = balIndCfg;

      pthread_mutex_lock(&bal_ind_queue_lock);
      add_bal_indication_node(bal_indication_node);
      pthread_mutex_unlock(&bal_ind_queue_lock);
   }

   return result;
}

/********************************************************************\
 * Function    : bal_flow_osc_indication_cb                         *
 * Description : This function will handle the indications for      *
 *               Flow Operational State Change                      *
 *                                                                  *
 ********************************************************************/
bcmos_errno bal_flow_osc_indication_cb(bcmbal_obj *obj)
{
   bcmos_errno result = BCM_ERR_OK;

   if(BCMBAL_OBJ_ID_FLOW != obj->obj_type ||
      bcmbal_flow_auto_id_oper_status_change != obj->subgroup)
   {
      ASFVOLT_LOG(ASFVOLT_ERROR, "Processing BAL API '%s' IND callback (status is %s)",
				  bcmbal_objtype_str(obj->obj_type), bcmos_strerror(obj->status));
      result = BCM_ERR_INTERNAL;
   }
   else
   {
      BalIndications *balIndCfg;
      balIndCfg = malloc(sizeof(BalIndications));
      memset(balIndCfg, 0, sizeof(BalIndications));
      bal_indications__init(balIndCfg);
      balIndCfg->u_case = BAL_INDICATIONS__U_FLOW_OP_STATE;
      balIndCfg->has_objtype = BAL_GRPC_PRES;
      balIndCfg->objtype = obj->obj_type;
      balIndCfg->has_sub_group = BAL_GRPC_PRES;
      balIndCfg->sub_group = obj->subgroup;
      balIndCfg->device_id = voltha_device_id;

      bcmbal_flow_oper_status_change *flow_osc = (bcmbal_flow_oper_status_change *)obj;

      BalFlowOperStatusChange *flowOscInd;
      flowOscInd = malloc(sizeof(BalFlowOperStatusChange));
      memset(flowOscInd, 0, sizeof(BalFlowOperStatusChange));
      bal_flow_oper_status_change__init(flowOscInd);
      balIndCfg->flow_op_state = flowOscInd;

	  /*'hdr' field is not parsed by voltha adapter, hence not filled */

      BalFlowKey *flowkey;
      flowkey = malloc(sizeof(BalFlowKey));
      memset(flowkey, 0, sizeof(BalFlowKey));
      bal_flow_key__init(flowkey);
      balIndCfg->flow_op_state->key = flowkey;

      balIndCfg->flow_op_state->key->has_flow_id = BAL_GRPC_PRES;
      balIndCfg->flow_op_state->key->flow_id = flow_osc->key.flow_id;
      balIndCfg->flow_op_state->key->has_flow_type = BAL_GRPC_PRES;
      balIndCfg->flow_op_state->key->flow_type = flow_osc->key.flow_type;

      BalFlowOperStatusChangeData *flowOscIndData;
      flowOscIndData = malloc(sizeof(BalFlowOperStatusChangeData));
      memset(flowOscIndData, 0, sizeof(BalFlowOperStatusChangeData));
      bal_flow_oper_status_change_data__init(flowOscIndData);
      balIndCfg->flow_op_state->data = flowOscIndData;

      balIndCfg->flow_op_state->data->has_new_oper_status = BAL_GRPC_PRES;
      balIndCfg->flow_op_state->data->new_oper_status = flow_osc->data.new_oper_status;
      balIndCfg->flow_op_state->data->has_old_oper_status = BAL_GRPC_PRES;
      balIndCfg->flow_op_state->data->old_oper_status = flow_osc->data.old_oper_status;
      balIndCfg->flow_op_state->data->has_admin_state = BAL_GRPC_PRES;
      balIndCfg->flow_op_state->data->admin_state = flow_osc->data.admin_state;
      balIndCfg->flow_op_state->data->has_svc_port_id = BAL_GRPC_PRES;
      balIndCfg->flow_op_state->data->svc_port_id = flow_osc->data.svc_port_id;
      balIndCfg->flow_op_state->data->has_dba_tm_sched_id = BAL_GRPC_PRES;
      balIndCfg->flow_op_state->data->dba_tm_sched_id = flow_osc->data.dba_tm_sched_id;
      balIndCfg->flow_op_state->data->has_cookie = BAL_GRPC_PRES;
      balIndCfg->flow_op_state->data->cookie = flow_osc->data.cookie;

      list_node *bal_indication_node = malloc(sizeof(list_node));
      bal_indication_node->bal_indication = balIndCfg;

      pthread_mutex_lock(&bal_ind_queue_lock);
      add_bal_indication_node(bal_indication_node);
      pthread_mutex_unlock(&bal_ind_queue_lock);
   }

   return result;
}

/********************************************************************\
 * Function    : bal_flow_processing_error_indication_cb            *
 * Description : This function will handle flow processing errors   *
 *                                                                  *
 ********************************************************************/
bcmos_errno bal_flow_processing_error_indication_cb(bcmbal_obj *obj)
{
    bcmos_errno result = BCM_ERR_OK;

   if(BCMBAL_OBJ_ID_FLOW != obj->obj_type ||
      bcmbal_flow_auto_id_processing_error != obj->subgroup)
   {
      ASFVOLT_LOG(ASFVOLT_ERROR, "Processing BAL API '%s' IND callback (status is %s)",
				  bcmbal_objtype_str(obj->obj_type), bcmos_strerror(obj->status));
      result = BCM_ERR_INTERNAL;
   }
   else
   {
      BalIndications *balIndCfg;
      balIndCfg = malloc(sizeof(BalIndications));
      memset(balIndCfg, 0, sizeof(BalIndications));
      bal_indications__init(balIndCfg);
      balIndCfg->has_objtype = BAL_GRPC_PRES;
      balIndCfg->objtype = obj->obj_type;
      balIndCfg->has_sub_group = BAL_GRPC_PRES;
      balIndCfg->sub_group = obj->subgroup;
      balIndCfg->u_case = BAL_INDICATIONS__U_FLOW_PROC_ERR;
      balIndCfg->device_id = voltha_device_id;

      bcmbal_flow_processing_error *flow_proc_error =
                                  (bcmbal_flow_processing_error *)obj;

      BalFlowProcessingError *flowProcError;
      flowProcError = malloc(sizeof(BalFlowProcessingError));
      memset(flowProcError, 0, sizeof(BalFlowProcessingError));
      bal_flow_processing_error__init(flowProcError);
      balIndCfg->flow_proc_err = flowProcError;

	  /*'hdr' field is not parsed by voltha adapter, hence not filled */

      BalFlowKey *flowKey;
      flowKey = malloc(sizeof(BalFlowKey));
      memset(flowKey, 0, sizeof(BalFlowKey));
      bal_flow_key__init(flowKey);

      balIndCfg->flow_proc_err->key = flowKey;

      balIndCfg->flow_proc_err->key->has_flow_id = BAL_GRPC_PRES;
      balIndCfg->flow_proc_err->key->flow_id =
                                    flow_proc_error->key.flow_id;

      balIndCfg->flow_proc_err->key->has_flow_type = BAL_GRPC_PRES;
      balIndCfg->flow_proc_err->key->flow_type =
                                    flow_proc_error->key.flow_type;

      list_node *bal_indication_node = malloc(sizeof(list_node));
      bal_indication_node->bal_indication = balIndCfg;

      pthread_mutex_lock(&bal_ind_queue_lock);
      add_bal_indication_node(bal_indication_node);
      pthread_mutex_unlock(&bal_ind_queue_lock);
   }


    return result;
}


/********************************************************************\
 * Function    : bal_interface_osc_indication_cb                    *
 * Description : This function will handle the indications for      *
 *               Interface Operational State Change                 *
 *                                                                  *
 ********************************************************************/
bcmos_errno bal_interface_osc_indication_cb(bcmbal_obj *obj)
{
   bcmos_errno result = BCM_ERR_OK;

   if(BCMBAL_OBJ_ID_INTERFACE != obj->obj_type ||
      bcmbal_interface_auto_id_oper_status_change != obj->subgroup)
   {
      ASFVOLT_LOG(ASFVOLT_ERROR, "Processing BAL API '%s' IND callback (status is %s)",
				  bcmbal_objtype_str(obj->obj_type), bcmos_strerror(obj->status));
      result = BCM_ERR_INTERNAL;
   }
   else
   {
      BalIndications *balIndCfg;
      balIndCfg = malloc(sizeof(BalIndications));
      memset(balIndCfg, 0, sizeof(BalIndications));
      bal_indications__init(balIndCfg);
      balIndCfg->u_case = BAL_INDICATIONS__U_INTERFACE_OP_STATE;
      balIndCfg->has_objtype = BAL_GRPC_PRES;
      balIndCfg->objtype = obj->obj_type;
      balIndCfg->has_sub_group = BAL_GRPC_PRES;
      balIndCfg->sub_group = obj->subgroup;
      balIndCfg->device_id = voltha_device_id;

      bcmbal_interface_oper_status_change *int_osc_ind =
                                 (bcmbal_interface_oper_status_change *)obj;

      BalInterfaceOperStatusChange *ifOsc;
      ifOsc = malloc(sizeof(BalInterfaceOperStatusChange));
      memset(ifOsc, 0, sizeof(BalInterfaceOperStatusChange));
      bal_interface_oper_status_change__init(ifOsc);
      balIndCfg->interface_op_state = ifOsc;

	  /*'hdr' field is not parsed by voltha adapter, hence not filled */

      BalInterfaceKey *ifkey;
      ifkey = malloc(sizeof(BalInterfaceKey));
      memset(ifkey, 0, sizeof(BalInterfaceKey));
      bal_interface_key__init(ifkey);
      balIndCfg->interface_op_state->key = ifkey;

      balIndCfg->interface_op_state->key->has_intf_id = BAL_GRPC_PRES;
      balIndCfg->interface_op_state->key->intf_id = int_osc_ind->key.intf_id;
      balIndCfg->interface_op_state->key->has_intf_type = BAL_GRPC_PRES;
      balIndCfg->interface_op_state->key->intf_type = int_osc_ind->key.intf_type;

      BalInterfaceOperStatusChangeData *ifOscIndData;
      ifOscIndData = malloc(sizeof(BalInterfaceOperStatusChangeData));
      memset(ifOscIndData, 0, sizeof(BalInterfaceOperStatusChangeData));
      bal_interface_oper_status_change_data__init(ifOscIndData);
      balIndCfg->interface_op_state->data = ifOscIndData;

      balIndCfg->interface_op_state->data->has_new_oper_status = BAL_GRPC_PRES;
      balIndCfg->interface_op_state->data->new_oper_status = int_osc_ind->data.new_oper_status;
      balIndCfg->interface_op_state->data->has_old_oper_status = BAL_GRPC_PRES;
      balIndCfg->interface_op_state->data->old_oper_status = int_osc_ind->data.old_oper_status;
      balIndCfg->interface_op_state->data->has_admin_state = BAL_GRPC_PRES;
      balIndCfg->interface_op_state->data->admin_state = int_osc_ind->data.admin_state;

      list_node *bal_indication_node = malloc(sizeof(list_node));
      bal_indication_node->bal_indication = balIndCfg;

      pthread_mutex_lock(&bal_ind_queue_lock);
      add_bal_indication_node(bal_indication_node);
      pthread_mutex_unlock(&bal_ind_queue_lock);
   }

   return result;
}

/********************************************************************\
 * Function    : bal_interface_los_indication_cb                    *
 * Description : This function will handle the indications for      *
 *               Interface los Indication                           *
 *                                                                  *
 ********************************************************************/
bcmos_errno bal_interface_los_indication_cb(bcmbal_obj *obj)
{
   bcmos_errno result = BCM_ERR_OK;

   if(BCMBAL_OBJ_ID_INTERFACE != obj->obj_type ||
      bcmbal_interface_auto_id_los != obj->subgroup)
   {
      ASFVOLT_LOG(ASFVOLT_ERROR, "Processing BAL API '%s' IND callback (status is %s)",
				  bcmbal_objtype_str(obj->obj_type), bcmos_strerror(obj->status));
      result = BCM_ERR_INTERNAL;
   }
   else
   {
      BalIndications *balIndCfg;
      balIndCfg = malloc(sizeof(BalIndications));
      memset(balIndCfg, 0, sizeof(BalIndications));
      bal_indications__init(balIndCfg);
      balIndCfg->u_case = BAL_INDICATIONS__U_INTERFACE_LOS;
      balIndCfg->has_objtype = BAL_GRPC_PRES;
      balIndCfg->objtype = obj->obj_type;
      balIndCfg->has_sub_group = BAL_GRPC_PRES;
      balIndCfg->sub_group = obj->subgroup;
      balIndCfg->device_id = voltha_device_id;

      bcmbal_interface_los *int_los_ind =
                                 (bcmbal_interface_los *)obj;

      BalInterfaceLos *ifLos;
      ifLos = malloc(sizeof(BalInterfaceLos));
      memset(ifLos, 0, sizeof(BalInterfaceLos));
      bal_interface_los__init(ifLos);
      balIndCfg->interface_los = ifLos;

	  /*'hdr' field is not parsed by voltha adapter, hence not filled */

      BalInterfaceKey *ifkey;
      ifkey = malloc(sizeof(BalInterfaceKey));
      memset(ifkey, 0, sizeof(BalInterfaceKey));
      bal_interface_key__init(ifkey);
      balIndCfg->interface_los->key = ifkey;

      balIndCfg->interface_los->key->has_intf_id = BAL_GRPC_PRES;
      balIndCfg->interface_los->key->intf_id = int_los_ind->key.intf_id;
      balIndCfg->interface_los->key->has_intf_type = BAL_GRPC_PRES;
      balIndCfg->interface_los->key->intf_type = int_los_ind->key.intf_type;

      BalInterfaceLosData *ifLosIndData;
      ifLosIndData = malloc(sizeof(BalInterfaceLosData));
      memset(ifLosIndData, 0, sizeof(BalInterfaceLosData));
      bal_interface_los_data__init(ifLosIndData);
      balIndCfg->interface_los->data = ifLosIndData;

      balIndCfg->interface_los->data->has_status = BAL_GRPC_PRES;
      balIndCfg->interface_los->data->status = int_los_ind->data.status;

      list_node *bal_indication_node = malloc(sizeof(list_node));
      bal_indication_node->bal_indication = balIndCfg;

      pthread_mutex_lock(&bal_ind_queue_lock);
      add_bal_indication_node(bal_indication_node);
      pthread_mutex_unlock(&bal_ind_queue_lock);
   }

   return result;
}

/********************************************************************\
 * Function    : bal_sub_term_osc_indication_cb                     *
 * Description : This function will handle the indications for      *
 *               Subscriber term Operational State Change           *
 *                                                                  *
 ********************************************************************/
bcmos_errno bal_sub_term_osc_indication_cb(bcmbal_obj *obj)
{
   bcmos_errno result = BCM_ERR_OK;

   if(BCMBAL_OBJ_ID_SUBSCRIBER_TERMINAL != obj->obj_type ||
      bcmbal_subscriber_terminal_auto_id_oper_status_change != obj->subgroup)
   {
      ASFVOLT_LOG(ASFVOLT_ERROR, "Processing BAL API '%s' IND callback (status is %s)",
				  bcmbal_objtype_str(obj->obj_type), bcmos_strerror(obj->status));
      result = BCM_ERR_INTERNAL;
   }
   else
   {
      BalIndications *balIndCfg;
      balIndCfg = malloc(sizeof(BalIndications));
      memset(balIndCfg, 0, sizeof(BalIndications));
      bal_indications__init(balIndCfg);
      balIndCfg->u_case = BAL_INDICATIONS__U_TERMINAL_OP_STATE;
      balIndCfg->has_objtype = BAL_GRPC_PRES;
      balIndCfg->objtype = obj->obj_type;
      balIndCfg->has_sub_group = BAL_GRPC_PRES;
      balIndCfg->sub_group = obj->subgroup;
      balIndCfg->device_id = voltha_device_id;

      bcmbal_subscriber_terminal_oper_status_change *sub_osc_ind =
                           (bcmbal_subscriber_terminal_oper_status_change *)obj;

      BalSubscriberTerminalOperStatusChange *subOscInd;
      subOscInd = malloc(sizeof(BalSubscriberTerminalOperStatusChange));
      memset(subOscInd, 0, sizeof(BalSubscriberTerminalOperStatusChange));
      bal_subscriber_terminal_oper_status_change__init(subOscInd);
      balIndCfg->terminal_op_state = subOscInd;

	  /*'hdr' field is not parsed by voltha adapter, hence not filled */

      BalSubscriberTerminalKey *subkey;
      subkey = malloc(sizeof(BalSubscriberTerminalKey));
      memset(subkey, 0, sizeof(BalSubscriberTerminalKey));
      bal_subscriber_terminal_key__init(subkey);
      balIndCfg->terminal_op_state->key = subkey;

      balIndCfg->terminal_op_state->key->has_intf_id = BAL_GRPC_PRES;
      balIndCfg->terminal_op_state->key->intf_id = sub_osc_ind->key.intf_id;
      balIndCfg->terminal_op_state->key->has_sub_term_id = BAL_GRPC_PRES;
      balIndCfg->terminal_op_state->key->sub_term_id = sub_osc_ind->key.sub_term_id;

      BalSubscriberTerminalOperStatusChangeData *subOscIndData;
      subOscIndData = malloc(sizeof(BalSubscriberTerminalOperStatusChangeData));
      memset(subOscIndData, 0, sizeof(BalSubscriberTerminalOperStatusChangeData));
      bal_subscriber_terminal_oper_status_change_data__init(subOscIndData);
      balIndCfg->terminal_op_state->data = subOscIndData;

      balIndCfg->terminal_op_state->data->has_new_oper_status = BAL_GRPC_PRES;
      balIndCfg->terminal_op_state->data->new_oper_status =
                                           sub_osc_ind->data.new_oper_status;
      balIndCfg->terminal_op_state->data->has_old_oper_status = BAL_GRPC_PRES;
      balIndCfg->terminal_op_state->data->old_oper_status =
                                           sub_osc_ind->data.old_oper_status;
      balIndCfg->terminal_op_state->data->has_admin_state = BAL_GRPC_PRES;
      balIndCfg->terminal_op_state->data->admin_state =
                                               sub_osc_ind->data.admin_state;

      list_node *bal_indication_node = malloc(sizeof(list_node));
      bal_indication_node->bal_indication = balIndCfg;

      pthread_mutex_lock(&bal_ind_queue_lock);
      add_bal_indication_node(bal_indication_node);
      pthread_mutex_unlock(&bal_ind_queue_lock);
   }

   return result;
}

/********************************************************************\
 * Function    : bal_sub_term_disc_indication_cb                    *
 * Description : This function will handle the indications for      *
 *               Subscriber term disc indication                    *
 *                                                                  *
 ********************************************************************/
bcmos_errno bal_sub_term_disc_indication_cb(bcmbal_obj *obj)
{
   bcmos_errno result = BCM_ERR_OK;

   if(BCMBAL_OBJ_ID_SUBSCRIBER_TERMINAL != obj->obj_type ||
      bcmbal_subscriber_terminal_auto_id_sub_term_disc != obj->subgroup)
   {
      ASFVOLT_LOG(ASFVOLT_ERROR, "Processing BAL API '%s' IND callback (status is %s)",
				  bcmbal_objtype_str(obj->obj_type), bcmos_strerror(obj->status));
      result = BCM_ERR_INTERNAL;
   }
   else
   {
      ASFVOLT_LOG(ASFVOLT_DEBUG, "Processing BAL API '%s' IND callback (status is %s)",
				  bcmbal_objtype_str(obj->obj_type), bcmos_strerror(obj->status));
      BalIndications *balIndCfg;
      balIndCfg = malloc(sizeof(BalIndications));
      memset(balIndCfg, 0, sizeof(BalIndications));
      bal_indications__init(balIndCfg);
      balIndCfg->u_case = BAL_INDICATIONS__U_TERMINAL_DISC;
      balIndCfg->has_objtype = BAL_GRPC_PRES;
      balIndCfg->objtype = obj->obj_type;
      balIndCfg->has_sub_group = BAL_GRPC_PRES;
      balIndCfg->sub_group = obj->subgroup;
      balIndCfg->device_id = voltha_device_id;

      bcmbal_subscriber_terminal_sub_term_disc *sub_disc_ind =
                           (bcmbal_subscriber_terminal_sub_term_disc *)obj;

      BalSubscriberTerminalSubTermDisc *subDiscInd;
      subDiscInd = malloc(sizeof(BalSubscriberTerminalSubTermDisc));
      memset(subDiscInd, 0, sizeof(BalSubscriberTerminalSubTermDisc));
      bal_subscriber_terminal_sub_term_disc__init(subDiscInd);
      balIndCfg->terminal_disc = subDiscInd;

	  /*'hdr' field is not parsed by voltha adapter, hence not filled */

      BalSubscriberTerminalKey *subkey;
      subkey = malloc(sizeof(BalSubscriberTerminalKey));
      memset(subkey, 0, sizeof(BalSubscriberTerminalKey));
      bal_subscriber_terminal_key__init(subkey);
      balIndCfg->terminal_disc->key = subkey;

      balIndCfg->terminal_disc->key->has_intf_id = BAL_GRPC_PRES;
      balIndCfg->terminal_disc->key->intf_id = sub_disc_ind->key.intf_id;
      balIndCfg->terminal_disc->key->has_sub_term_id = BAL_GRPC_PRES;
      balIndCfg->terminal_disc->key->sub_term_id = sub_disc_ind->key.sub_term_id;

      BalSubscriberTerminalSubTermDiscData *subDiscIndData;
      subDiscIndData = malloc(sizeof(BalSubscriberTerminalSubTermDiscData));
      memset(subDiscIndData, 0, sizeof(BalSubscriberTerminalSubTermDiscData));
      bal_subscriber_terminal_sub_term_disc_data__init(subDiscIndData);
      balIndCfg->terminal_disc->data = subDiscIndData;

      BalSerialNumber *serial_number;
      serial_number = malloc(sizeof(BalSerialNumber));
      memset(serial_number, 0, sizeof(BalSerialNumber));
      bal_serial_number__init(serial_number);

      balIndCfg->terminal_disc->data->serial_number = serial_number;

      ASFVOLT_LOG(ASFVOLT_INFO, "Discovered ONU serial number "
                "%c%c%c%c%02X%02X%02X%02X",
                sub_disc_ind->data.serial_number.vendor_id[0],
                sub_disc_ind->data.serial_number.vendor_id[1],
                sub_disc_ind->data.serial_number.vendor_id[2],
                sub_disc_ind->data.serial_number.vendor_id[3],
                sub_disc_ind->data.serial_number.vendor_specific[0],
                sub_disc_ind->data.serial_number.vendor_specific[1],
                sub_disc_ind->data.serial_number.vendor_specific[2],
                sub_disc_ind->data.serial_number.vendor_specific[3]);

      //ASFVOLT_LOG(ASFVOLT_DEBUG, "ONU Discovery:Before decoding:Vendor id is %s", sub_disc_ind->data.serial_number.vendor_id);
      //ASFVOLT_LOG(ASFVOLT_DEBUG, "ONU Discovery:Before decoding:Vendor specific is %s", sub_disc_ind->data.serial_number.vendor_specific);

      char *vendor_id = malloc(sizeof(char)*MAX_CHAR_LENGTH);
      memset(vendor_id, 0, MAX_CHAR_LENGTH);
      sprintf(vendor_id,"%c%c%c%c",
		      sub_disc_ind->data.serial_number.vendor_id[0],
		      sub_disc_ind->data.serial_number.vendor_id[1],
		      sub_disc_ind->data.serial_number.vendor_id[2],
		      sub_disc_ind->data.serial_number.vendor_id[3]);
      balIndCfg->terminal_disc->data->serial_number->vendor_id = vendor_id;
      ASFVOLT_LOG(ASFVOLT_DEBUG, "ONU Discovery:After decoding:Vendor id is %s", balIndCfg->terminal_disc->data->serial_number->vendor_id);
      char *vendor_specific = malloc(sizeof(char)*MAX_CHAR_LENGTH);
      memset(vendor_specific, 0, MAX_CHAR_LENGTH);
      sprintf(vendor_specific,"%1X%1X%1X%1X%1X%1X%1X%1X",
		      sub_disc_ind->data.serial_number.vendor_specific[0]>>4 & 0x0f,
		      sub_disc_ind->data.serial_number.vendor_specific[0] & 0x0f,
		      sub_disc_ind->data.serial_number.vendor_specific[1]>>4 & 0x0f,
		      sub_disc_ind->data.serial_number.vendor_specific[1] & 0x0f,
		      sub_disc_ind->data.serial_number.vendor_specific[2]>>4 & 0x0f,
		      sub_disc_ind->data.serial_number.vendor_specific[2] & 0x0f,
		      sub_disc_ind->data.serial_number.vendor_specific[3]>>4 & 0x0f,
		      sub_disc_ind->data.serial_number.vendor_specific[3] & 0x0f);
      balIndCfg->terminal_disc->data->serial_number->vendor_specific = vendor_specific;
      ASFVOLT_LOG(ASFVOLT_DEBUG, "ONU Discovery:After decoding:Vendor specific is %s", balIndCfg->terminal_disc->data->serial_number->vendor_specific);

      list_node *bal_indication_node = malloc(sizeof(list_node));
      bal_indication_node->bal_indication = balIndCfg;

      pthread_mutex_lock(&bal_ind_queue_lock);
      add_bal_indication_node(bal_indication_node);
      pthread_mutex_unlock(&bal_ind_queue_lock);
   }

   return result;
}

/********************************************************************\
 * Function    : bal_sub_term_alarm_indication_cb                   *
 * Description : This function will handle the indications for      *
 *               Subscriber term alarm indication                   *
 *                                                                  *
 ********************************************************************/
bcmos_errno bal_sub_term_alarm_indication_cb(bcmbal_obj *obj)
{
   bcmos_errno result = BCM_ERR_OK;

   if(BCMBAL_OBJ_ID_SUBSCRIBER_TERMINAL != obj->obj_type ||
      bcmbal_subscriber_terminal_auto_id_sub_term_alarm != obj->subgroup)
   {
      ASFVOLT_LOG(ASFVOLT_ERROR, "Processing BAL API \'%s\' IND callback (status is %s)",
				  bcmbal_objtype_str(obj->obj_type), bcmos_strerror(obj->status));
      result = BCM_ERR_INTERNAL;
   }
   else
   {
      BalIndications *balIndCfg;
      balIndCfg = malloc(sizeof(BalIndications));
      memset(balIndCfg, 0, sizeof(BalIndications));
      bal_indications__init(balIndCfg);
      balIndCfg->has_objtype = BAL_GRPC_PRES;
      balIndCfg->objtype = BAL_OBJ_ID__BAL_OBJ_ID_SUBSCRIBER_TERMINAL;
      balIndCfg->has_sub_group = BAL_GRPC_PRES;
      balIndCfg->sub_group = obj->subgroup;
      balIndCfg->device_id = voltha_device_id;

      balIndCfg->u_case = BAL_INDICATIONS__U_TERMINAL_ALARM;
      bcmbal_subscriber_terminal_sub_term_alarm *sub_term_alarm  =
                           (bcmbal_subscriber_terminal_sub_term_alarm *)obj;

      BalSubscriberTerminalSubTermAlarm *subTermAlarm;
      subTermAlarm = malloc(sizeof(BalSubscriberTerminalSubTermAlarm));
      memset(subTermAlarm, 0, sizeof(BalSubscriberTerminalSubTermAlarm));
      bal_subscriber_terminal_sub_term_alarm__init(subTermAlarm);
      balIndCfg->terminal_alarm = subTermAlarm;

	  /*'hdr' field is not parsed by voltha adapter, hence not filled */

      BalSubscriberTerminalKey *subkey;
      subkey = malloc(sizeof(BalSubscriberTerminalKey));
      memset(subkey, 0, sizeof(BalSubscriberTerminalKey));
      bal_subscriber_terminal_key__init(subkey);
      balIndCfg->terminal_alarm->key = subkey;

      balIndCfg->terminal_alarm->key->has_sub_term_id = BAL_GRPC_PRES;
      balIndCfg->terminal_alarm->key->sub_term_id = sub_term_alarm->key.sub_term_id;
      balIndCfg->terminal_alarm->key->has_intf_id = BAL_GRPC_PRES;
      balIndCfg->terminal_alarm->key->intf_id = sub_term_alarm->key.intf_id;

      BalsubscriberTerminalSubTermAlarmData *subTermAlarmData;
      subTermAlarmData = malloc(sizeof(BalsubscriberTerminalSubTermAlarmData));
      memset(subTermAlarmData, 0, sizeof(BalsubscriberTerminalSubTermAlarmData));
      balsubscriber_terminal_sub_term_alarm_data__init(subTermAlarmData);
      balIndCfg->terminal_alarm->data = subTermAlarmData;

      BalSubscriberTerminalAlarms *balSubAlarms;
      balSubAlarms = malloc(sizeof(BalSubscriberTerminalAlarms));
      memset(balSubAlarms, 0, sizeof(BalSubscriberTerminalAlarms));
      bal_subscriber_terminal_alarms__init(balSubAlarms);
      balIndCfg->terminal_alarm->data->alarm = balSubAlarms;

      balIndCfg->terminal_alarm->data->alarm->has_los = BAL_GRPC_PRES;
      balIndCfg->terminal_alarm->data->alarm->los = sub_term_alarm->data.alarm.los;
      balIndCfg->terminal_alarm->data->alarm->has_lob = BAL_GRPC_PRES;
      balIndCfg->terminal_alarm->data->alarm->lob = sub_term_alarm->data.alarm.lob;
      balIndCfg->terminal_alarm->data->alarm->has_lopc_miss = BAL_GRPC_PRES;
      balIndCfg->terminal_alarm->data->alarm->lopc_miss = sub_term_alarm->data.alarm.lopc_miss;
      balIndCfg->terminal_alarm->data->alarm->has_lopc_mic_error = BAL_GRPC_PRES;
      balIndCfg->terminal_alarm->data->alarm->lopc_mic_error = sub_term_alarm->data.alarm.lopc_mic_error;

      list_node *bal_indication_node = malloc(sizeof(list_node));
      bal_indication_node->bal_indication = balIndCfg;

      pthread_mutex_lock(&bal_ind_queue_lock);
      add_bal_indication_node(bal_indication_node);
      pthread_mutex_unlock(&bal_ind_queue_lock);
   }

   return result;
}

/********************************************************************\
 * Function    : bal_sub_term_dgi_indication_cb                     *
 * Description : This function will handle the indications for      *
 *               Subscriber term dgi indication                     *
 *                                                                  *
 ********************************************************************/
bcmos_errno bal_sub_term_dgi_indication_cb(bcmbal_obj *obj)
{
   bcmos_errno result = BCM_ERR_OK;

   if(BCMBAL_OBJ_ID_SUBSCRIBER_TERMINAL != obj->obj_type ||
      bcmbal_subscriber_terminal_auto_id_dgi != obj->subgroup)
   {
      ASFVOLT_LOG(ASFVOLT_ERROR, "Processing BAL API '%s' IND callback (status is %s)",
				  bcmbal_objtype_str(obj->obj_type), bcmos_strerror(obj->status));
      result = BCM_ERR_INTERNAL;
   }
   else
   {
      BalIndications *balIndCfg;
      balIndCfg = malloc(sizeof(BalIndications));
      memset(balIndCfg, 0, sizeof(BalIndications));
      bal_indications__init(balIndCfg);
      balIndCfg->u_case = BAL_INDICATIONS__U_TERMINAL_DGI;
      balIndCfg->has_objtype = BAL_GRPC_PRES;
      balIndCfg->objtype = obj->obj_type;
      balIndCfg->has_sub_group = BAL_GRPC_PRES;
      balIndCfg->sub_group = obj->subgroup;
      balIndCfg->device_id = voltha_device_id;

      bcmbal_subscriber_terminal_dgi *sub_dgi_ind =
                           (bcmbal_subscriber_terminal_dgi *)obj;

      BalSubscriberTerminalDgi *subDgiInd;
      subDgiInd = malloc(sizeof(BalSubscriberTerminalDgi));
      memset(subDgiInd, 0, sizeof(BalSubscriberTerminalDgi));
      bal_subscriber_terminal_dgi__init(subDgiInd);
      balIndCfg->terminal_dgi= subDgiInd;

	  /*'hdr' field is not parsed by voltha adapter, hence not filled */

      BalSubscriberTerminalKey *subkey;
      subkey = malloc(sizeof(BalSubscriberTerminalKey));
      memset(subkey, 0, sizeof(BalSubscriberTerminalKey));
      bal_subscriber_terminal_key__init(subkey);
      balIndCfg->terminal_dgi->key = subkey;

      balIndCfg->terminal_dgi->key->has_intf_id = BAL_GRPC_PRES;
      balIndCfg->terminal_dgi->key->intf_id = sub_dgi_ind->key.intf_id;
      balIndCfg->terminal_dgi->key->has_sub_term_id = BAL_GRPC_PRES;
      balIndCfg->terminal_dgi->key->sub_term_id = sub_dgi_ind->key.sub_term_id;

      BalSubscriberTerminalDgiData *subDgiIndData;
      subDgiIndData = malloc(sizeof(BalSubscriberTerminalDgiData));
      memset(subDgiIndData, 0, sizeof(BalSubscriberTerminalDgiData));
      bal_subscriber_terminal_dgi_data__init(subDgiIndData);
      balIndCfg->terminal_dgi->data = subDgiIndData;

      balIndCfg->terminal_dgi->data->has_dgi_status = BAL_GRPC_PRES;
      balIndCfg->terminal_dgi->data->dgi_status = sub_dgi_ind->data.dgi_status;

      list_node *bal_indication_node = malloc(sizeof(list_node));
      bal_indication_node->bal_indication = balIndCfg;

      pthread_mutex_lock(&bal_ind_queue_lock);
      add_bal_indication_node(bal_indication_node);
      pthread_mutex_unlock(&bal_ind_queue_lock);
   }

   return result;
}

/********************************************************************\
 * Function    : bal_sub_term_dowi_indication_cb                    *
 * Description : This function will handle dowi indication          *
 *                                                                  *
 ********************************************************************/
bcmos_errno bal_sub_term_dowi_indication_cb(bcmbal_obj *obj)
{
bcmos_errno result = BCM_ERR_OK;

   if(BCMBAL_OBJ_ID_SUBSCRIBER_TERMINAL != obj->obj_type ||
      bcmbal_subscriber_terminal_auto_id_dowi != obj->subgroup)
   {
      ASFVOLT_LOG(ASFVOLT_ERROR, "Processing BAL API '%s' IND callback (status is %s)",
				  bcmbal_objtype_str(obj->obj_type), bcmos_strerror(obj->status));
      result = BCM_ERR_INTERNAL;
   }
   else
   {
      BalIndications *balIndCfg;
      balIndCfg = malloc(sizeof(BalIndications));
      memset(balIndCfg, 0, sizeof(BalIndications));
      bal_indications__init(balIndCfg);
      balIndCfg->u_case = BAL_INDICATIONS__U_TERMINAL_DOWI;
      balIndCfg->has_objtype = BAL_GRPC_PRES;
      balIndCfg->objtype = obj->obj_type;
      balIndCfg->has_sub_group = BAL_GRPC_PRES;
      balIndCfg->sub_group = obj->subgroup;
      balIndCfg->device_id = voltha_device_id;

      bcmbal_subscriber_terminal_dowi *sub_dowi_ind =
                           (bcmbal_subscriber_terminal_dowi *)obj;

      BalSubscriberTerminalDowi *subDowiInd;
      subDowiInd = malloc(sizeof(BalSubscriberTerminalDowi));
      memset(subDowiInd, 0, sizeof(BalSubscriberTerminalDowi));
      bal_subscriber_terminal_dowi__init(subDowiInd);
      balIndCfg->terminal_dowi= subDowiInd;

	  /*'hdr' field is not parsed by voltha adapter, hence not filled */

      BalSubscriberTerminalKey *subkey;
      subkey = malloc(sizeof(BalSubscriberTerminalKey));
      memset(subkey, 0, sizeof(BalSubscriberTerminalKey));
      bal_subscriber_terminal_key__init(subkey);
      balIndCfg->terminal_dowi->key = subkey;

      balIndCfg->terminal_dowi->key->has_intf_id = BAL_GRPC_PRES;
      balIndCfg->terminal_dowi->key->intf_id = sub_dowi_ind->key.intf_id;
      balIndCfg->terminal_dowi->key->has_sub_term_id = BAL_GRPC_PRES;
      balIndCfg->terminal_dowi->key->sub_term_id = sub_dowi_ind->key.sub_term_id;

      BalSubscriberTerminalDowiData *subDowiIndData;
      subDowiIndData = malloc(sizeof(BalSubscriberTerminalDowiData));
      memset(subDowiIndData, 0, sizeof(BalSubscriberTerminalDowiData));
      bal_subscriber_terminal_dowi_data__init(subDowiIndData);
      balIndCfg->terminal_dowi->data = subDowiIndData;

      balIndCfg->terminal_dowi->data->has_dowi_status = BAL_GRPC_PRES;
      balIndCfg->terminal_dowi->data->dowi_status = sub_dowi_ind->data.dowi_status;

      balIndCfg->terminal_dowi->data->has_drift_value = BAL_GRPC_PRES;
      balIndCfg->terminal_dowi->data->drift_value = sub_dowi_ind->data.drift_value;

       balIndCfg->terminal_dowi->data->has_new_eqd = BAL_GRPC_PRES;
       balIndCfg->terminal_dowi->data->new_eqd = sub_dowi_ind->data.new_eqd;

      list_node *bal_indication_node = malloc(sizeof(list_node));
      bal_indication_node->bal_indication = balIndCfg;

      pthread_mutex_lock(&bal_ind_queue_lock);
      add_bal_indication_node(bal_indication_node);
      pthread_mutex_unlock(&bal_ind_queue_lock);
   }
   return result;
}

/********************************************************************\
 * Function    : bal_sub_term_looci_indication_cb                   *
 * Description : This function will handle looci indication         *
 *                                                                  *
 ********************************************************************/
bcmos_errno bal_sub_term_looci_indication_cb(bcmbal_obj *obj)
{
bcmos_errno result = BCM_ERR_OK;

   if(BCMBAL_OBJ_ID_SUBSCRIBER_TERMINAL != obj->obj_type ||
      bcmbal_subscriber_terminal_auto_id_looci != obj->subgroup)
   {
      ASFVOLT_LOG(ASFVOLT_ERROR, "Processing BAL API '%s' IND callback (status is %s)",
				  bcmbal_objtype_str(obj->obj_type), bcmos_strerror(obj->status));
      result = BCM_ERR_INTERNAL;
   }
   else
   {
      BalIndications *balIndCfg;
      balIndCfg = malloc(sizeof(BalIndications));
      memset(balIndCfg, 0, sizeof(BalIndications));
      bal_indications__init(balIndCfg);
      balIndCfg->u_case = BAL_INDICATIONS__U_TERMINAL_LOOCI;
      balIndCfg->has_objtype = BAL_GRPC_PRES;
      balIndCfg->objtype = obj->obj_type;
      balIndCfg->has_sub_group = BAL_GRPC_PRES;
      balIndCfg->sub_group = obj->subgroup;
      balIndCfg->device_id = voltha_device_id;

      bcmbal_subscriber_terminal_looci *sub_looci_ind =
                           (bcmbal_subscriber_terminal_looci *)obj;

      BalSubscriberTerminalLooci *subLoociInd;
      subLoociInd = malloc(sizeof(BalSubscriberTerminalLooci));
      memset(subLoociInd, 0, sizeof(BalSubscriberTerminalLooci));
      bal_subscriber_terminal_looci__init(subLoociInd);
      balIndCfg->terminal_looci= subLoociInd;

	  /*'hdr' field is not parsed by voltha adapter, hence not filled */

      BalSubscriberTerminalKey *subkey;
      subkey = malloc(sizeof(BalSubscriberTerminalKey));
      memset(subkey, 0, sizeof(BalSubscriberTerminalKey));
      bal_subscriber_terminal_key__init(subkey);
      balIndCfg->terminal_looci->key = subkey;

      balIndCfg->terminal_looci->key->has_intf_id = BAL_GRPC_PRES;
      balIndCfg->terminal_looci->key->intf_id = sub_looci_ind->key.intf_id;
      balIndCfg->terminal_looci->key->has_sub_term_id = BAL_GRPC_PRES;
      balIndCfg->terminal_looci->key->sub_term_id = sub_looci_ind->key.sub_term_id;

      BalSubscriberTerminalLoociData *subLoociIndData;
      subLoociIndData = malloc(sizeof(BalSubscriberTerminalLoociData));
      memset(subLoociIndData, 0, sizeof(BalSubscriberTerminalLoociData));
      bal_subscriber_terminal_looci_data__init(subLoociIndData);
      balIndCfg->terminal_looci->data = subLoociIndData;

      balIndCfg->terminal_looci->data->has_looci_status = BAL_GRPC_PRES;
      balIndCfg->terminal_looci->data->looci_status = sub_looci_ind->data.looci_status;

      list_node *bal_indication_node = malloc(sizeof(list_node));
      bal_indication_node->bal_indication = balIndCfg;

      pthread_mutex_lock(&bal_ind_queue_lock);
      add_bal_indication_node(bal_indication_node);
      pthread_mutex_unlock(&bal_ind_queue_lock);
   }
   return result;
}

/********************************************************************\
 * Function    : bal_sub_term_processing_error_indication_cb        *
 * Description : This function will handle sub term processing error*
 *                                                                  *
 ********************************************************************/
bcmos_errno bal_sub_term_processing_error_indication_cb(bcmbal_obj *obj)
{
bcmos_errno result = BCM_ERR_OK;

   if(BCMBAL_OBJ_ID_SUBSCRIBER_TERMINAL != obj->obj_type ||
      bcmbal_subscriber_terminal_auto_id_processing_error != obj->subgroup)
   {
      ASFVOLT_LOG(ASFVOLT_ERROR, "Processing BAL API '%s' IND callback (status is %s)",
				  bcmbal_objtype_str(obj->obj_type), bcmos_strerror(obj->status));
      result = BCM_ERR_INTERNAL;
   }
   else
   {
      BalIndications *balIndCfg;
      balIndCfg = malloc(sizeof(BalIndications));
      memset(balIndCfg, 0, sizeof(BalIndications));
      bal_indications__init(balIndCfg);
      balIndCfg->u_case = BAL_INDICATIONS__U_TERMINAL_PROC_ERR;
      balIndCfg->has_objtype = BAL_GRPC_PRES;
      balIndCfg->objtype = obj->obj_type;
      balIndCfg->has_sub_group = BAL_GRPC_PRES;
      balIndCfg->sub_group = obj->subgroup;
      balIndCfg->device_id = voltha_device_id;

      bcmbal_subscriber_terminal_processing_error *sub_proc_err_ind =
                           (bcmbal_subscriber_terminal_processing_error *)obj;

      BalSubscriberTerminalProcessingError *subProcErrInd;
      subProcErrInd = malloc(sizeof(BalSubscriberTerminalProcessingError));
      memset(subProcErrInd, 0, sizeof(BalSubscriberTerminalProcessingError));
      bal_subscriber_terminal_processing_error__init(subProcErrInd);
      balIndCfg->terminal_proc_err= subProcErrInd;

	  /*'hdr' field is not parsed by voltha adapter, hence not filled */

      BalSubscriberTerminalKey *subkey;
      subkey = malloc(sizeof(BalSubscriberTerminalKey));
      memset(subkey, 0, sizeof(BalSubscriberTerminalKey));
      bal_subscriber_terminal_key__init(subkey);
      balIndCfg->terminal_proc_err->key = subkey;

      balIndCfg->terminal_proc_err->key->has_intf_id = BAL_GRPC_PRES;
      balIndCfg->terminal_proc_err->key->intf_id = sub_proc_err_ind->key.intf_id;
      balIndCfg->terminal_proc_err->key->has_sub_term_id = BAL_GRPC_PRES;
      balIndCfg->terminal_proc_err->key->sub_term_id = sub_proc_err_ind->key.sub_term_id;

      list_node *bal_indication_node = malloc(sizeof(list_node));
      bal_indication_node->bal_indication = balIndCfg;

      pthread_mutex_lock(&bal_ind_queue_lock);
      add_bal_indication_node(bal_indication_node);
      pthread_mutex_unlock(&bal_ind_queue_lock);
   }
   return result;
}

/********************************************************************\
 * Function    : bal_sub_term_sdi_indication_cb                     *
 * Description : This function will handle sdi indication           *
 *                                                                  *
 ********************************************************************/
bcmos_errno bal_sub_term_sdi_indication_cb(bcmbal_obj *obj)
{
bcmos_errno result = BCM_ERR_OK;

   if(BCMBAL_OBJ_ID_SUBSCRIBER_TERMINAL != obj->obj_type ||
      bcmbal_subscriber_terminal_auto_id_sdi != obj->subgroup)
   {
      ASFVOLT_LOG(ASFVOLT_ERROR, "Processing BAL API '%s' IND callback (status is %s)",
				  bcmbal_objtype_str(obj->obj_type), bcmos_strerror(obj->status));
      result = BCM_ERR_INTERNAL;
   }
   else
   {
      BalIndications *balIndCfg;
      balIndCfg = malloc(sizeof(BalIndications));
      memset(balIndCfg, 0, sizeof(BalIndications));
      bal_indications__init(balIndCfg);
      balIndCfg->u_case = BAL_INDICATIONS__U_TERMINAL_SDI;
      balIndCfg->has_objtype = BAL_GRPC_PRES;
      balIndCfg->objtype = obj->obj_type;
      balIndCfg->has_sub_group = BAL_GRPC_PRES;
      balIndCfg->sub_group = obj->subgroup;
      balIndCfg->device_id = voltha_device_id;

      bcmbal_subscriber_terminal_sdi *sub_sdi_ind =
                           (bcmbal_subscriber_terminal_sdi *)obj;

      BalSubscriberTerminalSdi *subSdiInd;
      subSdiInd = malloc(sizeof(BalSubscriberTerminalSdi));
      memset(subSdiInd, 0, sizeof(BalSubscriberTerminalSdi));
      bal_subscriber_terminal_sdi__init(subSdiInd);
      balIndCfg->terminal_sdi= subSdiInd;

	  /*'hdr' field is not parsed by voltha adapter, hence not filled */

      BalSubscriberTerminalKey *subkey;
      subkey = malloc(sizeof(BalSubscriberTerminalKey));
      memset(subkey, 0, sizeof(BalSubscriberTerminalKey));
      bal_subscriber_terminal_key__init(subkey);
      balIndCfg->terminal_sdi->key = subkey;

      balIndCfg->terminal_sdi->key->has_intf_id = BAL_GRPC_PRES;
      balIndCfg->terminal_sdi->key->intf_id = sub_sdi_ind->key.intf_id;
      balIndCfg->terminal_sdi->key->has_sub_term_id = BAL_GRPC_PRES;
      balIndCfg->terminal_sdi->key->sub_term_id = sub_sdi_ind->key.sub_term_id;

      BalSubscriberTerminalSdiData *subSdiIndData;
      subSdiIndData = malloc(sizeof(BalSubscriberTerminalSdiData));
      memset(subSdiIndData, 0, sizeof(BalSubscriberTerminalSdiData));
      bal_subscriber_terminal_sdi_data__init(subSdiIndData);
      balIndCfg->terminal_sdi->data = subSdiIndData;

      balIndCfg->terminal_sdi->data->has_sdi_status = BAL_GRPC_PRES;
      balIndCfg->terminal_sdi->data->sdi_status = sub_sdi_ind->data.sdi_status;

      balIndCfg->terminal_sdi->data->has_ber = BAL_GRPC_PRES;
      balIndCfg->terminal_sdi->data->ber = sub_sdi_ind->data.ber;

      list_node *bal_indication_node = malloc(sizeof(list_node));
      bal_indication_node->bal_indication = balIndCfg;

      pthread_mutex_lock(&bal_ind_queue_lock);
      add_bal_indication_node(bal_indication_node);
      pthread_mutex_unlock(&bal_ind_queue_lock);
   }
   return result;
}

/********************************************************************\
 * Function    : bal_sub_term_sfi_indication_cb                     *
 * Description : This function will handle sfi indication           *
 *                                                                  *
 ********************************************************************/
bcmos_errno bal_sub_term_sfi_indication_cb(bcmbal_obj *obj)
{
bcmos_errno result = BCM_ERR_OK;

   if(BCMBAL_OBJ_ID_SUBSCRIBER_TERMINAL != obj->obj_type ||
      bcmbal_subscriber_terminal_auto_id_sfi != obj->subgroup)
   {
      ASFVOLT_LOG(ASFVOLT_ERROR, "Processing BAL API '%s' IND callback (status is %s)",
				  bcmbal_objtype_str(obj->obj_type), bcmos_strerror(obj->status));
      result = BCM_ERR_INTERNAL;
   }
   else
   {
      BalIndications *balIndCfg;
      balIndCfg = malloc(sizeof(BalIndications));
      memset(balIndCfg, 0, sizeof(BalIndications));
      bal_indications__init(balIndCfg);
      balIndCfg->u_case = BAL_INDICATIONS__U_TERMINAL_SFI;
      balIndCfg->has_objtype = BAL_GRPC_PRES;
      balIndCfg->objtype = obj->obj_type;
      balIndCfg->has_sub_group = BAL_GRPC_PRES;
      balIndCfg->sub_group = obj->subgroup;
      balIndCfg->device_id = voltha_device_id;

      bcmbal_subscriber_terminal_sfi *sub_sfi_ind =
                           (bcmbal_subscriber_terminal_sfi *)obj;

      BalSubscriberTerminalSfi *subSfiInd;
      subSfiInd = malloc(sizeof(BalSubscriberTerminalSfi));
      memset(subSfiInd, 0, sizeof(BalSubscriberTerminalSfi));
      bal_subscriber_terminal_sfi__init(subSfiInd);
      balIndCfg->terminal_sfi= subSfiInd;

	  /*'hdr' field is not parsed by voltha adapter, hence not filled */

      BalSubscriberTerminalKey *subkey;
      subkey = malloc(sizeof(BalSubscriberTerminalKey));
      memset(subkey, 0, sizeof(BalSubscriberTerminalKey));
      bal_subscriber_terminal_key__init(subkey);
      balIndCfg->terminal_sfi->key = subkey;

      balIndCfg->terminal_sfi->key->has_intf_id = BAL_GRPC_PRES;
      balIndCfg->terminal_sfi->key->intf_id = sub_sfi_ind->key.intf_id;
      balIndCfg->terminal_sfi->key->has_sub_term_id = BAL_GRPC_PRES;
      balIndCfg->terminal_sfi->key->sub_term_id = sub_sfi_ind->key.sub_term_id;

      BalSubscriberTerminalSfiData *subSfiIndData;
      subSfiIndData = malloc(sizeof(BalSubscriberTerminalSfiData));
      memset(subSfiIndData, 0, sizeof(BalSubscriberTerminalSfiData));
      bal_subscriber_terminal_sfi_data__init(subSfiIndData);
      balIndCfg->terminal_sfi->data = subSfiIndData;

      balIndCfg->terminal_sfi->data->has_sfi_status = BAL_GRPC_PRES;
      balIndCfg->terminal_sfi->data->sfi_status = sub_sfi_ind->data.sfi_status;

      balIndCfg->terminal_sfi->data->has_ber = BAL_GRPC_PRES;
      balIndCfg->terminal_sfi->data->ber = sub_sfi_ind->data.ber;

      list_node *bal_indication_node = malloc(sizeof(list_node));
      bal_indication_node->bal_indication = balIndCfg;

      pthread_mutex_lock(&bal_ind_queue_lock);
      add_bal_indication_node(bal_indication_node);
      pthread_mutex_unlock(&bal_ind_queue_lock);
   }
   return result;
}

/********************************************************************\
 * Function    : bal_sub_term_act_fail_indication_cb                *
 * Description : This function will handle sub term activation fail *
 *               indication                                         *
 *                                                                  *
 ********************************************************************/
bcmos_errno bal_sub_term_act_fail_indication_cb(bcmbal_obj *obj)
{
bcmos_errno result = BCM_ERR_OK;

   if(BCMBAL_OBJ_ID_SUBSCRIBER_TERMINAL != obj->obj_type ||
      bcmbal_subscriber_terminal_auto_id_sub_term_act_fail != obj->subgroup)
   {
      ASFVOLT_LOG(ASFVOLT_ERROR, "Processing BAL API '%s' IND callback (status is %s)",
				  bcmbal_objtype_str(obj->obj_type), bcmos_strerror(obj->status));
      result = BCM_ERR_INTERNAL;
   }
   else
   {
      BalIndications *balIndCfg;
      balIndCfg = malloc(sizeof(BalIndications));
      memset(balIndCfg, 0, sizeof(BalIndications));
      bal_indications__init(balIndCfg);
      balIndCfg->u_case = BAL_INDICATIONS__U_TERMINAL_SUB_TERM_ACT_FAIL;
      balIndCfg->has_objtype = BAL_GRPC_PRES;
      balIndCfg->objtype = obj->obj_type;
      balIndCfg->has_sub_group = BAL_GRPC_PRES;
      balIndCfg->sub_group = obj->subgroup;
      balIndCfg->device_id = voltha_device_id;

      bcmbal_subscriber_terminal_sub_term_act_fail *sub_term_act_fail_ind =
                           (bcmbal_subscriber_terminal_sub_term_act_fail *)obj;

      BalSubscriberTerminalSubTermActFail *subTermActFailInd;
      subTermActFailInd = malloc(sizeof(BalSubscriberTerminalSubTermActFail));
      memset(subTermActFailInd, 0, sizeof(BalSubscriberTerminalSubTermActFail));
      bal_subscriber_terminal_sub_term_act_fail__init(subTermActFailInd);
      balIndCfg->terminal_sub_term_act_fail= subTermActFailInd;

	  /*'hdr' field is not parsed by voltha adapter, hence not filled */

      BalSubscriberTerminalKey *subkey;
      subkey = malloc(sizeof(BalSubscriberTerminalKey));
      memset(subkey, 0, sizeof(BalSubscriberTerminalKey));
      bal_subscriber_terminal_key__init(subkey);
      balIndCfg->terminal_sub_term_act_fail->key = subkey;

      balIndCfg->terminal_sub_term_act_fail->key->has_intf_id = BAL_GRPC_PRES;
      balIndCfg->terminal_sub_term_act_fail->key->intf_id = sub_term_act_fail_ind->key.intf_id;
      balIndCfg->terminal_sub_term_act_fail->key->has_sub_term_id = BAL_GRPC_PRES;
      balIndCfg->terminal_sub_term_act_fail->key->sub_term_id = sub_term_act_fail_ind->key.sub_term_id;

      BalSubscriberTerminalSubTermActFailData *subTermActFailIndData;
      subTermActFailIndData = malloc(sizeof(BalSubscriberTerminalSubTermActFailData));
      memset(subTermActFailIndData, 0, sizeof(BalSubscriberTerminalSubTermActFailData));
      bal_subscriber_terminal_sub_term_act_fail_data__init(subTermActFailIndData);
      balIndCfg->terminal_sub_term_act_fail->data = subTermActFailIndData;

      balIndCfg->terminal_sub_term_act_fail->data->has_fail_reason = BAL_GRPC_PRES;
      balIndCfg->terminal_sub_term_act_fail->data->fail_reason = sub_term_act_fail_ind->data.fail_reason;

      list_node *bal_indication_node = malloc(sizeof(list_node));
      bal_indication_node->bal_indication = balIndCfg;

      pthread_mutex_lock(&bal_ind_queue_lock);
      add_bal_indication_node(bal_indication_node);
      pthread_mutex_unlock(&bal_ind_queue_lock);
   }
   return result;
}

/********************************************************************\
 * Function    : bal_sub_term_sufi_indication_cb                    *
 * Description : This function will handle sufi indication          *
 *                                                                  *
 ********************************************************************/
bcmos_errno bal_sub_term_sufi_indication_cb(bcmbal_obj *obj)
{
   bcmos_errno result = BCM_ERR_OK;
   if(BCMBAL_OBJ_ID_SUBSCRIBER_TERMINAL != obj->obj_type ||
      bcmbal_subscriber_terminal_auto_id_sufi != obj->subgroup)
   {
      ASFVOLT_LOG(ASFVOLT_ERROR, "Processing BAL API '%s' IND callback (status is %s)",
				  bcmbal_objtype_str(obj->obj_type), bcmos_strerror(obj->status));
      result = BCM_ERR_INTERNAL;
   }
   else
   {
      BalIndications *balIndCfg;
      balIndCfg = malloc(sizeof(BalIndications));
      memset(balIndCfg, 0, sizeof(BalIndications));
      bal_indications__init(balIndCfg);
      balIndCfg->u_case = BAL_INDICATIONS__U_TERMINAL_SUFI;
      balIndCfg->has_objtype = BAL_GRPC_PRES;
      balIndCfg->objtype = obj->obj_type;
      balIndCfg->has_sub_group = BAL_GRPC_PRES;
      balIndCfg->sub_group = obj->subgroup;
      balIndCfg->device_id = voltha_device_id;

      bcmbal_subscriber_terminal_sufi *sub_sufi_ind =
                           (bcmbal_subscriber_terminal_sufi *)obj;

      BalSubscriberTerminalSufi *subSufiInd;
      subSufiInd = malloc(sizeof(BalSubscriberTerminalSufi));
      memset(subSufiInd, 0, sizeof(BalSubscriberTerminalSufi));
      bal_subscriber_terminal_sufi__init(subSufiInd);
      balIndCfg->terminal_sufi= subSufiInd;

	  /*'hdr' field is not parsed by voltha adapter, hence not filled */

      BalSubscriberTerminalKey *subkey;
      subkey = malloc(sizeof(BalSubscriberTerminalKey));
      memset(subkey, 0, sizeof(BalSubscriberTerminalKey));
      bal_subscriber_terminal_key__init(subkey);
      balIndCfg->terminal_sufi->key = subkey;

      balIndCfg->terminal_sufi->key->has_intf_id = BAL_GRPC_PRES;
      balIndCfg->terminal_sufi->key->intf_id = sub_sufi_ind->key.intf_id;
      balIndCfg->terminal_sufi->key->has_sub_term_id = BAL_GRPC_PRES;
      balIndCfg->terminal_sufi->key->sub_term_id = sub_sufi_ind->key.sub_term_id;

      BalSubscriberTerminalSufiData *subSufiIndData;
      subSufiIndData = malloc(sizeof(BalSubscriberTerminalSufiData));
      memset(subSufiIndData, 0, sizeof(BalSubscriberTerminalSufiData));
      bal_subscriber_terminal_sufi_data__init(subSufiIndData);
      balIndCfg->terminal_sufi->data = subSufiIndData;

      balIndCfg->terminal_sufi->data->has_sufi_status = BAL_GRPC_PRES;
      balIndCfg->terminal_sufi->data->sufi_status = sub_sufi_ind->data.sufi_status;

      list_node *bal_indication_node = malloc(sizeof(list_node));
      bal_indication_node->bal_indication = balIndCfg;

      pthread_mutex_lock(&bal_ind_queue_lock);
      add_bal_indication_node(bal_indication_node);
      pthread_mutex_unlock(&bal_ind_queue_lock);
   }

   return result;
}

/********************************************************************\
 * Function    : bal_sub_term_tiwi_indication_cb                    *
 * Description : This function will handle tiwi indication          *
 *                                                                  *
 ********************************************************************/
bcmos_errno bal_sub_term_tiwi_indication_cb(bcmbal_obj *obj)
{
   bcmos_errno result = BCM_ERR_OK;
   if(BCMBAL_OBJ_ID_SUBSCRIBER_TERMINAL != obj->obj_type ||
      BCMBAL_SUBSCRIBER_TERMINAL_AUTO_ID_TIWI != obj->subgroup)
   {
      ASFVOLT_LOG(ASFVOLT_ERROR, "Processing BAL API '%s' IND callback (status is %s)",
				  bcmbal_objtype_str(obj->obj_type), bcmos_strerror(obj->status));
      result = BCM_ERR_INTERNAL;
   }
   else
   {
      BalIndications *balIndCfg;
      balIndCfg = malloc(sizeof(BalIndications));
      memset(balIndCfg, 0, sizeof(BalIndications));
      bal_indications__init(balIndCfg);
      balIndCfg->u_case = BAL_INDICATIONS__U_TERMINAL_TIWI;
      balIndCfg->has_objtype = BAL_GRPC_PRES;
      balIndCfg->objtype = obj->obj_type;
      balIndCfg->has_sub_group = BAL_GRPC_PRES;
      balIndCfg->sub_group = obj->subgroup;
      balIndCfg->device_id = voltha_device_id;

      bcmbal_subscriber_terminal_tiwi *sub_term_tiwi =
                                 (bcmbal_subscriber_terminal_tiwi *)obj;

      BalSubscriberTerminalTiwi *subTermTiwi;
      subTermTiwi = malloc(sizeof(BalSubscriberTerminalTiwi));
      memset(subTermTiwi, 0, sizeof(BalSubscriberTerminalTiwi));
      bal_subscriber_terminal_tiwi__init(subTermTiwi);
      balIndCfg->terminal_tiwi = subTermTiwi;

	  /*'hdr' field is not parsed by voltha adapter, hence not filled */

      BalSubscriberTerminalKey *sub_term_key;
      sub_term_key = malloc(sizeof(BalSubscriberTerminalKey));
      memset(sub_term_key, 0, sizeof(BalSubscriberTerminalKey));
      bal_subscriber_terminal_key__init(sub_term_key);
      balIndCfg->terminal_tiwi->key = sub_term_key;

      balIndCfg->terminal_tiwi->key->has_sub_term_id = BAL_GRPC_PRES;
      balIndCfg->terminal_tiwi->key->sub_term_id = sub_term_tiwi->key.sub_term_id;
      balIndCfg->terminal_tiwi->key->has_intf_id = BAL_GRPC_PRES;
      balIndCfg->terminal_tiwi->key->intf_id = sub_term_tiwi->key.intf_id;

      BalSubscriberTerminalTiwiData *sub_term_tiwi_data;
      sub_term_tiwi_data = malloc(sizeof(BalSubscriberTerminalTiwiData));
      memset(sub_term_tiwi_data, 0, sizeof(BalSubscriberTerminalTiwiData));
      bal_subscriber_terminal_tiwi_data__init(sub_term_tiwi_data);
      balIndCfg->terminal_tiwi->data = sub_term_tiwi_data;

      balIndCfg->terminal_tiwi->data->has_tiwi_status = BAL_GRPC_PRES;
      balIndCfg->terminal_tiwi->data->tiwi_status = sub_term_tiwi->data.tiwi_status;

      balIndCfg->terminal_tiwi->data->has_drift_value = BAL_GRPC_PRES;
      balIndCfg->terminal_tiwi->data->drift_value = sub_term_tiwi->data.drift_value;

      list_node *bal_indication_node = malloc(sizeof(list_node));
      bal_indication_node->bal_indication = balIndCfg;

      pthread_mutex_lock(&bal_ind_queue_lock);
      add_bal_indication_node(bal_indication_node);
      pthread_mutex_unlock(&bal_ind_queue_lock);
   }

   return result;
}

/********************************************************************\
 * Function    : bal_tm_sched_auto_id_oper_status_change_cb         *
 * Description : This function will handle tm sched operation status*
 *               change indication                                  *
 *                                                                  *
 ********************************************************************/
bcmos_errno bal_tm_sched_auto_id_oper_status_change_cb(bcmbal_obj *obj)
{
   bcmos_errno result = BCM_ERR_OK;

   if(BCMBAL_OBJ_ID_TM_SCHED != obj->obj_type ||
      bcmbal_tm_sched_auto_id_oper_status_change != obj->subgroup)
   {
      ASFVOLT_LOG(ASFVOLT_ERROR, "Processing BAL API '%s' IND callback (status is %s)",
				  bcmbal_objtype_str(obj->obj_type), bcmos_strerror(obj->status));
      result = BCM_ERR_INTERNAL;
   }
   else
   {
      BalIndications *balIndCfg;
      balIndCfg = malloc(sizeof(BalIndications));
      memset(balIndCfg, 0, sizeof(BalIndications));
      bal_indications__init(balIndCfg);
      balIndCfg->u_case = BAL_INDICATIONS__U_TM_SCHED_OPER_STATS_CHANGE;
      balIndCfg->has_objtype = BAL_GRPC_PRES;
      balIndCfg->objtype = obj->obj_type;
      balIndCfg->has_sub_group = BAL_GRPC_PRES;
      balIndCfg->sub_group = obj->subgroup;
      balIndCfg->device_id = voltha_device_id;

      bcmbal_tm_sched_oper_status_change *tm_sched_osc =
                                 (bcmbal_tm_sched_oper_status_change *)obj;

      BalTmSchedOperStatusChange *tmSchedOsc;
      tmSchedOsc = malloc(sizeof(BalTmSchedOperStatusChange));
      memset(tmSchedOsc, 0, sizeof(BalTmSchedOperStatusChange));
      bal_tm_sched_oper_status_change__init(tmSchedOsc);
      balIndCfg->tm_sched_oper_stats_change = tmSchedOsc;

	  /*'hdr' field is not parsed by voltha adapter, hence not filled */

      BalTmSchedKey *tmSchedKey;
      tmSchedKey = malloc(sizeof(BalTmSchedKey));
      memset(tmSchedKey, 0, sizeof(BalTmSchedKey));
      bal_tm_sched_key__init(tmSchedKey);
      balIndCfg->tm_sched_oper_stats_change->key = tmSchedKey;

      balIndCfg->tm_sched_oper_stats_change->key->has_dir = BAL_GRPC_PRES;
      balIndCfg->tm_sched_oper_stats_change->key->dir = tm_sched_osc->key.dir;
      balIndCfg->tm_sched_oper_stats_change->key->has_id = BAL_GRPC_PRES;
      balIndCfg->tm_sched_oper_stats_change->key->id = tm_sched_osc->key.id;

      BalTmSchedOperStatusChangeData *tmschedOscData;
      tmschedOscData = malloc(sizeof(BalTmSchedOperStatusChangeData));
      memset(tmschedOscData, 0, sizeof(BalTmSchedOperStatusChangeData));
      bal_tm_sched_oper_status_change_data__init(tmschedOscData);
      balIndCfg->tm_sched_oper_stats_change->data = tmschedOscData;

      balIndCfg->tm_sched_oper_stats_change->data->has_new_oper_status = BAL_GRPC_PRES;
      balIndCfg->tm_sched_oper_stats_change->data->new_oper_status =
	                                            tm_sched_osc->data.new_oper_status;
      balIndCfg->tm_sched_oper_stats_change->data->has_old_oper_status = BAL_GRPC_PRES;
      balIndCfg->tm_sched_oper_stats_change->data->old_oper_status =
	                                            tm_sched_osc->data.old_oper_status;

      list_node *bal_indication_node = malloc(sizeof(list_node));
      bal_indication_node->bal_indication = balIndCfg;

      pthread_mutex_lock(&bal_ind_queue_lock);
      add_bal_indication_node(bal_indication_node);
      pthread_mutex_unlock(&bal_ind_queue_lock);
   }

   return result;
}

/********************************************************************\
 * Function    : bal_packet_data_indication_cb                      *
 * Description : This function will handle the indications for      *
 *               Packet Data indication                             *
 *                                                                  *
 ********************************************************************/
bcmos_errno bal_packet_data_indication_cb(bcmbal_obj *obj)
{
   bcmos_errno result = BCM_ERR_OK;

   if(BCMBAL_OBJ_ID_PACKET != obj->obj_type ||
      bcmbal_packet_auto_id_bearer_channel_rx != obj->subgroup)
   {
      ASFVOLT_LOG(ASFVOLT_ERROR, "Processing BAL API '%s' IND callback (status is %s)",
				  bcmbal_objtype_str(obj->obj_type), bcmos_strerror(obj->status));
      result = BCM_ERR_INTERNAL;
   }
   else
   {
      /*bcmos_mutex_lock(&bal_ind_lock);-- Need to define bcm independent mutex*/

      BalIndications *balIndCfg;
      balIndCfg = malloc(sizeof(BalIndications));
      memset(balIndCfg, 0, sizeof(BalIndications));
      bal_indications__init(balIndCfg);
      balIndCfg->u_case = BAL_INDICATIONS__U_PKT_DATA;
      balIndCfg->has_objtype = BAL_GRPC_PRES;
      balIndCfg->objtype = obj->obj_type;
      balIndCfg->has_sub_group = BAL_GRPC_PRES;
      balIndCfg->sub_group = obj->subgroup;
      balIndCfg->device_id = voltha_device_id;

      BalPacketBearerChannelRx *rxChannel;
      rxChannel = malloc(sizeof(BalPacketBearerChannelRx));
      memset(rxChannel, 0, sizeof(BalPacketBearerChannelRx));
      bal_packet_bearer_channel_rx__init(rxChannel);
      balIndCfg->pktdata = rxChannel;

	  /*'hdr' field is not parsed by voltha adapter, hence not filled */

      BalPacketKey *packetkey;
      packetkey = malloc(sizeof(BalPacketKey));
      memset(packetkey, 0, sizeof(BalPacketKey));
      bal_packet_key__init(packetkey);
      balIndCfg->pktdata->key = packetkey;

      bcmbal_packet_bearer_channel_rx *rx_channel =
                                  (bcmbal_packet_bearer_channel_rx *)obj;

      balIndCfg->pktdata->key->has_reserved = BAL_GRPC_PRES;
      balIndCfg->pktdata->key->reserved = rx_channel->key.reserved;

      BalDest *PktSndDest;
      PktSndDest = malloc(sizeof(BalDest));
      memset(PktSndDest, 0, sizeof(BalDest));
      bal_dest__init(PktSndDest);
      balIndCfg->pktdata->key->packet_send_dest = PktSndDest;

      balIndCfg->pktdata->key->packet_send_dest->has_type = BAL_GRPC_PRES;
      balIndCfg->pktdata->key->packet_send_dest->type =
                                          rx_channel->key.packet_send_dest.type;
      switch( rx_channel->key.packet_send_dest.type)
      {
         case BCMBAL_DEST_TYPE_NNI:
         {
            balIndCfg->pktdata->key->packet_send_dest->u_case = BAL_DEST__U_NNI;
            BalDestNni *balNni;
            balNni = malloc(sizeof(BalDestNni));
            memset(balNni, 0, sizeof(BalDestNni));
            bal_dest_nni__init(balNni);
            balIndCfg->pktdata->key->packet_send_dest->nni = balNni;
            balIndCfg->pktdata->key->packet_send_dest->nni->has_intf_id = BAL_GRPC_PRES;
            balIndCfg->pktdata->key->packet_send_dest->nni->intf_id =
                                          rx_channel->key.packet_send_dest.u.nni.intf_id;
         }
         break;
         case BCMBAL_DEST_TYPE_SUB_TERM:
         {
            balIndCfg->pktdata->key->packet_send_dest->u_case = BAL_DEST__U_SUB_TERM;
            BalDestSubTerm *balSubTerm;
            balSubTerm = malloc(sizeof(BalDestSubTerm));
            memset(balSubTerm, 0, sizeof(BalDestSubTerm));
            bal_dest_sub_term__init(balSubTerm);
            balIndCfg->pktdata->key->packet_send_dest->sub_term = balSubTerm;
            balIndCfg->pktdata->key->packet_send_dest->sub_term->has_sub_term_id =
                                                                            BAL_GRPC_PRES;
            balIndCfg->pktdata->key->packet_send_dest->sub_term->sub_term_id =
                                 rx_channel->key.packet_send_dest.u.sub_term.sub_term_id;
            balIndCfg->pktdata->key->packet_send_dest->sub_term->has_intf_id =
                                                                            BAL_GRPC_PRES;
            balIndCfg->pktdata->key->packet_send_dest->sub_term->intf_id =
                                      rx_channel->key.packet_send_dest.u.sub_term.intf_id;
         }
         break;
         case BCMBAL_DEST_TYPE_SVC_PORT:
         {
            balIndCfg->pktdata->key->packet_send_dest->u_case = BAL_DEST__U_SVC_PORT;
            BalDestSvcPort *balSvcPort;
            balSvcPort = malloc(sizeof(BalDestSvcPort));
            memset(balSvcPort, 0, sizeof(BalDestSvcPort));
            bal_dest_svc_port__init(balSvcPort);
            balIndCfg->pktdata->key->packet_send_dest->svc_port = balSvcPort;
            balIndCfg->pktdata->key->packet_send_dest->svc_port->has_svc_port_id =
                                                                            BAL_GRPC_PRES;
            balIndCfg->pktdata->key->packet_send_dest->svc_port->svc_port_id =
                                 rx_channel->key.packet_send_dest.u.svc_port.svc_port_id;
            balIndCfg->pktdata->key->packet_send_dest->svc_port->has_intf_id =
                                                                            BAL_GRPC_PRES;
            balIndCfg->pktdata->key->packet_send_dest->svc_port->has_intf_id =
                                      rx_channel->key.packet_send_dest.u.svc_port.intf_id;
         }
         break;
         default:
         {
            balIndCfg->pktdata->key->packet_send_dest->u_case = BAL_DEST__U__NOT_SET;
         }
         break;
      }

      BalPacketBearerChannelRxData *pkdData;
      pkdData = malloc(sizeof(BalPacketBearerChannelRxData));
      memset(pkdData, 0, sizeof(BalPacketBearerChannelRxData));
      bal_packet_bearer_channel_rx_data__init(pkdData);
      balIndCfg->pktdata->data = pkdData;

      balIndCfg->pktdata->data->has_flow_id = BAL_GRPC_PRES;
      balIndCfg->pktdata->data->flow_id = rx_channel->data.flow_id;
      balIndCfg->pktdata->data->has_flow_type = BAL_GRPC_PRES;
      balIndCfg->pktdata->data->flow_type = rx_channel->data.flow_type;
      balIndCfg->pktdata->data->has_intf_id = BAL_GRPC_PRES;
      balIndCfg->pktdata->data->intf_id = rx_channel->data.intf_id;
      balIndCfg->pktdata->data->has_intf_type = BAL_GRPC_PRES;
      balIndCfg->pktdata->data->intf_type = rx_channel->data.intf_type;
      balIndCfg->pktdata->data->has_svc_port = BAL_GRPC_PRES;
      balIndCfg->pktdata->data->svc_port = rx_channel->data.svc_port;
      balIndCfg->pktdata->data->has_flow_cookie = BAL_GRPC_PRES;
      balIndCfg->pktdata->data->flow_cookie = rx_channel->data.flow_cookie;
      balIndCfg->pktdata->data->has_pkt = BAL_GRPC_PRES;
      balIndCfg->pktdata->data->pkt.len = rx_channel->data.pkt.len;
      balIndCfg->pktdata->data->pkt.data = (uint8_t *)malloc((balIndCfg->pktdata->data->pkt.len)*sizeof(uint8_t));
      memcpy(balIndCfg->pktdata->data->pkt.data,  rx_channel->data.pkt.val, balIndCfg->pktdata->data->pkt.len);

      /*bcmos_mutex_unlock(&bal_ind_lock);-- Need to define bcm independent mutex*/

      list_node *bal_indication_node = malloc(sizeof(list_node));
      bal_indication_node->bal_indication = balIndCfg;

      pthread_mutex_lock(&bal_ind_queue_lock);
      add_bal_indication_node(bal_indication_node);
      pthread_mutex_unlock(&bal_ind_queue_lock);
   }

   return result;
}

/********************************************************************\
 * Function    : bal_omci_data_indication_cb                        *
 * Description : This function will handle the indications for      *
 *               OMCI Data Response                                 *
 *                                                                  *
 ********************************************************************/
bcmos_errno bal_omci_data_indication_cb(bcmbal_obj *obj)
{
   bcmos_errno result = BCM_ERR_OK;

   if(BCMBAL_OBJ_ID_PACKET != obj->obj_type ||
      bcmbal_packet_auto_id_itu_omci_channel_rx != obj->subgroup)
   {
      ASFVOLT_LOG(ASFVOLT_ERROR, "Processing BAL API '%s' IND callback (status is %s)",
				  bcmbal_objtype_str(obj->obj_type), bcmos_strerror(obj->status));
      result = BCM_ERR_INTERNAL;
   }
   else
   {
      ASFVOLT_LOG(ASFVOLT_DEBUG, "Received OMCI response via BAL APIs");
      BalIndications *balIndCfg;
      balIndCfg = malloc(sizeof(BalIndications));
      memset(balIndCfg, 0, sizeof(BalIndications));
      bal_indications__init(balIndCfg);
      balIndCfg->u_case = BAL_INDICATIONS__U_BAL_OMCI_RESP;
      balIndCfg->has_objtype = BAL_GRPC_PRES;
      balIndCfg->objtype = obj->obj_type;
      balIndCfg->has_sub_group = BAL_GRPC_PRES;
      balIndCfg->sub_group = obj->subgroup;
      balIndCfg->device_id = voltha_device_id;

      BalPacketItuOmciChannelRx *omciChannel;
      omciChannel = malloc(sizeof(BalPacketItuOmciChannelRx));
      memset(omciChannel, 0, sizeof(BalPacketItuOmciChannelRx));
      bal_packet_itu_omci_channel_rx__init(omciChannel);
      balIndCfg->balomciresp = omciChannel;

	  /*'hdr' field is not parsed by voltha adapter, hence not filled */

      BalPacketKey *packetkey;
      packetkey = malloc(sizeof(BalPacketKey));
      memset(packetkey, 0, sizeof(BalPacketKey));
      bal_packet_key__init(packetkey);
      balIndCfg->balomciresp->key = packetkey;

      BalPacketItuOmciChannelRxData *omciData;
      omciData = malloc(sizeof(BalPacketItuOmciChannelRxData));
      memset(omciData, 0, sizeof(BalPacketItuOmciChannelRxData));
      bal_packet_itu_omci_channel_rx_data__init(omciData);
      balIndCfg->balomciresp->data = omciData;

      bcmbal_packet_itu_omci_channel_rx *omci_channel =
                                  (bcmbal_packet_itu_omci_channel_rx *)obj;

      balIndCfg->balomciresp->data->has_pkt =  BAL_GRPC_PRES;
      balIndCfg->balomciresp->data->pkt.len =  omci_channel->data.pkt.len;
      balIndCfg->balomciresp->data->pkt.data = (uint8_t *)malloc((omci_channel->data.pkt.len)*sizeof(uint8_t));
      memcpy(balIndCfg->balomciresp->data->pkt.data, omci_channel->data.pkt.val,
             balIndCfg->balomciresp->data->pkt.len);

      balIndCfg->balomciresp->key->has_reserved = BAL_GRPC_PRES;
      balIndCfg->balomciresp->key->reserved = omci_channel->key.reserved;

      BalDest *PktSndDest;
      PktSndDest = malloc(sizeof(BalDest));
      memset(PktSndDest, 0, sizeof(BalDest));
      bal_dest__init(PktSndDest);
      balIndCfg->balomciresp->key->packet_send_dest = PktSndDest;

      balIndCfg->balomciresp->key->packet_send_dest->has_type = BAL_GRPC_PRES;
      balIndCfg->balomciresp->key->packet_send_dest->type =
                                          omci_channel->key.packet_send_dest.type;
      switch( omci_channel->key.packet_send_dest.type)
      {
         case BCMBAL_DEST_TYPE_ITU_OMCI_CHANNEL:
         {
            balIndCfg->balomciresp->key->packet_send_dest->u_case = BAL_DEST__U_ITU_OMCI_CHANNEL;
            BalItuOmciChannel *balOmci;
            balOmci = malloc(sizeof(BalItuOmciChannel));
            memset(balOmci, 0, sizeof(BalItuOmciChannel));
            bal_itu_omci_channel__init(balOmci);
            balIndCfg->balomciresp->key->packet_send_dest->itu_omci_channel = balOmci;
            balIndCfg->balomciresp->key->packet_send_dest->itu_omci_channel->has_sub_term_id =
                                                                            BAL_GRPC_PRES;
            balIndCfg->balomciresp->key->packet_send_dest->itu_omci_channel->sub_term_id =
                            omci_channel->key.packet_send_dest.u.itu_omci_channel.sub_term_id;
            balIndCfg->balomciresp->key->packet_send_dest->itu_omci_channel->has_intf_id =
                                                                            BAL_GRPC_PRES;
            balIndCfg->balomciresp->key->packet_send_dest->itu_omci_channel->intf_id =
                                 omci_channel->key.packet_send_dest.u.itu_omci_channel.intf_id;
            ASFVOLT_LOG(ASFVOLT_DEBUG, "OMCI Response for ONU id %d",
                 balIndCfg->balomciresp->key->packet_send_dest->itu_omci_channel->sub_term_id);
         }
         break;
         default:
         {
            balIndCfg->balomciresp->key->packet_send_dest->u_case = BAL_DEST__U__NOT_SET;
         }
         break;
      }

      ASFVOLT_HEX2LOG(ASFVOLT_DEBUG, "OMCI Response with %zd bytes is ",
         balIndCfg->balomciresp->data->pkt.data, balIndCfg->balomciresp->data->pkt.len);

      ASFVOLT_LOG(ASFVOLT_DEBUG, "OMCI Response for ONU id %d",
		      balIndCfg->balomciresp->key->packet_send_dest->itu_omci_channel->sub_term_id);

      list_node *bal_indication_node = malloc(sizeof(list_node));
      bal_indication_node->bal_indication = balIndCfg;

      pthread_mutex_lock(&bal_ind_queue_lock);
      add_bal_indication_node(bal_indication_node);
      pthread_mutex_unlock(&bal_ind_queue_lock);
   }

   return result;
}

/********************************************************************\
 * Function    : bal_oam_data_indication_cb                         *
 * Description : This function will handle the indications for      *
 *               OAM Data Response                                 *
 *                                                                  *
 ********************************************************************/
bcmos_errno bal_oam_data_indication_cb(bcmbal_obj *obj)
{
   bcmos_errno result = BCM_ERR_OK;

   if(BCMBAL_OBJ_ID_PACKET != obj->obj_type ||
      bcmbal_packet_auto_id_ieee_oam_channel_rx != obj->subgroup)
   {
      ASFVOLT_LOG(ASFVOLT_ERROR, "Processing BAL API '%s' IND callback (status is %s)",
				  bcmbal_objtype_str(obj->obj_type), bcmos_strerror(obj->status));
      result = BCM_ERR_INTERNAL;
   }
   else
   {
      BalIndications *balIndCfg;
      balIndCfg = malloc(sizeof(BalIndications));
      memset(balIndCfg, 0, sizeof(BalIndications));
      bal_indications__init(balIndCfg);
      balIndCfg->u_case = BAL_INDICATIONS__U_BAL_OAM_RESP;
      balIndCfg->has_objtype = BAL_GRPC_PRES;
      balIndCfg->objtype = obj->obj_type;
      balIndCfg->has_sub_group = BAL_GRPC_PRES;
      balIndCfg->sub_group = obj->subgroup;
      balIndCfg->device_id = voltha_device_id;

      BalPacketIeeeOamChannelRx *oamChannel;
      oamChannel = malloc(sizeof(BalPacketIeeeOamChannelRx));
      memset(oamChannel, 0, sizeof(BalPacketIeeeOamChannelRx));
      bal_packet_ieee_oam_channel_rx__init(oamChannel);
      balIndCfg->baloamresp = oamChannel;

	  /*'hdr' field is not parsed by voltha adapter, hence not filled */

      BalPacketKey *packetkey;
      packetkey = malloc(sizeof(BalPacketKey));
      memset(packetkey, 0, sizeof(BalPacketKey));
      bal_packet_key__init(packetkey);
      balIndCfg->baloamresp->key = packetkey;

      bcmbal_packet_ieee_oam_channel_rx *oam_channel =
                                  (bcmbal_packet_ieee_oam_channel_rx *)obj;

      balIndCfg->baloamresp->key->has_reserved = BAL_GRPC_PRES;
      balIndCfg->baloamresp->key->reserved = oam_channel->key.reserved;

      BalDest *PktSndDest;
      PktSndDest = malloc(sizeof(BalDest));
      memset(PktSndDest, 0, sizeof(BalDest));
      bal_dest__init(PktSndDest);
      balIndCfg->baloamresp->key->packet_send_dest = PktSndDest;

      balIndCfg->baloamresp->key->packet_send_dest->has_type = BAL_GRPC_PRES;
      balIndCfg->baloamresp->key->packet_send_dest->type =
                                          oam_channel->key.packet_send_dest.type;
      switch( oam_channel->key.packet_send_dest.type)
      {
         case BCMBAL_DEST_TYPE_IEEE_OAM_CHANNEL:
         {
            balIndCfg->baloamresp->key->packet_send_dest->u_case = BAL_DEST__U_IEEE_OAM_CHANNEL;
            BalIeeeOamChannel *balOam;
            balOam = malloc(sizeof(BalIeeeOamChannel));
            memset(balOam, 0, sizeof(BalIeeeOamChannel));
            bal_ieee_oam_channel__init(balOam);
            balIndCfg->baloamresp->key->packet_send_dest->ieee_oam_channel = balOam;
            balIndCfg->baloamresp->key->packet_send_dest->ieee_oam_channel->has_intf_id =
                                                                            BAL_GRPC_PRES;
            balIndCfg->baloamresp->key->packet_send_dest->ieee_oam_channel->intf_id =
                                 oam_channel->key.packet_send_dest.u.ieee_oam_channel.intf_id;
            balIndCfg->baloamresp->key->packet_send_dest->ieee_oam_channel->has_mac_address = BAL_GRPC_PRES;
            balIndCfg->baloamresp->key->packet_send_dest->ieee_oam_channel->mac_address.len =
               (BCMOS_ETH_ALEN)*sizeof(oam_channel->key.packet_send_dest.u.ieee_oam_channel.mac_address.u8);
            balIndCfg->baloamresp->key->packet_send_dest->ieee_oam_channel->mac_address.data =
             (uint8_t *)malloc((balIndCfg->baloamresp->key->packet_send_dest->ieee_oam_channel->mac_address.len)*sizeof(uint8_t));
            memcpy(balIndCfg->baloamresp->key->packet_send_dest->ieee_oam_channel->mac_address.data,
                   oam_channel->key.packet_send_dest.u.ieee_oam_channel.mac_address.u8,
                   balIndCfg->baloamresp->key->packet_send_dest->ieee_oam_channel->mac_address.len);

         }
         break;
         default:
         {
            balIndCfg->baloamresp->key->packet_send_dest->u_case = BAL_DEST__U__NOT_SET;
         }
         break;
      }

      BalPacketIeeeOamChannelRxData *oamData;
      oamData = malloc(sizeof(BalPacketIeeeOamChannelRxData));
      memset(oamData, 0, sizeof(BalPacketIeeeOamChannelRxData));
      bal_packet_ieee_oam_channel_rx_data__init(oamData);
      balIndCfg->baloamresp->data = oamData;

      balIndCfg->baloamresp->data->pkt.len =  oam_channel->data.pkt.len;
      balIndCfg->baloamresp->data->pkt.data = (uint8_t *)malloc((balIndCfg->baloamresp->data->pkt.len)*sizeof(uint8_t));
      memcpy(balIndCfg->baloamresp->data->pkt.data, oam_channel->data.pkt.val,
             balIndCfg->baloamresp->data->pkt.len);

      list_node *bal_indication_node = malloc(sizeof(list_node));
      bal_indication_node->bal_indication = balIndCfg;

      pthread_mutex_lock(&bal_ind_queue_lock);
      add_bal_indication_node(bal_indication_node);
      pthread_mutex_unlock(&bal_ind_queue_lock);
   }

   return result;
}
