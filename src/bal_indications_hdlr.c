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

/*static bcmos_mutex bal_ind_lock; - Need to define bcm independent mutex*/
/********************************************************************\
 * Function    : bal_acc_term_indication_cb                         *
 * Description : This function will handle the indications for      *
 *               Access Terminal Indication                         *
 *                                                                  *
 ********************************************************************/
bcmos_errno bal_acc_term_indication_cb(bcmbal_obj *obj)
{
   bcmos_errno result = BCM_ERR_OK;

   if(BCMBAL_OBJ_ID_ACCESS_TERMINAL != obj->obj_type ||
      bcmbal_access_terminal_auto_id_ind != obj->subgroup)
   {
      ASFVOLT_LOG(ASFVOLT_ERROR, "Processing BAL API \'%s\' IND callback (status is %s)\n",
				  bcmbal_objtype_str(obj->obj_type), bcmos_strerror(obj->status));
      result = BCM_ERR_INTERNAL;
   }
   else
   {
      ASFVOLT_LOG(ASFVOLT_DEBUG, "Processing BAL API \'%s\' IND callback (status is %s)\n",
				  bcmbal_objtype_str(obj->obj_type), bcmos_strerror(obj->status));
      BalIndications balIndCfg;
      memset(&balIndCfg, 0, sizeof(BalIndications));
      bal_indications__init(&balIndCfg);
      balIndCfg.u_case = BAL_INDICATIONS__U_ACCESS_TERM_IND;
      balIndCfg.has_objtype = BAL_GRPC_PRES;
      balIndCfg.objtype = obj->obj_type;
      balIndCfg.has_sub_group = BAL_GRPC_PRES;
      balIndCfg.sub_group = obj->subgroup;
      balIndCfg.device_id = voltha_device_id;

      bcmbal_access_terminal_ind *acc_term_ind = (bcmbal_access_terminal_ind *)obj;

      BalAccessTerminalInd acessTermInd;
      memset(&acessTermInd, 0, sizeof(BalAccessTerminalInd));
      bal_access_terminal_ind__init(&acessTermInd);
      balIndCfg.access_term_ind = &acessTermInd;

      BalObj hdr;
      memset(&hdr, 0, sizeof(BalObj));
      bal_obj__init(&hdr);
      balIndCfg.access_term_ind->hdr = &hdr;

      BalAccessTerminalKey accessTermkey;
      memset(&accessTermkey, 0, sizeof(BalAccessTerminalKey));
      bal_access_terminal_key__init(&accessTermkey);
      balIndCfg.access_term_ind->key = &accessTermkey;

      balIndCfg.access_term_ind->key->has_access_term_id = BAL_GRPC_PRES;
      balIndCfg.access_term_ind->key->access_term_id = acc_term_ind->key.access_term_id;

      BalAccessTerminalIndData accessTermIndData;
      memset(&accessTermIndData, 0, sizeof(BalAccessTerminalIndData));
      bal_access_terminal_ind_data__init(&accessTermIndData);
      balIndCfg.access_term_ind->data = &accessTermIndData;

      balIndCfg.access_term_ind->data->has_admin_state = BAL_GRPC_PRES;
      balIndCfg.access_term_ind->data->admin_state = acc_term_ind->data.admin_state;
      balIndCfg.access_term_ind->data->has_oper_status = BAL_GRPC_PRES;
      balIndCfg.access_term_ind->data->oper_status = acc_term_ind->data.oper_status;
      balIndCfg.access_term_ind->data->has_iwf_mode = BAL_GRPC_PRES;
      balIndCfg.access_term_ind->data->iwf_mode = acc_term_ind->data.iwf_mode;

      BalTopology balTop;
      memset(&balTop, 0, sizeof(BalTopology));
      bal_topology__init(&balTop);
      balIndCfg.access_term_ind->data->topology = &balTop;

      balIndCfg.access_term_ind->data->topology->has_num_of_nni_ports = BAL_GRPC_PRES;
      balIndCfg.access_term_ind->data->topology->num_of_nni_ports =
                                            acc_term_ind->data.topology.num_of_nni_ports;
      balIndCfg.access_term_ind->data->topology->has_num_of_pon_ports = BAL_GRPC_PRES;
      balIndCfg.access_term_ind->data->topology->num_of_pon_ports =
                                            acc_term_ind->data.topology.num_of_pon_ports;
      balIndCfg.access_term_ind->data->topology->has_num_of_mac_devs = BAL_GRPC_PRES;
      balIndCfg.access_term_ind->data->topology->num_of_mac_devs =
                                             acc_term_ind->data.topology.num_of_mac_devs;
      balIndCfg.access_term_ind->data->topology->has_num_of_pons_per_mac_dev = BAL_GRPC_PRES;
      balIndCfg.access_term_ind->data->topology->num_of_pons_per_mac_dev =
                                      acc_term_ind->data.topology.num_of_pons_per_mac_dev;
      balIndCfg.access_term_ind->data->topology->has_pon_family = BAL_GRPC_PRES;
      balIndCfg.access_term_ind->data->topology->pon_family =
                                                   acc_term_ind->data.topology.pon_family;
      balIndCfg.access_term_ind->data->topology->has_pon_sub_family = BAL_GRPC_PRES;
      balIndCfg.access_term_ind->data->topology->pon_sub_family =
                                                acc_term_ind->data.topology.pon_sub_family;

      BalSwVersion balsv;
      memset(&balsv, 0, sizeof(BalSwVersion));
      bal_sw_version__init(&balsv);
      balIndCfg.access_term_ind->data->sw_version = &balsv;

      balIndCfg.access_term_ind->data->sw_version->has_version_type = BAL_GRPC_PRES;
      balIndCfg.access_term_ind->data->sw_version->version_type =
                                                acc_term_ind->data.sw_version.version_type;
      balIndCfg.access_term_ind->data->sw_version->has_major_rev = BAL_GRPC_PRES;
      balIndCfg.access_term_ind->data->sw_version->major_rev =
                                                   acc_term_ind->data.sw_version.major_rev;
      balIndCfg.access_term_ind->data->sw_version->has_minor_rev = BAL_GRPC_PRES;
      balIndCfg.access_term_ind->data->sw_version->minor_rev =
                                                   acc_term_ind->data.sw_version.minor_rev;
      balIndCfg.access_term_ind->data->sw_version->has_patch_rev = BAL_GRPC_PRES;
      balIndCfg.access_term_ind->data->sw_version->patch_rev =
                                                   acc_term_ind->data.sw_version.patch_rev;
      balIndCfg.access_term_ind->data->sw_version->has_om_version = BAL_GRPC_PRES;
      balIndCfg.access_term_ind->data->sw_version->om_version =
                                                  acc_term_ind->data.sw_version.om_version;
      balIndCfg.access_term_ind->data->sw_version->has_dev_point = BAL_GRPC_PRES;
      balIndCfg.access_term_ind->data->sw_version->dev_point =
                                                   acc_term_ind->data.sw_version.dev_point;

      bal_register_indication_cbs();

      BalErr *output;
      result = bal_ind__bal_acc_term_ind(client, NULL, &balIndCfg, &output, NULL, 0);
      is_reboot = BAL_REBOOT_STATUS__BAL_OLT_UP_AFTER_ACTIVATION;
   }

   return result;
}

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
      ASFVOLT_LOG(ASFVOLT_ERROR, "Processing BAL API \'%s\' IND callback (status is %s)\n",
				  bcmbal_objtype_str(obj->obj_type), bcmos_strerror(obj->status));
      result = BCM_ERR_INTERNAL;
   }
   else
   {
      BalIndications balIndCfg;
      memset(&balIndCfg, 0, sizeof(BalIndications));
      bal_indications__init(&balIndCfg);
      balIndCfg.has_objtype = BAL_GRPC_PRES;
      balIndCfg.objtype = obj->obj_type;
      balIndCfg.has_sub_group = BAL_GRPC_PRES;
      balIndCfg.sub_group = obj->subgroup;
      balIndCfg.u_case = BAL_INDICATIONS__U_ACCESS_TERM_IND_OP_STATE;
      balIndCfg.device_id = voltha_device_id;

      bcmbal_access_terminal_oper_status_change *acc_term_osc =
                                  (bcmbal_access_terminal_oper_status_change *)obj;

      BalAccessTerminalOperStatusChange acessTermOSC;
      memset(&acessTermOSC, 0, sizeof(BalAccessTerminalOperStatusChange));
      bal_access_terminal_oper_status_change__init(&acessTermOSC);
      balIndCfg.access_term_ind_op_state = &acessTermOSC;

      BalObj hdr;
      memset(&hdr, 0, sizeof(BalObj));
      bal_obj__init(&hdr);
      balIndCfg.access_term_ind_op_state->hdr = &hdr;

      BalAccessTerminalKey accessTermkey;
      memset(&accessTermkey, 0, sizeof(BalAccessTerminalKey));
      bal_access_terminal_key__init(&accessTermkey);
      balIndCfg.access_term_ind_op_state->key = &accessTermkey;

      balIndCfg.access_term_ind_op_state->key->has_access_term_id = BAL_GRPC_PRES;
      balIndCfg.access_term_ind_op_state->key->access_term_id =
                                                  acc_term_osc->key.access_term_id;

      BalAccessTerminalOperStatusChangeData accessTermOscData;
      memset(&accessTermOscData, 0, sizeof(BalAccessTerminalOperStatusChangeData));
      bal_access_terminal_oper_status_change_data__init(&accessTermOscData);
      balIndCfg.access_term_ind_op_state->data = &accessTermOscData;

      balIndCfg.access_term_ind_op_state->data->has_new_oper_status = BAL_GRPC_PRES;
      balIndCfg.access_term_ind_op_state->data->new_oper_status =
                                                 acc_term_osc->data.new_oper_status;
      balIndCfg.access_term_ind_op_state->data->has_old_oper_status = BAL_GRPC_PRES;
      balIndCfg.access_term_ind_op_state->data->old_oper_status =
                                                 acc_term_osc->data.old_oper_status;
      balIndCfg.access_term_ind_op_state->data->has_admin_state = BAL_GRPC_PRES;
      balIndCfg.access_term_ind_op_state->data->admin_state =
                                                 acc_term_osc->data.admin_state;

      BalErr *output;
      result = bal_ind__bal_acc_term_oper_sts_cng_ind(client, NULL, &balIndCfg, &output, NULL, 0);
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
      ASFVOLT_LOG(ASFVOLT_ERROR, "Processing BAL API \'%s\' IND callback (status is %s)\n",
				  bcmbal_objtype_str(obj->obj_type), bcmos_strerror(obj->status));
      result = BCM_ERR_INTERNAL;
   }
   else
   {
      BalIndications balIndCfg;
      memset(&balIndCfg, 0, sizeof(BalIndications));
      bal_indications__init(&balIndCfg);
      balIndCfg.u_case = BAL_INDICATIONS__U_FLOW_OP_STATE;
      balIndCfg.has_objtype = BAL_GRPC_PRES;
      balIndCfg.objtype = obj->obj_type;
      balIndCfg.has_sub_group = BAL_GRPC_PRES;
      balIndCfg.sub_group = obj->subgroup;
      balIndCfg.device_id = voltha_device_id;

      bcmbal_flow_oper_status_change *flow_osc = (bcmbal_flow_oper_status_change *)obj;

      BalFlowOperStatusChange flowOscInd;
      memset(&flowOscInd, 0, sizeof(BalFlowOperStatusChange));
      bal_flow_oper_status_change__init(&flowOscInd);
      balIndCfg.flow_op_state = &flowOscInd;

      BalObj hdr;
      memset(&hdr, 0, sizeof(BalObj));
      bal_obj__init(&hdr);
      balIndCfg.flow_op_state->hdr = &hdr;

      BalFlowKey flowkey;
      memset(&flowkey, 0, sizeof(BalFlowKey));
      bal_flow_key__init(&flowkey);
      balIndCfg.flow_op_state->key = &flowkey;

      balIndCfg.flow_op_state->key->has_flow_id = BAL_GRPC_PRES;
      balIndCfg.flow_op_state->key->flow_id = flow_osc->key.flow_id;
      balIndCfg.flow_op_state->key->has_flow_type = BAL_GRPC_PRES;
      balIndCfg.flow_op_state->key->flow_type = flow_osc->key.flow_type;

      BalFlowOperStatusChangeData flowOscIndData;
      memset(&flowOscIndData, 0, sizeof(BalFlowOperStatusChangeData));
      bal_flow_oper_status_change_data__init(&flowOscIndData);
      balIndCfg.flow_op_state->data = &flowOscIndData;

      balIndCfg.flow_op_state->data->has_new_oper_status = BAL_GRPC_PRES;
      balIndCfg.flow_op_state->data->new_oper_status = flow_osc->data.new_oper_status;
      balIndCfg.flow_op_state->data->has_old_oper_status = BAL_GRPC_PRES;
      balIndCfg.flow_op_state->data->old_oper_status = flow_osc->data.old_oper_status;
      balIndCfg.flow_op_state->data->has_admin_state = BAL_GRPC_PRES;
      balIndCfg.flow_op_state->data->admin_state = flow_osc->data.admin_state;
      balIndCfg.flow_op_state->data->has_svc_port_id = BAL_GRPC_PRES;
      balIndCfg.flow_op_state->data->svc_port_id = flow_osc->data.svc_port_id;
      balIndCfg.flow_op_state->data->has_dba_tm_sched_id = BAL_GRPC_PRES;
      balIndCfg.flow_op_state->data->dba_tm_sched_id = flow_osc->data.dba_tm_sched_id;
      balIndCfg.flow_op_state->data->has_cookie = BAL_GRPC_PRES;
      balIndCfg.flow_op_state->data->cookie = flow_osc->data.cookie;

      BalErr *output;
      result = bal_ind__bal_flow_oper_sts_cng(client, NULL, &balIndCfg, &output, NULL, 0);
   }

   return result;
}

/********************************************************************\
 * Function    : bal_flow_indication_cb                             *
 * Description : This function will handle the indications for      *
 *               Flow Indication                                    *
 *                                                                  *
 ********************************************************************/
bcmos_errno bal_flow_indication_cb(bcmbal_obj *obj)
{
   bcmos_errno result = BCM_ERR_OK;

   if(BCMBAL_OBJ_ID_FLOW != obj->obj_type ||
      bcmbal_flow_auto_id_ind != obj->subgroup)
   {
      ASFVOLT_LOG(ASFVOLT_ERROR, "Processing BAL API \'%s\' IND callback (status is %s)\n",
				  bcmbal_objtype_str(obj->obj_type), bcmos_strerror(obj->status));
      result = BCM_ERR_INTERNAL;
   }
   else
   {
      BalIndications balIndCfg;
      memset(&balIndCfg, 0, sizeof(BalIndications));
      bal_indications__init(&balIndCfg);
      balIndCfg.u_case = BAL_INDICATIONS__U_FLOW_IND;
      balIndCfg.has_objtype = BAL_GRPC_PRES;
      balIndCfg.objtype = obj->obj_type;
      balIndCfg.has_sub_group = BAL_GRPC_PRES;
      balIndCfg.sub_group = obj->subgroup;
      balIndCfg.device_id = voltha_device_id;

      bcmbal_flow_ind *flow_ind = (bcmbal_flow_ind *)obj;

      BalFlowInd flowInd;
      memset(&flowInd, 0, sizeof(BalFlowInd));
      bal_flow_ind__init(&flowInd);
      balIndCfg.flow_ind = &flowInd;

      BalObj hdr;
      memset(&hdr, 0, sizeof(BalObj));
      bal_obj__init(&hdr);
      balIndCfg.flow_ind->hdr = &hdr;

      BalFlowKey flowkey;
      memset(&flowkey, 0, sizeof(BalFlowKey));
      bal_flow_key__init(&flowkey);
      balIndCfg.flow_ind->key = &flowkey;

      balIndCfg.flow_ind->key->has_flow_id = BAL_GRPC_PRES;
      balIndCfg.flow_ind->key->flow_id = flow_ind->key.flow_id;
      balIndCfg.flow_ind->key->has_flow_type = BAL_GRPC_PRES;
      balIndCfg.flow_ind->key->flow_type = flow_ind->key.flow_type;

      BalFlowIndData flowIndData;
      memset(&flowIndData, 0, sizeof(BalFlowIndData));
      bal_flow_ind_data__init(&flowIndData);
      balIndCfg.flow_ind->data = &flowIndData;

      balIndCfg.flow_ind->data->has_admin_state = BAL_GRPC_PRES;
      balIndCfg.flow_ind->data->admin_state = flow_ind->data.admin_state;
      balIndCfg.flow_ind->data->has_oper_status= BAL_GRPC_PRES;
      balIndCfg.flow_ind->data->oper_status= flow_ind->data.oper_status;
      balIndCfg.flow_ind->data->has_access_int_id = BAL_GRPC_PRES;
      balIndCfg.flow_ind->data->access_int_id = flow_ind->data.access_int_id;
      balIndCfg.flow_ind->data->has_network_int_id = BAL_GRPC_PRES;
      balIndCfg.flow_ind->data->network_int_id = flow_ind->data.network_int_id;
      balIndCfg.flow_ind->data->has_sub_term_id = BAL_GRPC_PRES;
      balIndCfg.flow_ind->data->sub_term_id = flow_ind->data.sub_term_id;
      balIndCfg.flow_ind->data->has_sub_term_uni_idx = BAL_GRPC_PRES;
      balIndCfg.flow_ind->data->sub_term_uni_idx = flow_ind->data.sub_term_uni_idx;
      balIndCfg.flow_ind->data->has_svc_port_id = BAL_GRPC_PRES;
      balIndCfg.flow_ind->data->svc_port_id = flow_ind->data.svc_port_id;
      balIndCfg.flow_ind->data->has_resolve_mac = BAL_GRPC_PRES;
      balIndCfg.flow_ind->data->resolve_mac = flow_ind->data.resolve_mac;
      balIndCfg.flow_ind->data->has_cookie = BAL_GRPC_PRES;
      balIndCfg.flow_ind->data->cookie = flow_ind->data.cookie;

      BalClassifier balClassifier;
      memset(&balClassifier, 0, sizeof(BalClassifier));
      bal_classifier__init(&balClassifier);
      balIndCfg.flow_ind->data->classifier = &balClassifier;

      balIndCfg.flow_ind->data->classifier->has_presence_mask = BAL_GRPC_PRES;
      balIndCfg.flow_ind->data->classifier->presence_mask = flow_ind->data.classifier.presence_mask;
      balIndCfg.flow_ind->data->classifier->has_o_tpid = BAL_GRPC_PRES;
      balIndCfg.flow_ind->data->classifier->o_tpid = flow_ind->data.classifier.o_tpid;
      balIndCfg.flow_ind->data->classifier->has_o_vid = BAL_GRPC_PRES;
      balIndCfg.flow_ind->data->classifier->o_vid = flow_ind->data.classifier.o_vid;
      balIndCfg.flow_ind->data->classifier->has_i_tpid = BAL_GRPC_PRES;
      balIndCfg.flow_ind->data->classifier->i_tpid = flow_ind->data.classifier.i_tpid;
      balIndCfg.flow_ind->data->classifier->has_i_vid = BAL_GRPC_PRES;
      balIndCfg.flow_ind->data->classifier->i_vid = flow_ind->data.classifier.i_vid;
      balIndCfg.flow_ind->data->classifier->has_o_pbits = BAL_GRPC_PRES;
      balIndCfg.flow_ind->data->classifier->o_pbits = flow_ind->data.classifier.o_pbits;
      balIndCfg.flow_ind->data->classifier->has_i_pbits = BAL_GRPC_PRES;
      balIndCfg.flow_ind->data->classifier->i_pbits = flow_ind->data.classifier.i_pbits;
      balIndCfg.flow_ind->data->classifier->has_ether_type = BAL_GRPC_PRES;
      balIndCfg.flow_ind->data->classifier->ether_type = flow_ind->data.classifier.ether_type;
      balIndCfg.flow_ind->data->classifier->has_dst_mac = BAL_GRPC_PRES;
      balIndCfg.flow_ind->data->classifier->dst_mac.len =
                            (BCMOS_ETH_ALEN)*sizeof(flow_ind->data.classifier.dst_mac.u8);
      balIndCfg.flow_ind->data->classifier->dst_mac.data =
           (uint8_t *)malloc((balIndCfg.flow_ind->data->classifier->dst_mac.len)*sizeof(uint8_t));
      memcpy(balIndCfg.flow_ind->data->classifier->dst_mac.data,
             flow_ind->data.classifier.dst_mac.u8,
             balIndCfg.flow_ind->data->classifier->dst_mac.len);
      balIndCfg.flow_ind->data->classifier->has_src_mac = BAL_GRPC_PRES;
      balIndCfg.flow_ind->data->classifier->src_mac.len =
                            (BCMOS_ETH_ALEN)*sizeof(flow_ind->data.classifier.src_mac.u8);
      balIndCfg.flow_ind->data->classifier->src_mac.data =
           (uint8_t *)malloc((balIndCfg.flow_ind->data->classifier->src_mac.len)*sizeof(uint8_t));
      memcpy(balIndCfg.flow_ind->data->classifier->src_mac.data,
             flow_ind->data.classifier.src_mac.u8,
             balIndCfg.flow_ind->data->classifier->src_mac.len);
      balIndCfg.flow_ind->data->classifier->has_ip_proto = BAL_GRPC_PRES;
      balIndCfg.flow_ind->data->classifier->ip_proto = flow_ind->data.classifier.ip_proto;
      balIndCfg.flow_ind->data->classifier->has_dst_ip = BAL_GRPC_PRES;
      balIndCfg.flow_ind->data->classifier->dst_ip = flow_ind->data.classifier.dst_ip.u32;
      balIndCfg.flow_ind->data->classifier->has_src_ip = BAL_GRPC_PRES;
      balIndCfg.flow_ind->data->classifier->src_ip = flow_ind->data.classifier.src_ip.u32;
      balIndCfg.flow_ind->data->classifier->has_src_port = BAL_GRPC_PRES;
      balIndCfg.flow_ind->data->classifier->src_port = flow_ind->data.classifier.src_port;
      balIndCfg.flow_ind->data->classifier->has_dst_port = BAL_GRPC_PRES;
      balIndCfg.flow_ind->data->classifier->dst_port = flow_ind->data.classifier.dst_port;
      balIndCfg.flow_ind->data->classifier->has_pkt_tag_type = BAL_GRPC_PRES;
      balIndCfg.flow_ind->data->classifier->pkt_tag_type = flow_ind->data.classifier.pkt_tag_type;

      BalAction balAction;
      memset(&balAction, 0, sizeof(BalAction));
      bal_action__init(&balAction);
      balIndCfg.flow_ind->data->action = &balAction;

      balIndCfg.flow_ind->data->action->has_presence_mask = BAL_GRPC_PRES;
      balIndCfg.flow_ind->data->action->presence_mask = flow_ind->data.action.presence_mask;
      balIndCfg.flow_ind->data->action->has_cmds_bitmask = BAL_GRPC_PRES;
      balIndCfg.flow_ind->data->action->cmds_bitmask = flow_ind->data.action.cmds_bitmask;
      balIndCfg.flow_ind->data->action->has_o_vid = BAL_GRPC_PRES;
      balIndCfg.flow_ind->data->action->o_vid = flow_ind->data.action.o_vid;
      balIndCfg.flow_ind->data->action->has_o_pbits = BAL_GRPC_PRES;
      balIndCfg.flow_ind->data->action->o_pbits = flow_ind->data.action.o_pbits;
      balIndCfg.flow_ind->data->action->has_o_tpid = BAL_GRPC_PRES;
      balIndCfg.flow_ind->data->action->o_tpid = flow_ind->data.action.o_tpid;
      balIndCfg.flow_ind->data->action->has_i_vid = BAL_GRPC_PRES;
      balIndCfg.flow_ind->data->action->i_vid = flow_ind->data.action.i_vid;
      balIndCfg.flow_ind->data->action->has_i_pbits = BAL_GRPC_PRES;
      balIndCfg.flow_ind->data->action->i_pbits = flow_ind->data.action.i_pbits;
      balIndCfg.flow_ind->data->action->has_i_tpid = BAL_GRPC_PRES;
      balIndCfg.flow_ind->data->action->i_tpid = flow_ind->data.action.i_tpid;
      BalErr *output;
      result = bal_ind__bal_flow_ind(client, NULL, &balIndCfg, &output, NULL, 0);
   }

   return result;
}

/********************************************************************\
 * Function    : bal_group_indication_cb                            *
 * Description : This function will handle the indications for      *
 *               Group Indication                                   *
 *                                                                  *
 ********************************************************************/
bcmos_errno bal_group_indication_cb(bcmbal_obj *obj)
{
   bcmos_errno result = BCM_ERR_OK;

   if(BCMBAL_OBJ_ID_GROUP != obj->obj_type ||
      bcmbal_group_auto_id_ind != obj->subgroup)
   {
      ASFVOLT_LOG(ASFVOLT_ERROR, "Processing BAL API \'%s\' IND callback (status is %s)\n",
				  bcmbal_objtype_str(obj->obj_type), bcmos_strerror(obj->status));
      result = BCM_ERR_INTERNAL;
   }
   else
   {
      BalIndications balIndCfg;
      memset(&balIndCfg, 0, sizeof(BalIndications));
      bal_indications__init(&balIndCfg);
      balIndCfg.u_case = BAL_INDICATIONS__U_GROUP_IND;
      balIndCfg.has_objtype = BAL_GRPC_PRES;
      balIndCfg.objtype = obj->obj_type;
      balIndCfg.has_sub_group = BAL_GRPC_PRES;
      balIndCfg.sub_group = obj->subgroup;
      balIndCfg.device_id = voltha_device_id;

      bcmbal_group_ind *group_ind = (bcmbal_group_ind *)obj;

      BalGroupInd groupInd;
      memset(&groupInd, 0, sizeof(BalGroupInd));
      bal_group_ind__init(&groupInd);
      balIndCfg.group_ind = &groupInd;

      BalObj hdr;
      memset(&hdr, 0, sizeof(BalObj));
      bal_obj__init(&hdr);
      balIndCfg.group_ind->hdr = &hdr;

      BalGroupKey groupkey;
      memset(&groupkey, 0, sizeof(BalGroupKey));
      bal_group_key__init(&groupkey);
      balIndCfg.group_ind->key = &groupkey;

      balIndCfg.group_ind->key->has_group_id = BAL_GRPC_PRES;
      balIndCfg.group_ind->key->group_id = group_ind->key.group_id;

      BalGroupIndData groupIndData;
      memset(&groupIndData, 0, sizeof(BalGroupIndData));
      bal_group_ind_data__init(&groupIndData);
      balIndCfg.group_ind->data = &groupIndData;

      balIndCfg.group_ind->data->has_members_cmd = BAL_GRPC_PRES;
      balIndCfg.group_ind->data->members_cmd = group_ind->data.members_cmd;
      balIndCfg.group_ind->data->has_cookie = BAL_GRPC_PRES;
      balIndCfg.group_ind->data->cookie = group_ind->data.cookie;
      balIndCfg.group_ind->data->has_owner = BAL_GRPC_PRES;
      balIndCfg.group_ind->data->owner = group_ind->data.owner;

      BalGroupMemberInfoList balMembers;
      memset(&balMembers, 0, sizeof(BalGroupMemberInfoList));
      bal_group_member_info_list__init(&balMembers);
      balIndCfg.group_ind->data->members = &balMembers;

      balIndCfg.group_ind->data->members->n_val = group_ind->data.members.len;

      BalGroupMemberInfo *balMemberInfo;
      balMemberInfo = (BalGroupMemberInfo *)malloc(sizeof(BalGroupMemberInfo));
      memset(&balMemberInfo, 0, sizeof(BalGroupMemberInfo));
      bal_group_member_info__init(balMemberInfo);
      balIndCfg.group_ind->data->members->val = &balMemberInfo;

      balMemberInfo->has_intf_id = BAL_GRPC_PRES;
      balMemberInfo->intf_id = group_ind->data.members.val->intf_id;
      balMemberInfo->has_svc_port_id = BAL_GRPC_PRES;
      balMemberInfo->svc_port_id = group_ind->data.members.val->svc_port_id;

      BalAction balAction;
      memset(&balAction, 0, sizeof(BalAction));
      bal_action__init(&balAction);
      balMemberInfo->action = &balAction;

      balMemberInfo->action->has_presence_mask = BAL_GRPC_PRES;
      balMemberInfo->action->presence_mask = group_ind->data.members.val->action.presence_mask;
      balMemberInfo->action->has_cmds_bitmask = BAL_GRPC_PRES;
      balMemberInfo->action->cmds_bitmask = group_ind->data.members.val->action.cmds_bitmask;
      balMemberInfo->action->has_o_vid = BAL_GRPC_PRES;
      balMemberInfo->action->o_vid = group_ind->data.members.val->action.o_vid;
      balMemberInfo->action->has_o_pbits = BAL_GRPC_PRES;
      balMemberInfo->action->o_pbits = group_ind->data.members.val->action.o_pbits;
      balMemberInfo->action->has_o_tpid = BAL_GRPC_PRES;
      balMemberInfo->action->o_tpid = group_ind->data.members.val->action.o_tpid;
      balMemberInfo->action->has_i_vid = BAL_GRPC_PRES;
      balMemberInfo->action->i_vid = group_ind->data.members.val->action.i_vid;
      balMemberInfo->action->has_i_pbits = BAL_GRPC_PRES;
      balMemberInfo->action->i_pbits = group_ind->data.members.val->action.i_pbits;
      balMemberInfo->action->has_i_tpid = BAL_GRPC_PRES;
      balMemberInfo->action->i_tpid = group_ind->data.members.val->action.i_tpid;

      BalTmQueueRef balQueue;
      memset(&balQueue, 0, sizeof(BalTmQueueRef));
      bal_tm_queue_ref__init(&balQueue);
      balMemberInfo->queue = &balQueue;

      balMemberInfo->queue->has_sched_id = BAL_GRPC_PRES;
      balMemberInfo->queue->sched_id = group_ind->data.members.val->queue.sched_id;
      balMemberInfo->queue->has_queue_id = BAL_GRPC_PRES;
      balMemberInfo->queue->queue_id = group_ind->data.members.val->queue.queue_id;

      BalIdList balFlows;
      memset(&balFlows, 0, sizeof(BalIdList));
      bal_id_list__init(&balFlows);
      balIndCfg.group_ind->data->flows = &balFlows;

      balIndCfg.group_ind->data->flows->n_val =  group_ind->data.flows.len;
      balIndCfg.group_ind->data->flows->val =
           (uint32_t *)malloc((balIndCfg.group_ind->data->flows->n_val)*sizeof(uint32_t));
      memcpy(balIndCfg.group_ind->data->flows->val, group_ind->data.flows.val,
             balIndCfg.group_ind->data->flows->n_val);

      BalErr *output;
      result = bal_ind__bal_group_ind(client, NULL, &balIndCfg, &output, NULL, 0);
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
      ASFVOLT_LOG(ASFVOLT_ERROR, "Processing BAL API \'%s\' IND callback (status is %s)\n",
				  bcmbal_objtype_str(obj->obj_type), bcmos_strerror(obj->status));
      result = BCM_ERR_INTERNAL;
   }
   else
   {
      BalIndications balIndCfg;
      memset(&balIndCfg, 0, sizeof(BalIndications));
      bal_indications__init(&balIndCfg);
      balIndCfg.u_case = BAL_INDICATIONS__U_INTERFACE_OP_STATE;
      balIndCfg.has_objtype = BAL_GRPC_PRES;
      balIndCfg.objtype = obj->obj_type;
      balIndCfg.has_sub_group = BAL_GRPC_PRES;
      balIndCfg.sub_group = obj->subgroup;
      balIndCfg.device_id = voltha_device_id;

      bcmbal_interface_oper_status_change *int_osc_ind =
                                 (bcmbal_interface_oper_status_change *)obj;

      BalInterfaceOperStatusChange ifOsc;
      memset(&ifOsc, 0, sizeof(BalInterfaceOperStatusChange));
      bal_interface_oper_status_change__init(&ifOsc);
      balIndCfg.interface_op_state = &ifOsc;

      BalObj hdr;
      memset(&hdr, 0, sizeof(BalObj));
      bal_obj__init(&hdr);
      balIndCfg.interface_op_state->hdr = &hdr;

      BalInterfaceKey ifkey;
      memset(&ifkey, 0, sizeof(BalInterfaceKey));
      bal_interface_key__init(&ifkey);
      balIndCfg.interface_op_state->key = &ifkey;

      balIndCfg.interface_op_state->key->has_intf_id = BAL_GRPC_PRES;
      balIndCfg.interface_op_state->key->intf_id = int_osc_ind->key.intf_id;
      balIndCfg.interface_op_state->key->has_intf_type = BAL_GRPC_PRES;
      balIndCfg.interface_op_state->key->intf_type = int_osc_ind->key.intf_type;

      BalInterfaceOperStatusChangeData ifOscIndData;
      memset(&ifOscIndData, 0, sizeof(BalInterfaceOperStatusChangeData));
      bal_interface_oper_status_change_data__init(&ifOscIndData);
      balIndCfg.interface_op_state->data = &ifOscIndData;

      balIndCfg.interface_op_state->data->has_new_oper_status = BAL_GRPC_PRES;
      balIndCfg.interface_op_state->data->new_oper_status = int_osc_ind->data.new_oper_status;
      balIndCfg.interface_op_state->data->has_old_oper_status = BAL_GRPC_PRES;
      balIndCfg.interface_op_state->data->old_oper_status = int_osc_ind->data.old_oper_status;
      balIndCfg.interface_op_state->data->has_admin_state = BAL_GRPC_PRES;
      balIndCfg.interface_op_state->data->admin_state = int_osc_ind->data.admin_state;

      BalErr *output;
      result = bal_ind__bal_iface_oper_sts_cng(client, NULL, &balIndCfg, &output, NULL, 0);
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
      ASFVOLT_LOG(ASFVOLT_ERROR, "Processing BAL API \'%s\' IND callback (status is %s)\n",
				  bcmbal_objtype_str(obj->obj_type), bcmos_strerror(obj->status));
      result = BCM_ERR_INTERNAL;
   }
   else
   {
      BalIndications balIndCfg;
      memset(&balIndCfg, 0, sizeof(BalIndications));
      bal_indications__init(&balIndCfg);
      balIndCfg.u_case = BAL_INDICATIONS__U_INTERFACE_LOS;
      balIndCfg.has_objtype = BAL_GRPC_PRES;
      balIndCfg.objtype = obj->obj_type;
      balIndCfg.has_sub_group = BAL_GRPC_PRES;
      balIndCfg.sub_group = obj->subgroup;
      balIndCfg.device_id = voltha_device_id;

      bcmbal_interface_los *int_los_ind =
                                 (bcmbal_interface_los *)obj;

      BalInterfaceLos ifLos;
      memset(&ifLos, 0, sizeof(BalInterfaceLos));
      bal_interface_los__init(&ifLos);
      balIndCfg.interface_los = &ifLos;

      BalObj hdr;
      memset(&hdr, 0, sizeof(BalObj));
      bal_obj__init(&hdr);
      balIndCfg.interface_los->hdr = &hdr;

      BalInterfaceKey ifkey;
      memset(&ifkey, 0, sizeof(BalInterfaceKey));
      bal_interface_key__init(&ifkey);
      balIndCfg.interface_los->key = &ifkey;

      balIndCfg.interface_los->key->has_intf_id = BAL_GRPC_PRES;
      balIndCfg.interface_los->key->intf_id = int_los_ind->key.intf_id;
      balIndCfg.interface_los->key->has_intf_type = BAL_GRPC_PRES;
      balIndCfg.interface_los->key->intf_type = int_los_ind->key.intf_type;

      BalInterfaceLosData ifLosIndData;
      memset(&ifLosIndData, 0, sizeof(BalInterfaceLosData));
      bal_interface_los_data__init(&ifLosIndData);
      balIndCfg.interface_los->data = &ifLosIndData;

      balIndCfg.interface_los->data->has_status = BAL_GRPC_PRES;
      balIndCfg.interface_los->data->status = int_los_ind->data.status;

      BalErr *output;
      result = bal_ind__bal_iface_los(client, NULL, &balIndCfg, &output, NULL, 0);
   }

   return result;
}

/********************************************************************\
 * Function    : bal_interface_indication_cb                        *
 * Description : This function will handle the indications for      *
 *               Interface Indication                           *
 *                                                                  *
 ********************************************************************/
bcmos_errno bal_interface_indication_cb(bcmbal_obj *obj)
{
   bcmos_errno result = BCM_ERR_OK;

   if(BCMBAL_OBJ_ID_INTERFACE != obj->obj_type ||
      bcmbal_interface_auto_id_ind != obj->subgroup)
   {
      ASFVOLT_LOG(ASFVOLT_ERROR, "Processing BAL API \'%s\' IND callback (status is %s)\n",
				  bcmbal_objtype_str(obj->obj_type), bcmos_strerror(obj->status));
      result = BCM_ERR_INTERNAL;
   }
   else
   {
      BalIndications balIndCfg;
      memset(&balIndCfg, 0, sizeof(BalIndications));
      bal_indications__init(&balIndCfg);
      balIndCfg.u_case = BAL_INDICATIONS__U_INTERFACE_IND;
      balIndCfg.has_objtype = BAL_GRPC_PRES;
      balIndCfg.objtype = obj->obj_type;
      balIndCfg.has_sub_group = BAL_GRPC_PRES;
      balIndCfg.sub_group = obj->subgroup;
      balIndCfg.device_id = voltha_device_id;

      bcmbal_interface_ind *interface_ind = (bcmbal_interface_ind *)obj;

      BalInterfaceInd ifInd;
      memset(&ifInd, 0, sizeof(BalInterfaceInd));
      bal_interface_ind__init(&ifInd);
      balIndCfg.interface_ind = &ifInd;

      BalObj hdr;
      memset(&hdr, 0, sizeof(BalObj));
      bal_obj__init(&hdr);
      balIndCfg.interface_ind->hdr = &hdr;

      BalInterfaceKey ifkey;
      memset(&ifkey, 0, sizeof(BalInterfaceKey));
      bal_interface_key__init(&ifkey);
      balIndCfg.interface_ind->key = &ifkey;

      balIndCfg.interface_ind->key->has_intf_id = BAL_GRPC_PRES;
      balIndCfg.interface_ind->key->intf_id = interface_ind->key.intf_id;
      balIndCfg.interface_ind->key->has_intf_type = BAL_GRPC_PRES;
      balIndCfg.interface_ind->key->intf_type = interface_ind->key.intf_type;

      BalInterfaceIndData ifIndData;
      memset(&ifIndData, 0, sizeof(BalInterfaceIndData));
      bal_interface_ind_data__init(&ifIndData);
      balIndCfg.interface_ind->data = &ifIndData;

      balIndCfg.interface_ind->data->has_admin_state = BAL_GRPC_PRES;
      balIndCfg.interface_ind->data->admin_state = interface_ind->data.admin_state;
      balIndCfg.interface_ind->data->has_oper_status = BAL_GRPC_PRES;
      balIndCfg.interface_ind->data->oper_status = interface_ind->data.oper_status;
      balIndCfg.interface_ind->data->has_min_data_agg_port_id = BAL_GRPC_PRES;
      balIndCfg.interface_ind->data->min_data_agg_port_id = interface_ind->data.min_data_agg_port_id;
      balIndCfg.interface_ind->data->has_min_data_svc_port_id = BAL_GRPC_PRES;
      balIndCfg.interface_ind->data->min_data_svc_port_id = interface_ind->data.min_data_svc_port_id;
      balIndCfg.interface_ind->data->has_transceiver_type = BAL_GRPC_PRES;
      balIndCfg.interface_ind->data->transceiver_type = interface_ind->data.transceiver_type;
      balIndCfg.interface_ind->data->has_ds_miss_mode = BAL_GRPC_PRES;
      balIndCfg.interface_ind->data->ds_miss_mode = interface_ind->data.ds_miss_mode;
      balIndCfg.interface_ind->data->has_mtu = BAL_GRPC_PRES;
      balIndCfg.interface_ind->data->mtu = interface_ind->data.mtu;
      balIndCfg.interface_ind->data->has_flow_control = BAL_GRPC_PRES;
      balIndCfg.interface_ind->data->flow_control = interface_ind->data.flow_control;
      balIndCfg.interface_ind->data->has_ds_tm = BAL_GRPC_PRES;
      balIndCfg.interface_ind->data->ds_tm = interface_ind->data.ds_tm;
      balIndCfg.interface_ind->data->has_us_tm = BAL_GRPC_PRES;
      balIndCfg.interface_ind->data->us_tm = interface_ind->data.us_tm;

      BalIdList balFlows;
      memset(&balFlows, 0, sizeof(BalIdList));
      bal_id_list__init(&balFlows);
      balIndCfg.interface_ind->data->sub_term_id_list = &balFlows;

      balIndCfg.interface_ind->data->sub_term_id_list->n_val =  interface_ind->data.sub_term_id_list.len;
      balIndCfg.interface_ind->data->sub_term_id_list->val =
           (uint32_t *)malloc((balIndCfg.interface_ind->data->sub_term_id_list->n_val)*sizeof(uint32_t));
      memcpy(balIndCfg.interface_ind->data->sub_term_id_list->val, interface_ind->data.sub_term_id_list.val,
             balIndCfg.interface_ind->data->sub_term_id_list->n_val);

      BalErr *output;
      result = bal_ind__bal_iface_ind(client, NULL, &balIndCfg, &output, NULL, 0);
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
      ASFVOLT_LOG(ASFVOLT_ERROR, "Processing BAL API \'%s\' IND callback (status is %s)\n",
				  bcmbal_objtype_str(obj->obj_type), bcmos_strerror(obj->status));
      result = BCM_ERR_INTERNAL;
   }
   else
   {
      BalIndications balIndCfg;
      memset(&balIndCfg, 0, sizeof(BalIndications));
      bal_indications__init(&balIndCfg);
      balIndCfg.u_case = BAL_INDICATIONS__U_TERMINAL_OP_STATE;
      balIndCfg.has_objtype = BAL_GRPC_PRES;
      balIndCfg.objtype = obj->obj_type;
      balIndCfg.has_sub_group = BAL_GRPC_PRES;
      balIndCfg.sub_group = obj->subgroup;
      balIndCfg.device_id = voltha_device_id;

      bcmbal_subscriber_terminal_oper_status_change *sub_osc_ind =
                           (bcmbal_subscriber_terminal_oper_status_change *)obj;

      BalSubscriberTerminalOperStatusChange subOscInd;
      memset(&subOscInd, 0, sizeof(BalSubscriberTerminalOperStatusChange));
      bal_subscriber_terminal_oper_status_change__init(&subOscInd);
      balIndCfg.terminal_op_state = &subOscInd;

      BalObj hdr;
      memset(&hdr, 0, sizeof(BalObj));
      bal_obj__init(&hdr);
      balIndCfg.terminal_op_state->hdr = &hdr;

      BalSubscriberTerminalKey subkey;
      memset(&subkey, 0, sizeof(BalSubscriberTerminalKey));
      bal_subscriber_terminal_key__init(&subkey);
      balIndCfg.terminal_op_state->key = &subkey;

      balIndCfg.terminal_op_state->key->has_intf_id = BAL_GRPC_PRES;
      balIndCfg.terminal_op_state->key->intf_id = sub_osc_ind->key.intf_id;
      balIndCfg.terminal_op_state->key->has_sub_term_id = BAL_GRPC_PRES;
      balIndCfg.terminal_op_state->key->sub_term_id = sub_osc_ind->key.sub_term_id;

      BalSubscriberTerminalOperStatusChangeData subOscIndData;
      memset(&subOscIndData, 0, sizeof(BalSubscriberTerminalOperStatusChangeData));
      bal_subscriber_terminal_oper_status_change_data__init(&subOscIndData);
      balIndCfg.terminal_op_state->data = &subOscIndData;

      balIndCfg.terminal_op_state->data->has_new_oper_status = BAL_GRPC_PRES;
      balIndCfg.terminal_op_state->data->new_oper_status =
                                           sub_osc_ind->data.new_oper_status;
      balIndCfg.terminal_op_state->data->has_old_oper_status = BAL_GRPC_PRES;
      balIndCfg.terminal_op_state->data->old_oper_status =
                                           sub_osc_ind->data.old_oper_status;
      balIndCfg.terminal_op_state->data->has_admin_state = BAL_GRPC_PRES;
      balIndCfg.terminal_op_state->data->admin_state =
                                               sub_osc_ind->data.admin_state;

      BalErr *output;
      result = bal_ind__bal_subs_term_oper_sts_cng(client, NULL, &balIndCfg, &output, NULL, 0);
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
      ASFVOLT_LOG(ASFVOLT_ERROR, "Processing BAL API \'%s\' IND callback (status is %s)\n",
				  bcmbal_objtype_str(obj->obj_type), bcmos_strerror(obj->status));
      result = BCM_ERR_INTERNAL;
   }
   else
   {
      ASFVOLT_LOG(ASFVOLT_DEBUG, "Processing BAL API \'%s\' IND callback (status is %s)\n",
				  bcmbal_objtype_str(obj->obj_type), bcmos_strerror(obj->status));
      BalIndications balIndCfg;
      memset(&balIndCfg, 0, sizeof(BalIndications));
      bal_indications__init(&balIndCfg);
      balIndCfg.u_case = BAL_INDICATIONS__U_TERMINAL_DISC;
      balIndCfg.has_objtype = BAL_GRPC_PRES;
      balIndCfg.objtype = obj->obj_type;
      balIndCfg.has_sub_group = BAL_GRPC_PRES;
      balIndCfg.sub_group = obj->subgroup;
      balIndCfg.device_id = voltha_device_id;

      bcmbal_subscriber_terminal_sub_term_disc *sub_disc_ind =
                           (bcmbal_subscriber_terminal_sub_term_disc *)obj;

      BalSubscriberTerminalSubTermDisc subDiscInd;
      memset(&subDiscInd, 0, sizeof(BalSubscriberTerminalSubTermDisc));
      bal_subscriber_terminal_sub_term_disc__init(&subDiscInd);
      balIndCfg.terminal_disc = &subDiscInd;

      BalObj hdr;
      memset(&hdr, 0, sizeof(BalObj));
      bal_obj__init(&hdr);
      balIndCfg.terminal_disc->hdr = &hdr;

      BalSubscriberTerminalKey subkey;
      memset(&subkey, 0, sizeof(BalSubscriberTerminalKey));
      bal_subscriber_terminal_key__init(&subkey);
      balIndCfg.terminal_disc->key = &subkey;

      balIndCfg.terminal_disc->key->has_intf_id = BAL_GRPC_PRES;
      balIndCfg.terminal_disc->key->intf_id = sub_disc_ind->key.intf_id;
      balIndCfg.terminal_disc->key->has_sub_term_id = BAL_GRPC_PRES;
      balIndCfg.terminal_disc->key->sub_term_id = sub_disc_ind->key.sub_term_id;

      BalSubscriberTerminalSubTermDiscData subDiscIndData;
      memset(&subDiscIndData, 0, sizeof(BalSubscriberTerminalSubTermDiscData));
      bal_subscriber_terminal_sub_term_disc_data__init(&subDiscIndData);
      balIndCfg.terminal_disc->data = &subDiscIndData;

      BalSerialNumber serial_number;
      memset(&serial_number, 0, sizeof(BalSerialNumber));
      bal_serial_number__init(&serial_number);

      balIndCfg.terminal_disc->data->serial_number = &serial_number;

      ASFVOLT_LOG(ASFVOLT_DEBUG, "Discovered ONU serial number "
                "%2X%2X%2X%2X%1X%1X%1X%1X%1X%1X%1X%1X ",
                sub_disc_ind->data.serial_number.vendor_id[0],
                sub_disc_ind->data.serial_number.vendor_id[1],
                sub_disc_ind->data.serial_number.vendor_id[2],
                sub_disc_ind->data.serial_number.vendor_id[3],
                sub_disc_ind->data.serial_number.vendor_specific[0]>>4 & 0x0f,
                sub_disc_ind->data.serial_number.vendor_specific[0] & 0x0f,
                sub_disc_ind->data.serial_number.vendor_specific[1]>>4 & 0x0f,
                sub_disc_ind->data.serial_number.vendor_specific[1] & 0x0f,
                sub_disc_ind->data.serial_number.vendor_specific[2]>>4 & 0x0f,
                sub_disc_ind->data.serial_number.vendor_specific[2] & 0x0f,
                sub_disc_ind->data.serial_number.vendor_specific[3]>>4 & 0x0f,
                sub_disc_ind->data.serial_number.vendor_specific[3] & 0x0f);

      ASFVOLT_LOG(ASFVOLT_DEBUG, "ONU Discovery:Before decoding:Vendor id is %s\n", sub_disc_ind->data.serial_number.vendor_id);
      ASFVOLT_LOG(ASFVOLT_DEBUG, "ONU Discovery:Before decoding:Vendor specific is %s\n", sub_disc_ind->data.serial_number.vendor_specific);

      char vendor_id[MAX_CHAR_LENGTH];
      memset(&vendor_id, 0, MAX_CHAR_LENGTH);
      sprintf(vendor_id,"%c%c%c%c",
		      sub_disc_ind->data.serial_number.vendor_id[0],
		      sub_disc_ind->data.serial_number.vendor_id[1],
		      sub_disc_ind->data.serial_number.vendor_id[2],
		      sub_disc_ind->data.serial_number.vendor_id[3]);
      balIndCfg.terminal_disc->data->serial_number->vendor_id = vendor_id;
      ASFVOLT_LOG(ASFVOLT_DEBUG, "ONU Discovery:After decoding:Vendor id is %s\n", balIndCfg.terminal_disc->data->serial_number->vendor_id);
      char vendor_specific[MAX_CHAR_LENGTH];
      memset(&vendor_specific, 0, MAX_CHAR_LENGTH);
      sprintf(vendor_specific,"%1X%1X%1X%1X%1X%1X%1X%1X",
		      sub_disc_ind->data.serial_number.vendor_specific[0]>>4 & 0x0f,
		      sub_disc_ind->data.serial_number.vendor_specific[0] & 0x0f,
		      sub_disc_ind->data.serial_number.vendor_specific[1]>>4 & 0x0f,
		      sub_disc_ind->data.serial_number.vendor_specific[1] & 0x0f,
		      sub_disc_ind->data.serial_number.vendor_specific[2]>>4 & 0x0f,
		      sub_disc_ind->data.serial_number.vendor_specific[2] & 0x0f,
		      sub_disc_ind->data.serial_number.vendor_specific[3]>>4 & 0x0f,
		      sub_disc_ind->data.serial_number.vendor_specific[3] & 0x0f);
      balIndCfg.terminal_disc->data->serial_number->vendor_specific = vendor_specific;
      ASFVOLT_LOG(ASFVOLT_DEBUG, "ONU Discovery:After decoding:Vendor specific is %s\n", balIndCfg.terminal_disc->data->serial_number->vendor_specific);

      BalErr *output;
      result = bal_ind__bal_subs_term_discovery_ind(client, NULL, &balIndCfg, &output, NULL, 0);
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
      ASFVOLT_LOG(ASFVOLT_ERROR, "Processing BAL API \'%s\' IND callback (status is %s)\n",
				  bcmbal_objtype_str(obj->obj_type), bcmos_strerror(obj->status));
      result = BCM_ERR_INTERNAL;
   }
   else
   {
      BalIndications balIndCfg;
      memset(&balIndCfg, 0, sizeof(BalIndications));
      bal_indications__init(&balIndCfg);
      balIndCfg.has_objtype = BAL_GRPC_PRES;
      balIndCfg.objtype = BAL_OBJ_ID__BAL_OBJ_ID_SUBSCRIBER_TERMINAL;
      balIndCfg.has_sub_group = BAL_GRPC_PRES;
      balIndCfg.sub_group = obj->subgroup;

      balIndCfg.device_id = voltha_device_id;

      balIndCfg.u_case = BAL_INDICATIONS__U_TERMINAL_ALARM;
      bcmbal_subscriber_terminal_sub_term_alarm *sub_term_alarm  =
                           (bcmbal_subscriber_terminal_sub_term_alarm *)obj;

      BalSubscriberTerminalSubTermAlarm subTermAlarm;
      memset(&subTermAlarm, 0, sizeof(BalSubscriberTerminalSubTermAlarm));
      bal_subscriber_terminal_sub_term_alarm__init(&subTermAlarm);
      balIndCfg.terminal_alarm = &subTermAlarm;

      BalObj hdr;
      memset(&hdr, 0, sizeof(BalObj));
      bal_obj__init(&hdr);
      balIndCfg.terminal_alarm->hdr = &hdr;

      BalSubscriberTerminalKey subkey;
      memset(&subkey, 0, sizeof(BalSubscriberTerminalKey));
      bal_subscriber_terminal_key__init(&subkey);
      balIndCfg.terminal_alarm->key = &subkey;

      balIndCfg.terminal_alarm->key->has_sub_term_id = BAL_GRPC_PRES;
      balIndCfg.terminal_alarm->key->sub_term_id = sub_term_alarm->key.sub_term_id;
      balIndCfg.terminal_alarm->key->has_intf_id = BAL_GRPC_PRES;
      balIndCfg.terminal_alarm->key->intf_id = sub_term_alarm->key.intf_id;

      BalsubscriberTerminalSubTermAlarmData subTermAlarmData;
      memset(&subTermAlarmData, 0, sizeof(BalsubscriberTerminalSubTermAlarmData));
      balsubscriber_terminal_sub_term_alarm_data__init(&subTermAlarmData);
      balIndCfg.terminal_alarm->data = &subTermAlarmData;

      BalSubscriberTerminalAlarms balSubAlarms;
      memset(&balSubAlarms, 0, sizeof(BalSubscriberTerminalAlarms));
      bal_subscriber_terminal_alarms__init(&balSubAlarms);
      balIndCfg.terminal_alarm->data->alarm = &balSubAlarms;

      balIndCfg.terminal_alarm->data->alarm->has_los = BAL_GRPC_PRES;
      balIndCfg.terminal_alarm->data->alarm->los = sub_term_alarm->data.alarm.los;
      balIndCfg.terminal_alarm->data->alarm->has_lob = BAL_GRPC_PRES;
      balIndCfg.terminal_alarm->data->alarm->lob = sub_term_alarm->data.alarm.lob;
      balIndCfg.terminal_alarm->data->alarm->has_lopc_miss = BAL_GRPC_PRES;
      balIndCfg.terminal_alarm->data->alarm->lopc_miss = sub_term_alarm->data.alarm.lopc_miss;
      balIndCfg.terminal_alarm->data->alarm->has_lopc_mic_error = BAL_GRPC_PRES;
      balIndCfg.terminal_alarm->data->alarm->lopc_mic_error = sub_term_alarm->data.alarm.lopc_mic_error;

      BalErr *output;
      result = bal_ind__bal_subs_term_alarm_ind(client, NULL, &balIndCfg, &output, NULL, 0);
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
      ASFVOLT_LOG(ASFVOLT_ERROR, "Processing BAL API \'%s\' IND callback (status is %s)\n",
				  bcmbal_objtype_str(obj->obj_type), bcmos_strerror(obj->status));
      result = BCM_ERR_INTERNAL;
   }
   else
   {
      BalIndications balIndCfg;
      memset(&balIndCfg, 0, sizeof(BalIndications));
      bal_indications__init(&balIndCfg);
      balIndCfg.u_case = BAL_INDICATIONS__U_TERMINAL_DGI;
      balIndCfg.has_objtype = BAL_GRPC_PRES;
      balIndCfg.objtype = obj->obj_type;
      balIndCfg.has_sub_group = BAL_GRPC_PRES;
      balIndCfg.sub_group = obj->subgroup;
      balIndCfg.device_id = voltha_device_id;

      bcmbal_subscriber_terminal_dgi *sub_dgi_ind =
                           (bcmbal_subscriber_terminal_dgi *)obj;

      BalSubscriberTerminalDgi subDgiInd;
      memset(&subDgiInd, 0, sizeof(BalSubscriberTerminalDgi));
      bal_subscriber_terminal_dgi__init(&subDgiInd);
      balIndCfg.terminal_dgi= &subDgiInd;

      BalObj hdr;
      memset(&hdr, 0, sizeof(BalObj));
      bal_obj__init(&hdr);
      balIndCfg.terminal_dgi->hdr = &hdr;

      BalSubscriberTerminalKey subkey;
      memset(&subkey, 0, sizeof(BalSubscriberTerminalKey));
      bal_subscriber_terminal_key__init(&subkey);
      balIndCfg.terminal_disc->key = &subkey;

      balIndCfg.terminal_dgi->key->has_intf_id = BAL_GRPC_PRES;
      balIndCfg.terminal_dgi->key->intf_id = sub_dgi_ind->key.intf_id;
      balIndCfg.terminal_dgi->key->has_sub_term_id = BAL_GRPC_PRES;
      balIndCfg.terminal_dgi->key->sub_term_id = sub_dgi_ind->key.sub_term_id;

      BalSubscriberTerminalDgiData subDgiIndData;
      memset(&subDgiIndData, 0, sizeof(BalSubscriberTerminalDgiData));
      bal_subscriber_terminal_dgi_data__init(&subDgiIndData);
      balIndCfg.terminal_dgi->data = &subDgiIndData;

      balIndCfg.terminal_dgi->data->has_dgi_status = BAL_GRPC_PRES;
      balIndCfg.terminal_dgi->data->dgi_status = sub_dgi_ind->data.dgi_status;

      BalErr *output;
      result = bal_ind__bal_subs_term_dgi_ind(client, NULL, &balIndCfg, &output, NULL, 0);
   }

   return result;
}

/********************************************************************\
 * Function    : bal_sub_term_indication_cb                         *
 * Description : This function will handle the indications for      *
 *               Subscriber term indication                         *
 *                                                                  *
 ********************************************************************/
bcmos_errno bal_sub_term_indication_cb(bcmbal_obj *obj)
{
   bcmos_errno result = BCM_ERR_OK;

   if(BCMBAL_OBJ_ID_SUBSCRIBER_TERMINAL != obj->obj_type ||
      bcmbal_subscriber_terminal_auto_id_ind != obj->subgroup)
   {
      ASFVOLT_LOG(ASFVOLT_ERROR, "Processing BAL API \'%s\' IND callback (status is %s)\n",
				  bcmbal_objtype_str(obj->obj_type), bcmos_strerror(obj->status));
      result = BCM_ERR_INTERNAL;
   }
   else
   {
      BalIndications balIndCfg;
      memset(&balIndCfg, 0, sizeof(BalIndications));
      bal_indications__init(&balIndCfg);
      balIndCfg.u_case = BAL_INDICATIONS__U_TERMINAL_IND;
      balIndCfg.has_objtype = BAL_GRPC_PRES;
      balIndCfg.objtype = obj->obj_type;
      balIndCfg.has_sub_group = BAL_GRPC_PRES;
      balIndCfg.sub_group = obj->subgroup;
      balIndCfg.device_id = voltha_device_id;

      bcmbal_subscriber_terminal_ind *sub_ind = (bcmbal_subscriber_terminal_ind *)obj;

      BalSubscriberTerminalInd subInd;
      memset(&subInd, 0, sizeof(BalSubscriberTerminalInd));
      bal_subscriber_terminal_ind__init(&subInd);
      balIndCfg.terminal_ind = &subInd;

      BalObj hdr;
      memset(&hdr, 0, sizeof(BalObj));
      bal_obj__init(&hdr);
      balIndCfg.terminal_ind->hdr = &hdr;

      BalSubscriberTerminalKey subkey;
      memset(&subkey, 0, sizeof(BalSubscriberTerminalKey));
      bal_subscriber_terminal_key__init(&subkey);
      balIndCfg.terminal_ind->key = &subkey;

      balIndCfg.terminal_ind->key->has_intf_id = BAL_GRPC_PRES;
      balIndCfg.terminal_ind->key->intf_id = sub_ind->key.intf_id;
      balIndCfg.terminal_ind->key->has_sub_term_id = BAL_GRPC_PRES;
      balIndCfg.terminal_ind->key->sub_term_id = sub_ind->key.sub_term_id;

      BalSubscriberTerminalIndData subIndData;
      memset(&subIndData, 0, sizeof(BalSubscriberTerminalIndData));
      bal_subscriber_terminal_ind_data__init(&subIndData);
      balIndCfg.terminal_ind->data = &subIndData;

      balIndCfg.terminal_ind->data->has_admin_state = BAL_GRPC_PRES;
      balIndCfg.terminal_ind->data->admin_state = sub_ind->data.admin_state;
      balIndCfg.terminal_ind->data->has_oper_status = BAL_GRPC_PRES;
      balIndCfg.terminal_ind->data->oper_status = sub_ind->data.oper_status;
      balIndCfg.terminal_ind->data->has_svc_port_id = BAL_GRPC_PRES;
      balIndCfg.terminal_ind->data->svc_port_id = sub_ind->data.svc_port_id;
      balIndCfg.terminal_ind->data->has_ds_tm = BAL_GRPC_PRES;
      balIndCfg.terminal_ind->data->ds_tm = sub_ind->data.ds_tm;
      balIndCfg.terminal_ind->data->has_us_tm = BAL_GRPC_PRES;
      balIndCfg.terminal_ind->data->us_tm = sub_ind->data.us_tm;
      balIndCfg.terminal_ind->data->has_sub_term_rate = BAL_GRPC_PRES;
      balIndCfg.terminal_ind->data->sub_term_rate = sub_ind->data.sub_term_rate;
      char password[MAX_CHAR_LENGTH*2];
      memset(&password, 0, MAX_CHAR_LENGTH*2);
      strcpy(password,(const char *)sub_ind->data.password.arr);
      balIndCfg.terminal_ind->data->password = password;
      char registration_id[MAX_CHAR_LENGTH*8];
      memset(&registration_id, 0, MAX_CHAR_LENGTH*8);
      strcpy(registration_id,(const char *)sub_ind->data.registration_id.arr);
      balIndCfg.terminal_ind->data->registration_id =  registration_id;

#if 0
      balIndCfg.terminal_ind->data->has_mac_address = BAL_GRPC_PRES;
      balIndCfg.terminal_ind->data->mac_address.len =
                            (BCMOS_ETH_ALEN)*sizeof(sub_ind->data.mac_address.u8);
      uint8_t mac_address[balIndCfg.terminal_ind->data->mac_address.len];
      memset(&mac_address, 0 ,balIndCfg.terminal_ind->data->mac_address.len);
      strcpy((char *)mac_address,(const char *)sub_ind->data.mac_address.u8);
      balIndCfg.terminal_ind->data->mac_address.data = mac_address;
#endif

      BalSerialNumber serialNum;
      memset(&serialNum, 0, sizeof(BalSerialNumber));
      bal_serial_number__init(&serialNum);
      balIndCfg.terminal_ind->data->serial_number = &serialNum;

      ASFVOLT_LOG(ASFVOLT_ERROR, "ONU Activation:Before decoding:Vendor id is %s\n", sub_ind->data.serial_number.vendor_id);
      ASFVOLT_LOG(ASFVOLT_ERROR, "ONU Activation:Before decoding:Vendor specific is %s\n", sub_ind->data.serial_number.vendor_specific);

      char vendor_id[MAX_CHAR_LENGTH];
      memset(&vendor_id, 0, MAX_CHAR_LENGTH);
      sprintf(vendor_id,"%c%c%c%c",
		      sub_ind->data.serial_number.vendor_id[0],
		      sub_ind->data.serial_number.vendor_id[1],
		      sub_ind->data.serial_number.vendor_id[2],
		      sub_ind->data.serial_number.vendor_id[3]);
      balIndCfg.terminal_ind->data->serial_number->vendor_id = vendor_id;
      ASFVOLT_LOG(ASFVOLT_ERROR, "ONU Activation:After decoding:Vendor id is %s\n", balIndCfg.terminal_ind->data->serial_number->vendor_id);
      char vendor_specific[MAX_CHAR_LENGTH];
      memset(&vendor_specific, 0, MAX_CHAR_LENGTH);
      sprintf(vendor_specific,"%1X%1X%1X%1X%1X%1X%1X%1X",
		      sub_ind->data.serial_number.vendor_specific[0]>>4 & 0x0f,
		      sub_ind->data.serial_number.vendor_specific[0] & 0x0f,
		      sub_ind->data.serial_number.vendor_specific[1]>>4 & 0x0f,
		      sub_ind->data.serial_number.vendor_specific[1] & 0x0f,
		      sub_ind->data.serial_number.vendor_specific[2]>>4 & 0x0f,
		      sub_ind->data.serial_number.vendor_specific[2] & 0x0f,
		      sub_ind->data.serial_number.vendor_specific[3]>>4 & 0x0f,
		      sub_ind->data.serial_number.vendor_specific[3] & 0x0f);
      balIndCfg.terminal_ind->data->serial_number->vendor_specific = vendor_specific;
      ASFVOLT_LOG(ASFVOLT_ERROR, "ONU Activation:After decoding:Vendor specific is %s\n", balIndCfg.terminal_ind->data->serial_number->vendor_specific);

      BalIdList balAggportList;
      memset(&balAggportList, 0, sizeof(BalIdList));
      bal_id_list__init(&balAggportList);
      balIndCfg.terminal_ind->data->agg_port_id_list = &balAggportList;

#if 0
      balIndCfg.terminal_ind->data->agg_port_id_list->n_val =  sub_ind->data.agg_port_id_list.len;
      uint32_t agg_port_id_list[balIndCfg.terminal_ind->data->agg_port_id_list->n_val];
      memset(&agg_port_id_list, 0, balIndCfg.terminal_ind->data->agg_port_id_list->n_val);
      strcpy((char *)agg_port_id_list,(const char *)sub_ind->data.agg_port_id_list.val);
      balIndCfg.terminal_ind->data->agg_port_id_list->val = agg_port_id_list;
#endif

      BalErr *output;
      result = bal_ind__bal_subs_term_ind(client, NULL, &balIndCfg, &output, NULL, 0);
   }

   return result;
}

/********************************************************************\
 * Function    : fill_bal_tm_red                                    *
 * Description : This function will fill grpc-BalTmred struture     *
 *               from bal-bcmbal_tm_red structure                   *
 *                                                                  *
 ********************************************************************/
void fill_bal_tm_red(BalTmred *grpc_red, bcmbal_tm_red *bal_red)
{
   grpc_red->has_min_threshold = BAL_GRPC_PRES;
   grpc_red->min_threshold = bal_red->min_threshold;
   grpc_red->has_max_threshold = BAL_GRPC_PRES;
   grpc_red->max_threshold = bal_red->max_threshold;
   grpc_red->has_max_probability = BAL_GRPC_PRES;
   grpc_red->max_probability = bal_red->max_probability;
   return;
}

/********************************************************************\
 * Function    : bal_tm_queue_indication_cb                         *
 * Description : This function will handle the indications for      *
 *               TM Queue indication                                *
 *                                                                  *
 ********************************************************************/
bcmos_errno bal_tm_queue_indication_cb(bcmbal_obj *obj)
{
   bcmos_errno result = BCM_ERR_OK;

   if(BCMBAL_OBJ_ID_TM_QUEUE != obj->obj_type ||
      bcmbal_tm_queue_auto_id_ind != obj->subgroup)
   {
      ASFVOLT_LOG(ASFVOLT_ERROR, "Processing BAL API \'%s\' IND callback (status is %s)\n",
				  bcmbal_objtype_str(obj->obj_type), bcmos_strerror(obj->status));
      result = BCM_ERR_INTERNAL;
   }
   else
   {
      BalIndications balIndCfg;
      memset(&balIndCfg, 0, sizeof(BalIndications));
      bal_indications__init(&balIndCfg);
      balIndCfg.u_case = BAL_INDICATIONS__U_TM_QUEUE__IND;
      balIndCfg.has_objtype = BAL_GRPC_PRES;
      balIndCfg.objtype = obj->obj_type;
      balIndCfg.has_sub_group = BAL_GRPC_PRES;
      balIndCfg.sub_group = obj->subgroup;
      balIndCfg.device_id = voltha_device_id;

      bcmbal_tm_queue_ind *tm_que_ind = (bcmbal_tm_queue_ind *)obj;

      BalTmQueueInd tmQueInd;
      memset(&tmQueInd, 0, sizeof(BalTmQueueInd));
      bal_tm_queue_ind__init(&tmQueInd);
      balIndCfg.tm_queue_ind = &tmQueInd;

      BalTmQueueKey tmQkey;
      memset(&tmQkey, 0, sizeof(BalTmQueueKey));
      bal_tm_queue_key__init(&tmQkey);
      balIndCfg.tm_queue_ind->key = &tmQkey;

      balIndCfg.tm_queue_ind->key->has_sched_id = BAL_GRPC_PRES;
      balIndCfg.tm_queue_ind->key->sched_id = tm_que_ind->key.sched_id;
      balIndCfg.tm_queue_ind->key->has_sched_dir = BAL_GRPC_PRES;
      balIndCfg.tm_queue_ind->key->sched_dir = tm_que_ind->key.sched_dir;
      balIndCfg.tm_queue_ind->key->has_id = BAL_GRPC_PRES;
      balIndCfg.tm_queue_ind->key->id = tm_que_ind->key.id;

      BalTmQueueIndData tmQIndData;
      memset(&tmQIndData, 0, sizeof(BalTmQueueIndData));
      bal_tm_queue_ind_data__init(&tmQIndData);
      balIndCfg.tm_queue_ind->data = &tmQIndData;

      balIndCfg.tm_queue_ind->data->has_priority = BAL_GRPC_PRES;
      balIndCfg.tm_queue_ind->data->priority = tm_que_ind->data.priority;
      balIndCfg.tm_queue_ind->data->has_weight = BAL_GRPC_PRES;
      balIndCfg.tm_queue_ind->data->weight = tm_que_ind->data.weight;
      balIndCfg.tm_queue_ind->data->has_create_mode = BAL_GRPC_PRES;
      balIndCfg.tm_queue_ind->data->create_mode = tm_que_ind->data.create_mode;
      balIndCfg.tm_queue_ind->data->has_ref_count = BAL_GRPC_PRES;
      balIndCfg.tm_queue_ind->data->ref_count = tm_que_ind->data.ref_count;

      BalTmShaping balShape;
      memset(&balShape, 0, sizeof(BalTmShaping));
      bal_tm_shaping__init(&balShape);
      balIndCfg.tm_queue_ind->data->rate = &balShape;

      balIndCfg.tm_queue_ind->data->rate->has_presence_mask = BAL_GRPC_PRES;
      balIndCfg.tm_queue_ind->data->rate->presence_mask = tm_que_ind->data.rate.presence_mask;
      balIndCfg.tm_queue_ind->data->rate->has_cir = BAL_GRPC_PRES;
      balIndCfg.tm_queue_ind->data->rate->cir = tm_que_ind->data.rate.cir;
      balIndCfg.tm_queue_ind->data->rate->has_pir = BAL_GRPC_PRES;
      balIndCfg.tm_queue_ind->data->rate->pir = tm_que_ind->data.rate.pir;
      balIndCfg.tm_queue_ind->data->rate->has_burst = BAL_GRPC_PRES;
      balIndCfg.tm_queue_ind->data->rate->burst = tm_que_ind->data.rate.burst;

      BalTmBac balBac;
      memset(&balBac, 0, sizeof(BalTmBac));
      bal_tm_bac__init(&balBac);
      balIndCfg.tm_queue_ind->data->bac = &balBac;

      balIndCfg.tm_queue_ind->data->bac->has_type = BAL_GRPC_PRES;
      balIndCfg.tm_queue_ind->data->bac->type = tm_que_ind->data.bac.type;
      switch(tm_que_ind->data.bac.type)
      {
         case BCMBAL_TM_BAC_TYPE_TAILDROP:
         {
            balIndCfg.tm_queue_ind->data->bac->u_case = BAL_TM_BAC__U_TAILDROP;
            BalTMBacTaildrop balTaildrop;
            memset(&balTaildrop, 0, sizeof(BalTMBacTaildrop));
            bal_tmbac_taildrop__init(&balTaildrop);
            balIndCfg.tm_queue_ind->data->bac->taildrop = &balTaildrop;

            balIndCfg.tm_queue_ind->data->bac->taildrop->has_max_size = BAL_GRPC_PRES;
            balIndCfg.tm_queue_ind->data->bac->taildrop->max_size =
                                               tm_que_ind->data.bac.u.taildrop.max_size;
         }
         break;
         case BCMBAL_TM_BAC_TYPE_WTAILDROP:
         {
           /* No bal/grpc structure was defined */
         }
         break;
         case BCMBAL_TM_BAC_TYPE_RED:
         {
            balIndCfg.tm_queue_ind->data->bac->u_case = BAL_TM_BAC__U_RED;
            BalTMBacRed balBacRed;
            memset(&balBacRed, 0, sizeof(BalTMBacRed));
            bal_tmbac_red__init(&balBacRed);
            balIndCfg.tm_queue_ind->data->bac->red = &balBacRed;

            BalTmred balRed;
            memset(&balRed, 0, sizeof(BalTmred));
            bal_tmred__init(&balRed);
            balIndCfg.tm_queue_ind->data->bac->red->red = &balRed;
            fill_bal_tm_red(balIndCfg.tm_queue_ind->data->bac->red->red, &tm_que_ind->data.bac.u.red.red);
         }
         break;
         case BCMBAL_TM_BAC_TYPE_WRED:
         {
            balIndCfg.tm_queue_ind->data->bac->u_case = BAL_TM_BAC__U_WRED;
            BalTMBacWred balBacWred;
            memset(&balBacWred, 0, sizeof(BalTMBacWred));
            bal_tmbac_wred__init(&balBacWred);
            balIndCfg.tm_queue_ind->data->bac->wred = &balBacWred;

            BalTmred balGreen;
            memset(&balGreen, 0, sizeof(BalTmred));
            bal_tmred__init(&balGreen);
            balIndCfg.tm_queue_ind->data->bac->wred->green = &balGreen;
            fill_bal_tm_red(balIndCfg.tm_queue_ind->data->bac->wred->green, &tm_que_ind->data.bac.u.wred.green);

            BalTmred balYellow;
            memset(&balYellow, 0, sizeof(BalTmred));
            bal_tmred__init(&balYellow);
            balIndCfg.tm_queue_ind->data->bac->wred->yellow = &balYellow;
            fill_bal_tm_red(balIndCfg.tm_queue_ind->data->bac->wred->yellow, &tm_que_ind->data.bac.u.wred.yellow);

            BalTmred balRed;
            memset(&balRed, 0, sizeof(BalTmred));
            bal_tmred__init(&balRed);
            balIndCfg.tm_queue_ind->data->bac->wred->red = &balRed;
            fill_bal_tm_red(balIndCfg.tm_queue_ind->data->bac->wred->red, &tm_que_ind->data.bac.u.wred.red);
         }
         break;
         default:
         {
            balIndCfg.tm_queue_ind->data->bac->u_case = BAL_TM_BAC__U__NOT_SET;
         }
         break;

      }

      BalErr *output;
      result = bal_ind__bal_tm_queue_ind_info(client, NULL, &balIndCfg, &output, NULL, 0);
   }

   return result;
}

/********************************************************************\
 * Function    : bal_tm_sched_indication_cb                         *
 * Description : This function will handle the indications for      *
 *               TM Sched indication                                *
 *                                                                  *
 ********************************************************************/
bcmos_errno bal_tm_sched_indication_cb(bcmbal_obj *obj)
{
   bcmos_errno result = BCM_ERR_OK;

   if(BCMBAL_OBJ_ID_TM_SCHED != obj->obj_type ||
      bcmbal_tm_sched_auto_id_ind != obj->subgroup)
   {
      ASFVOLT_LOG(ASFVOLT_ERROR, "Processing BAL API \'%s\' IND callback (status is %s)\n",
				  bcmbal_objtype_str(obj->obj_type), bcmos_strerror(obj->status));
      result = BCM_ERR_INTERNAL;
   }
   else
   {
      BalIndications balIndCfg;
      memset(&balIndCfg, 0, sizeof(BalIndications));
      bal_indications__init(&balIndCfg);
      balIndCfg.u_case = BAL_INDICATIONS__U_TM_SCHED__IND;
      balIndCfg.has_objtype = BAL_GRPC_PRES;
      balIndCfg.objtype = obj->obj_type;
      balIndCfg.has_sub_group = BAL_GRPC_PRES;
      balIndCfg.sub_group = obj->subgroup;
      balIndCfg.device_id = voltha_device_id;

      bcmbal_tm_sched_ind *tm_sched_ind = (bcmbal_tm_sched_ind *)obj;

      BalTmSchedInd tmSchedInd;
      memset(&tmSchedInd, 0, sizeof(BalTmSchedInd));
      bal_tm_sched_ind__init(&tmSchedInd);
      balIndCfg.tm_sched_ind = &tmSchedInd;

      BalTmSchedKey tmSchedkey;
      memset(&tmSchedkey, 0, sizeof(BalTmSchedKey));
      bal_tm_sched_key__init(&tmSchedkey);
      balIndCfg.tm_sched_ind->key = &tmSchedkey;

      balIndCfg.tm_sched_ind->key->has_dir = BAL_GRPC_PRES;
      balIndCfg.tm_sched_ind->key->dir = tm_sched_ind->key.dir;
      balIndCfg.tm_sched_ind->key->has_id = BAL_GRPC_PRES;
      balIndCfg.tm_sched_ind->key->id = tm_sched_ind->key.id;

      BalTmSchedIndData tmSIndData;
      memset(&tmSIndData, 0, sizeof(BalTmSchedIndData));
      bal_tm_sched_ind_data__init(&tmSIndData);
      balIndCfg.tm_sched_ind->data = &tmSIndData;
      /* TODO: data should be populate */

      BalErr *output;
      result = bal_ind__bal_tm_sched_ind_info(client, NULL, &balIndCfg, &output, NULL, 0);
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
      ASFVOLT_LOG(ASFVOLT_ERROR, "Processing BAL API \'%s\' IND callback (status is %s)\n",
				  bcmbal_objtype_str(obj->obj_type), bcmos_strerror(obj->status));
      result = BCM_ERR_INTERNAL;
   }
   else
   {
      /*bcmos_mutex_lock(&bal_ind_lock);-- Need to define bcm independent mutex*/

      BalIndications balIndCfg;
      memset(&balIndCfg, 0, sizeof(BalIndications));
      bal_indications__init(&balIndCfg);
      balIndCfg.u_case = BAL_INDICATIONS__U_PKT_DATA;
      balIndCfg.has_objtype = BAL_GRPC_PRES;
      balIndCfg.objtype = obj->obj_type;
      balIndCfg.has_sub_group = BAL_GRPC_PRES;
      balIndCfg.sub_group = obj->subgroup;
      balIndCfg.device_id = voltha_device_id;

      BalPacketBearerChannelRx rxChannel;
      memset(&rxChannel, 0, sizeof(BalPacketBearerChannelRx));
      bal_packet_bearer_channel_rx__init(&rxChannel);
      balIndCfg.pktdata = &rxChannel;

      BalObj hdr;
      memset(&hdr, 0, sizeof(BalObj));
      bal_obj__init(&hdr);
      balIndCfg.pktdata->hdr = &hdr;

      BalPacketKey packetkey;
      memset(&packetkey, 0, sizeof(BalPacketKey));
      bal_packet_key__init(&packetkey);
      balIndCfg.pktdata->key = &packetkey;

      bcmbal_packet_bearer_channel_rx *rx_channel =
                                  (bcmbal_packet_bearer_channel_rx *)obj;

      balIndCfg.pktdata->key->has_reserved = BAL_GRPC_PRES;
      balIndCfg.pktdata->key->reserved = rx_channel->key.reserved;

      BalDest PktSndDest;
      memset(&PktSndDest, 0, sizeof(BalDest));
      bal_dest__init(&PktSndDest);
      balIndCfg.pktdata->key->packet_send_dest = &PktSndDest;

      balIndCfg.pktdata->key->packet_send_dest->has_type = BAL_GRPC_PRES;
      balIndCfg.pktdata->key->packet_send_dest->type =
                                          rx_channel->key.packet_send_dest.type;
      switch( rx_channel->key.packet_send_dest.type)
      {
         case BCMBAL_DEST_TYPE_NNI:
         {
            balIndCfg.pktdata->key->packet_send_dest->u_case = BAL_DEST__U_NNI;
            BalDestNni balNni;
            memset(&balNni, 0, sizeof(BalDestNni));
            bal_dest_nni__init(&balNni);
            balIndCfg.pktdata->key->packet_send_dest->nni = &balNni;
            balIndCfg.pktdata->key->packet_send_dest->nni->has_intf_id = BAL_GRPC_PRES;
            balIndCfg.pktdata->key->packet_send_dest->nni->intf_id =
                                          rx_channel->key.packet_send_dest.u.nni.intf_id;
         }
         break;
         case BCMBAL_DEST_TYPE_SUB_TERM:
         {
            balIndCfg.pktdata->key->packet_send_dest->u_case = BAL_DEST__U_SUB_TERM;
            BalDestSubTerm balSubTerm;
            memset(&balSubTerm, 0, sizeof(BalDestSubTerm));
            bal_dest_sub_term__init(&balSubTerm);
            balIndCfg.pktdata->key->packet_send_dest->sub_term = &balSubTerm;
            balIndCfg.pktdata->key->packet_send_dest->sub_term->has_sub_term_id =
                                                                            BAL_GRPC_PRES;
            balIndCfg.pktdata->key->packet_send_dest->sub_term->sub_term_id =
                                 rx_channel->key.packet_send_dest.u.sub_term.sub_term_id;
            balIndCfg.pktdata->key->packet_send_dest->sub_term->has_sub_term_uni =
                                                                            BAL_GRPC_PRES;
            balIndCfg.pktdata->key->packet_send_dest->sub_term->sub_term_uni =
                                rx_channel->key.packet_send_dest.u.sub_term.sub_term_uni;
            balIndCfg.pktdata->key->packet_send_dest->sub_term->has_intf_id =
                                                                            BAL_GRPC_PRES;
            balIndCfg.pktdata->key->packet_send_dest->sub_term->intf_id =
                                      rx_channel->key.packet_send_dest.u.sub_term.intf_id;
         }
         break;
         case BCMBAL_DEST_TYPE_SVC_PORT:
         {
            balIndCfg.pktdata->key->packet_send_dest->u_case = BAL_DEST__U_SVC_PORT;
            BalDestSvcPort balSvcPort;
            memset(&balSvcPort, 0, sizeof(BalDestSvcPort));
            bal_dest_svc_port__init(&balSvcPort);
            balIndCfg.pktdata->key->packet_send_dest->svc_port = &balSvcPort;
            balIndCfg.pktdata->key->packet_send_dest->svc_port->has_svc_port_id =
                                                                            BAL_GRPC_PRES;
            balIndCfg.pktdata->key->packet_send_dest->svc_port->svc_port_id =
                                 rx_channel->key.packet_send_dest.u.svc_port.svc_port_id;
            balIndCfg.pktdata->key->packet_send_dest->svc_port->has_intf_id =
                                                                            BAL_GRPC_PRES;
            balIndCfg.pktdata->key->packet_send_dest->svc_port->has_intf_id =
                                      rx_channel->key.packet_send_dest.u.svc_port.intf_id;
         }
         break;
         default:
         {
            balIndCfg.pktdata->key->packet_send_dest->u_case = BAL_DEST__U__NOT_SET;
         }
         break;
      }

      BalPacketBearerChannelRxData pkdData;
      memset(&pkdData, 0, sizeof(BalPacketBearerChannelRxData));
      bal_packet_bearer_channel_rx_data__init(&pkdData);
      balIndCfg.pktdata->data = &pkdData;

      balIndCfg.pktdata->data->has_flow_id = BAL_GRPC_PRES;
      balIndCfg.pktdata->data->flow_id = rx_channel->data.flow_id;
      balIndCfg.pktdata->data->has_flow_type = BAL_GRPC_PRES;
      balIndCfg.pktdata->data->flow_type = rx_channel->data.flow_type;
      balIndCfg.pktdata->data->has_intf_id = BAL_GRPC_PRES;
      balIndCfg.pktdata->data->intf_id = rx_channel->data.intf_id;
      balIndCfg.pktdata->data->has_intf_type = BAL_GRPC_PRES;
      balIndCfg.pktdata->data->intf_type = rx_channel->data.intf_type;
      balIndCfg.pktdata->data->has_svc_port = BAL_GRPC_PRES;
      balIndCfg.pktdata->data->svc_port = rx_channel->data.svc_port;
      balIndCfg.pktdata->data->has_flow_cookie = BAL_GRPC_PRES;
      balIndCfg.pktdata->data->flow_cookie = rx_channel->data.flow_cookie;
      balIndCfg.pktdata->data->has_pkt = BAL_GRPC_PRES;
      balIndCfg.pktdata->data->pkt.len = rx_channel->data.pkt.len;
      balIndCfg.pktdata->data->pkt.data = (uint8_t *)malloc((balIndCfg.pktdata->data->pkt.len)*sizeof(uint8_t));
      memcpy(balIndCfg.pktdata->data->pkt.data,  rx_channel->data.pkt.val, balIndCfg.pktdata->data->pkt.len);

      /*bcmos_mutex_unlock(&bal_ind_lock);-- Need to define bcm independent mutex*/

      BalErr *output;
      result = bal_ind__bal_pkt_bearer_channel_rx_ind(client, NULL, &balIndCfg, &output, NULL, 0);
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
      ASFVOLT_LOG(ASFVOLT_ERROR, "Processing BAL API \'%s\' IND callback (status is %s)\n",
				  bcmbal_objtype_str(obj->obj_type), bcmos_strerror(obj->status));
      result = BCM_ERR_INTERNAL;
   }
   else
   {
      ASFVOLT_LOG(ASFVOLT_DEBUG, "Received OMCI response via BAL APIs\n");
      BalIndications balIndCfg;
      memset(&balIndCfg, 0, sizeof(BalIndications));
      bal_indications__init(&balIndCfg);
      balIndCfg.u_case = BAL_INDICATIONS__U_BAL_OMCI_RESP;
      balIndCfg.has_objtype = BAL_GRPC_PRES;
      balIndCfg.objtype = obj->obj_type;
      balIndCfg.has_sub_group = BAL_GRPC_PRES;
      balIndCfg.sub_group = obj->subgroup;
      balIndCfg.device_id = voltha_device_id;

      BalPacketItuOmciChannelRx omciChannel;
      memset(&omciChannel, 0, sizeof(BalPacketItuOmciChannelRx));
      bal_packet_itu_omci_channel_rx__init(&omciChannel);
      balIndCfg.balomciresp = &omciChannel;

      BalObj hdr;
      memset(&hdr, 0, sizeof(BalObj));
      bal_obj__init(&hdr);
      balIndCfg.balomciresp->hdr = &hdr;

      BalPacketKey packetkey;
      memset(&packetkey, 0, sizeof(BalPacketKey));
      bal_packet_key__init(&packetkey);
      balIndCfg.balomciresp->key = &packetkey;

      BalPacketItuOmciChannelRxData omciData;
      memset(&omciData, 0, sizeof(BalPacketItuOmciChannelRxData));
      bal_packet_itu_omci_channel_rx_data__init(&omciData);
      balIndCfg.balomciresp->data = &omciData;

      bcmbal_packet_itu_omci_channel_rx *omci_channel =
                                  (bcmbal_packet_itu_omci_channel_rx *)obj;

      balIndCfg.balomciresp->data->has_pkt =  BAL_GRPC_PRES;
      balIndCfg.balomciresp->data->pkt.len =  omci_channel->data.pkt.len;
      balIndCfg.balomciresp->data->pkt.data = (uint8_t *)malloc((omci_channel->data.pkt.len)*sizeof(uint8_t));
      memcpy(balIndCfg.balomciresp->data->pkt.data, omci_channel->data.pkt.val,
             balIndCfg.balomciresp->data->pkt.len);

      balIndCfg.balomciresp->key->has_reserved = BAL_GRPC_PRES;
      balIndCfg.balomciresp->key->reserved = omci_channel->key.reserved;

      BalDest PktSndDest;
      memset(&PktSndDest, 0, sizeof(BalDest));
      bal_dest__init(&PktSndDest);
      balIndCfg.balomciresp->key->packet_send_dest = &PktSndDest;

      balIndCfg.balomciresp->key->packet_send_dest->has_type = BAL_GRPC_PRES;
      balIndCfg.balomciresp->key->packet_send_dest->type =
                                          omci_channel->key.packet_send_dest.type;
      switch( omci_channel->key.packet_send_dest.type)
      {
         case BCMBAL_DEST_TYPE_ITU_OMCI_CHANNEL:
         {
            balIndCfg.balomciresp->key->packet_send_dest->u_case = BAL_DEST__U_ITU_OMCI_CHANNEL;
            BalItuOmciChannel balOmci;
            memset(&balOmci, 0, sizeof(BalItuOmciChannel));
            bal_itu_omci_channel__init(&balOmci);
            balIndCfg.balomciresp->key->packet_send_dest->itu_omci_channel = &balOmci;
            balIndCfg.balomciresp->key->packet_send_dest->itu_omci_channel->has_sub_term_id =
                                                                            BAL_GRPC_PRES;
            balIndCfg.balomciresp->key->packet_send_dest->itu_omci_channel->sub_term_id =
                            omci_channel->key.packet_send_dest.u.itu_omci_channel.sub_term_id;
            balIndCfg.balomciresp->key->packet_send_dest->itu_omci_channel->has_intf_id =
                                                                            BAL_GRPC_PRES;
            balIndCfg.balomciresp->key->packet_send_dest->itu_omci_channel->intf_id =
                                 omci_channel->key.packet_send_dest.u.itu_omci_channel.intf_id;
            ASFVOLT_LOG(ASFVOLT_DEBUG, "OMCI Response for ONU id %d\n",
                 balIndCfg.balomciresp->key->packet_send_dest->itu_omci_channel->sub_term_id);
         }
         break;
         default:
         {
            balIndCfg.balomciresp->key->packet_send_dest->u_case = BAL_DEST__U__NOT_SET;
         }
         break;
      }

      ASFVOLT_LOG(ASFVOLT_DEBUG, "OMCI Response with %zd bytes is \n",balIndCfg.balomciresp->data->pkt.len);
      uint16_t idx;
      for(idx=0; idx<balIndCfg.balomciresp->data->pkt.len; idx++)
      {
	      printf("%02x", balIndCfg.balomciresp->data->pkt.data[idx]);
      }
      printf("\n");
      ASFVOLT_LOG(ASFVOLT_DEBUG, "OMCI Response for ONU id %d\n",
		      balIndCfg.balomciresp->key->packet_send_dest->itu_omci_channel->sub_term_id);
      BalErr *output;
      result = bal_ind__bal_pkt_omci_channel_rx_ind(client, NULL, &balIndCfg, &output, NULL, 0);
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
      ASFVOLT_LOG(ASFVOLT_ERROR, "Processing BAL API \'%s\' IND callback (status is %s)\n",
				  bcmbal_objtype_str(obj->obj_type), bcmos_strerror(obj->status));
      result = BCM_ERR_INTERNAL;
   }
   else
   {
      BalIndications balIndCfg;
      memset(&balIndCfg, 0, sizeof(BalIndications));
      bal_indications__init(&balIndCfg);
      balIndCfg.u_case = BAL_INDICATIONS__U_BAL_OAM_RESP;
      balIndCfg.has_objtype = BAL_GRPC_PRES;
      balIndCfg.objtype = obj->obj_type;
      balIndCfg.has_sub_group = BAL_GRPC_PRES;
      balIndCfg.sub_group = obj->subgroup;
      balIndCfg.device_id = voltha_device_id;

      BalPacketIeeeOamChannelRx oamChannel;
      memset(&oamChannel, 0, sizeof(BalPacketIeeeOamChannelRx));
      bal_packet_ieee_oam_channel_rx__init(&oamChannel);
      balIndCfg.baloamresp = &oamChannel;

      BalObj hdr;
      memset(&hdr, 0, sizeof(BalObj));
      bal_obj__init(&hdr);
      balIndCfg.baloamresp->hdr = &hdr;

      BalPacketKey packetkey;
      memset(&packetkey, 0, sizeof(BalPacketKey));
      bal_packet_key__init(&packetkey);
      balIndCfg.baloamresp->key = &packetkey;

      bcmbal_packet_ieee_oam_channel_rx *oam_channel =
                                  (bcmbal_packet_ieee_oam_channel_rx *)obj;

      balIndCfg.baloamresp->key->has_reserved = BAL_GRPC_PRES;
      balIndCfg.baloamresp->key->reserved = oam_channel->key.reserved;

      BalDest PktSndDest;
      memset(&PktSndDest, 0, sizeof(BalDest));
      bal_dest__init(&PktSndDest);
      balIndCfg.baloamresp->key->packet_send_dest = &PktSndDest;

      balIndCfg.baloamresp->key->packet_send_dest->has_type = BAL_GRPC_PRES;
      balIndCfg.baloamresp->key->packet_send_dest->type =
                                          oam_channel->key.packet_send_dest.type;
      switch( oam_channel->key.packet_send_dest.type)
      {
         case BCMBAL_DEST_TYPE_IEEE_OAM_CHANNEL:
         {
            balIndCfg.baloamresp->key->packet_send_dest->u_case = BAL_DEST__U_IEEE_OAM_CHANNEL;
            BalIeeeOamChannel balOam;
            memset(&balOam, 0, sizeof(BalIeeeOamChannel));
            bal_ieee_oam_channel__init(&balOam);
            balIndCfg.baloamresp->key->packet_send_dest->ieee_oam_channel = &balOam;
            balIndCfg.baloamresp->key->packet_send_dest->ieee_oam_channel->has_intf_id =
                                                                            BAL_GRPC_PRES;
            balIndCfg.baloamresp->key->packet_send_dest->ieee_oam_channel->intf_id =
                                 oam_channel->key.packet_send_dest.u.ieee_oam_channel.intf_id;
            balIndCfg.baloamresp->key->packet_send_dest->ieee_oam_channel->has_mac_address = BAL_GRPC_PRES;
            balIndCfg.baloamresp->key->packet_send_dest->ieee_oam_channel->mac_address.len =
               (BCMOS_ETH_ALEN)*sizeof(oam_channel->key.packet_send_dest.u.ieee_oam_channel.mac_address.u8);
            balIndCfg.baloamresp->key->packet_send_dest->ieee_oam_channel->mac_address.data =
             (uint8_t *)malloc((balIndCfg.baloamresp->key->packet_send_dest->ieee_oam_channel->mac_address.len)*sizeof(uint8_t));
            memcpy(balIndCfg.baloamresp->key->packet_send_dest->ieee_oam_channel->mac_address.data,
                   oam_channel->key.packet_send_dest.u.ieee_oam_channel.mac_address.u8,
                   balIndCfg.baloamresp->key->packet_send_dest->ieee_oam_channel->mac_address.len);

         }
         break;
         default:
         {
            balIndCfg.baloamresp->key->packet_send_dest->u_case = BAL_DEST__U__NOT_SET;
         }
         break;
      }

      BalPacketIeeeOamChannelRxData oamData;
      memset(&oamData, 0, sizeof(BalPacketIeeeOamChannelRxData));
      bal_packet_ieee_oam_channel_rx_data__init(&oamData);
      balIndCfg.baloamresp->data = &oamData;

      balIndCfg.baloamresp->data->pkt.len =  oam_channel->data.pkt.len;
      balIndCfg.baloamresp->data->pkt.data = (uint8_t *)malloc((balIndCfg.baloamresp->data->pkt.len)*sizeof(uint8_t));
      memcpy(balIndCfg.baloamresp->data->pkt.data, oam_channel->data.pkt.val,
             balIndCfg.baloamresp->data->pkt.len);

      BalErr *output;
      result = bal_ind__bal_pkt_ieee_oam_channel_rx_ind(client, NULL, &balIndCfg, &output, NULL, 0);

   }

   return result;
}
