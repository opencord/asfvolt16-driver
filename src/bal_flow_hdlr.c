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

#include <stdio.h>
#include <bal_api.h>

#undef _SYS_QUEUE_H_

#include "asfvolt16_driver.h"
#include "bal_flow_hdlr.h"


/********************************************************************\
 * Function    : bal_fill_queue_cfg                                 *
 * Description : copy queue information from proto structure to     *
 *               BAL structure                                      *
 ********************************************************************/
static uint32_t bal_fill_queue_cfg(BalTmQueueRef       *tmp_que, 
                                   bcmbal_tm_queue_ref *que_val)
{
   if(tmp_que->has_sched_id)
   {
      que_val->sched_id = tmp_que->sched_id; 
      ASFVOLT_LOG(ASFVOLT_DEBUG, "que_val->sched_id = %d\n", que_val->sched_id);
   }

   if(tmp_que->has_queue_id)
   {
      que_val->queue_id = tmp_que->queue_id;
      ASFVOLT_LOG(ASFVOLT_DEBUG, "que_val->queue_id = %d\n", que_val->queue_id);
   }

   return BAL_ERRNO__BAL_ERR_OK;
}
   

/********************************************************************\
 * Function    : bal_fill_action_cfg                                *
 * Description : copy action information from proto structure to    *
 *               BAL structure                                      *
 ********************************************************************/
static uint32_t bal_fill_action_cfg(BalAction     *tmp_action, 
                                    bcmbal_action *action_val)
{

   if(tmp_action->has_presence_mask)
   {
      action_val->presence_mask = tmp_action->presence_mask;
      if (action_val->presence_mask & BCMBAL_ACTION_ID_CMDS_BITMASK)
      {
         action_val->cmds_bitmask = tmp_action->cmds_bitmask;
         ASFVOLT_LOG(ASFVOLT_DEBUG, "action_val->cmds_bitmask = %x\n", action_val->cmds_bitmask);
      }

      if (action_val->presence_mask & BCMBAL_ACTION_ID_O_VID)
      {
         action_val->o_vid = tmp_action->o_vid;
         ASFVOLT_LOG(ASFVOLT_DEBUG, "action_val->o_vid = %d\n",  action_val->o_vid);
      }

      if (action_val->presence_mask & BCMBAL_ACTION_ID_O_PBITS)
      {
         action_val->o_pbits = tmp_action->o_pbits;
         ASFVOLT_LOG(ASFVOLT_DEBUG, "action_val->o_pbits = %d\n",  action_val->o_pbits);
      }

      if (action_val->presence_mask & BCMBAL_ACTION_ID_O_TPID)
      {
         action_val->o_tpid = tmp_action->o_tpid;
         ASFVOLT_LOG(ASFVOLT_DEBUG, "action_val->o_tpid = %d\n",  action_val->o_tpid);
      }

      if (action_val->presence_mask & BCMBAL_ACTION_ID_I_VID)
      {
         action_val->i_vid = tmp_action->i_vid;
         ASFVOLT_LOG(ASFVOLT_DEBUG, "action_val->i_vid = %d\n",  action_val->i_vid);
      }

      if (action_val->presence_mask & BCMBAL_ACTION_ID_I_PBITS)
      {
         action_val->i_pbits = tmp_action->i_pbits;
         ASFVOLT_LOG(ASFVOLT_DEBUG, "action_val->i_pbits = %d\n",  action_val->i_pbits);
      }

      if (action_val->presence_mask & BCMBAL_ACTION_ID_I_TPID)
      {
         action_val->i_tpid = tmp_action->i_tpid;
         ASFVOLT_LOG(ASFVOLT_DEBUG, "action_val->i_tpid = %d\n",  action_val->i_tpid);
      }
      ASFVOLT_LOG(ASFVOLT_DEBUG, "action_val->presence_mask = %x\n",  action_val->presence_mask);
   }
   else
   {
      ASFVOLT_LOG(ASFVOLT_ERROR, "Action is not present in the flow\n");
      return BAL_ERRNO__BAL_ERR_PARM;
   }

   return BAL_ERRNO__BAL_ERR_OK;
}


/********************************************************************\
 * Function    : bal_fill_classifier_cfg                            *
 * Description : copy classifier information from proto structure to*
 *               BAL structure
 ********************************************************************/
uint32_t bal_fill_classifier_cfg(BalClassifier     *tmp_classifier, 
                                        bcmbal_classifier *classifier_val)
{

   if(tmp_classifier->has_presence_mask)
   {
      classifier_val->presence_mask = tmp_classifier->presence_mask;
      if (classifier_val->presence_mask  & BCMBAL_CLASSIFIER_ID_O_TPID)
      {
         classifier_val->o_tpid = tmp_classifier->o_tpid;
         ASFVOLT_LOG(ASFVOLT_DEBUG, "classifier_val->o_tpid = %d\n",  classifier_val->o_tpid);
      }

      if (classifier_val->presence_mask & BCMBAL_CLASSIFIER_ID_O_VID)
      {
         classifier_val->o_vid = tmp_classifier->o_vid;
         ASFVOLT_LOG(ASFVOLT_DEBUG, "classifier_val->o_vid = %d\n",  classifier_val->o_vid);
      }

      if (classifier_val->presence_mask & BCMBAL_CLASSIFIER_ID_I_TPID)
      {
         classifier_val->i_tpid = tmp_classifier->i_tpid;
         ASFVOLT_LOG(ASFVOLT_DEBUG, "classifier_val->i_tpid = %d\n",  classifier_val->i_tpid);
      }

      if (classifier_val->presence_mask & BCMBAL_CLASSIFIER_ID_I_VID)
      {
         classifier_val->i_vid = tmp_classifier->i_vid;
         ASFVOLT_LOG(ASFVOLT_DEBUG, "classifier_val->i_vid = %d\n",  classifier_val->i_vid);
      }

      if (classifier_val->presence_mask & BCMBAL_CLASSIFIER_ID_O_PBITS)
      {
         classifier_val->o_pbits = tmp_classifier->o_pbits;
         ASFVOLT_LOG(ASFVOLT_DEBUG, "classifier_val->o_pbits = %d\n",  classifier_val->o_pbits);
      }

      if (classifier_val->presence_mask & BCMBAL_CLASSIFIER_ID_I_PBITS)
      {
         classifier_val->i_pbits = tmp_classifier->i_pbits;
         ASFVOLT_LOG(ASFVOLT_DEBUG, "classifier_val->i_pbits = %d\n",  classifier_val->i_pbits);
      }

      if (classifier_val->presence_mask & BCMBAL_CLASSIFIER_ID_ETHER_TYPE)
      {
         classifier_val->ether_type = tmp_classifier->ether_type;
         ASFVOLT_LOG(ASFVOLT_DEBUG, "classifier_val->ether_type = %x\n",  classifier_val->ether_type);
      }

      if (classifier_val->presence_mask & BCMBAL_CLASSIFIER_ID_DST_MAC)
      {
         if(tmp_classifier->dst_mac.len > BCMOS_ETH_ALEN)
         {
            memcpy(classifier_val->dst_mac.u8, 
                   tmp_classifier->dst_mac.data, tmp_classifier->dst_mac.len);
            /*ASFVOLT_LOG(ASFVOLT_DEBUG, "classifier_val->dst_mac= %d\n",  classifier_val->dst_mac);*/
         }
         else
         {
            ASFVOLT_LOG(ASFVOLT_ERROR, "dst_mac should not be more than 6 bytes."
                                        "actual bytes %lud\n",  tmp_classifier->dst_mac.len);
            return BAL_ERRNO__BAL_ERR_PARM;
         }
      }

      if (classifier_val->presence_mask & BCMBAL_CLASSIFIER_ID_SRC_MAC)
      {
         if(tmp_classifier->src_mac.len > BCMOS_ETH_ALEN)
         {
            memcpy(classifier_val->src_mac.u8, 
                   tmp_classifier->src_mac.data, tmp_classifier->src_mac.len);
            /*ASFVOLT_LOG(ASFVOLT_DEBUG, "classifier_val->src_mac= %x\n",  classifier_val->src_mac);*/
         }
         else
         {
            ASFVOLT_LOG(ASFVOLT_ERROR, "src_mac should not be more than 6 bytes."
                                        "actual bytes %lud\n",  tmp_classifier->src_mac.len);
            return BAL_ERRNO__BAL_ERR_PARM;
         }

      }

      if (classifier_val->presence_mask & BCMBAL_CLASSIFIER_ID_IP_PROTO)
      {
         classifier_val->ip_proto  = tmp_classifier->ip_proto;
         ASFVOLT_LOG(ASFVOLT_DEBUG, "classifier_val->ip_proto = %x\n",  classifier_val->ip_proto);
      }

      if (classifier_val->presence_mask & BCMBAL_CLASSIFIER_ID_DST_IP)
      {
         classifier_val->dst_ip.u32 = tmp_classifier->dst_ip;
         ASFVOLT_LOG(ASFVOLT_DEBUG, "classifier_val->dst_ip = %x\n",  classifier_val->dst_ip.u32);
      }

      if (classifier_val->presence_mask & BCMBAL_CLASSIFIER_ID_SRC_IP)
      {
         classifier_val->src_ip.u32 = tmp_classifier->src_ip;
         ASFVOLT_LOG(ASFVOLT_DEBUG, "classifier_val->src_ip = %x\n",  classifier_val->src_ip.u32);
      }

      if (classifier_val->presence_mask & BCMBAL_CLASSIFIER_ID_SRC_PORT)
      {
         classifier_val->src_port = tmp_classifier->src_port;
         ASFVOLT_LOG(ASFVOLT_DEBUG, "classifier_val->src_port = %d\n",  classifier_val->src_port);
      }

      if (classifier_val->presence_mask & BCMBAL_CLASSIFIER_ID_DST_PORT)
      {
         classifier_val->dst_port = tmp_classifier->dst_port;
         ASFVOLT_LOG(ASFVOLT_DEBUG, "classifier_val->dst_port = %d\n",  classifier_val->dst_port);
      }

      if (classifier_val->presence_mask & BCMBAL_CLASSIFIER_ID_PKT_TAG_TYPE)
      {
         classifier_val->pkt_tag_type = tmp_classifier->pkt_tag_type;
         ASFVOLT_LOG(ASFVOLT_DEBUG, "classifier_val->pkt_tag_type = %x\n",  classifier_val->pkt_tag_type);
      }

      ASFVOLT_LOG(ASFVOLT_DEBUG, "classifier_val->presence_mask = %x\n",  classifier_val->presence_mask);
   }
   else
   {
      ASFVOLT_LOG(ASFVOLT_ERROR, "Classifier is not present in the flow\n");
      return BAL_ERRNO__BAL_ERR_PARM;
   }

   return BAL_ERRNO__BAL_ERR_OK;
}

/********************************************************************\
 * Function    : bal_flow_cfg_req                                   *
 * Description : Configure the the OLT DEVICE with flow information *
 *               using BAL Apis                                     *
 ********************************************************************/
uint32_t bal_flow_cfg_set(BalFlowCfg *flow_cfg)
{
   bcmos_errno err = BCM_ERR_OK;
   BalErrno   bal_err = BAL_ERRNO__BAL_ERR_OK;
   bcmbal_flow_cfg cfg;        /**< declare main API struct */
   bcmbal_flow_key key = { 0, 0};  /**< declare key */
   bcmbal_classifier classifier_value = {};
   bcmbal_action action_value = {};
   bcmbal_tm_queue_ref queue_value = {};

   /* build key from CLI parameters */
   if(flow_cfg->key->has_flow_id)
   {
      key.flow_id = (bcmbal_flow_id) flow_cfg->key->flow_id;
   }
   else
   {
      ASFVOLT_LOG(ASFVOLT_ERROR, "Invalid Flow Id (%d)\n", flow_cfg->key->flow_id);
      return BAL_ERRNO__BAL_ERR_PARM;
   }


   ASFVOLT_LOG(ASFVOLT_INFO, "Adding the flow to OLT. FlowID(%d)\n", flow_cfg->key->flow_id);

   if (!flow_cfg->key->has_flow_type && 
         ((flow_cfg->key->flow_type < BAL_FLOW_TYPE__BAL_FLOW_TYPE_UPSTREAM) || 
         flow_cfg->key->flow_type > BAL_FLOW_TYPE__BAL_FLOW_TYPE_MULTICAST ))
   {
      ASFVOLT_LOG(ASFVOLT_ERROR, "Flow type is not supported (%d)\n", flow_cfg->key->flow_type);
      return BAL_ERRNO__BAL_ERR_PARM;
   }
   else
   {
      ASFVOLT_LOG(ASFVOLT_DEBUG, "flow_type = %d\n",  flow_cfg->key->flow_type);
      key.flow_type = (bcmbal_flow_type) flow_cfg->key->flow_type;
   }

   /* init the API struct */
   BCMBAL_CFG_INIT(&cfg, flow, key);

   /* decode API parameters from grpc-c */
   /* Admin State */
   ASFVOLT_CFG_PROP_SET(cfg, flow, admin_state, 
                        flow_cfg->data->has_admin_state, 
                        flow_cfg->data->admin_state);
   ASFVOLT_LOG(ASFVOLT_DEBUG, "admin state = %d\n",  flow_cfg->data->admin_state);

   /* Operational status */
   ASFVOLT_CFG_PROP_SET(cfg, flow, oper_status, 
                        flow_cfg->data->has_oper_status, 
                        flow_cfg->data->oper_status);
   ASFVOLT_LOG(ASFVOLT_DEBUG, "Oper. status = %d\n",  flow_cfg->data->oper_status);

   /* Access Side interface ID */
   ASFVOLT_CFG_PROP_SET(cfg, flow, access_int_id, 
                        BCMOS_TRUE, 
                        flow_cfg->data->access_int_id);
   ASFVOLT_LOG(ASFVOLT_DEBUG, "access_int_id = %d\n",  flow_cfg->data->access_int_id);

   /* Network interface ID */
   ASFVOLT_CFG_PROP_SET(cfg, flow, network_int_id, 
                        BCMOS_TRUE,
                        flow_cfg->data->network_int_id);
   ASFVOLT_LOG(ASFVOLT_DEBUG, "network_int_id = %d\n",  flow_cfg->data->network_int_id);

   /* ONU ID */
   if(BAL_FLOW_TYPE__BAL_FLOW_TYPE_MULTICAST != flow_cfg->key->flow_type)
   {
      ASFVOLT_CFG_PROP_SET(cfg, flow, sub_term_id, 
                           flow_cfg->data->has_sub_term_id, 
                           flow_cfg->data->sub_term_id);
      ASFVOLT_LOG(ASFVOLT_DEBUG, "sub_term_id = %d\n",  flow_cfg->data->sub_term_id);
   }
   else if (BAL_FLOW_TYPE__BAL_FLOW_TYPE_MULTICAST == flow_cfg->key->flow_type)
   {
      /* In case of Multicast the flow belongs to particular group */
      ASFVOLT_CFG_PROP_SET(cfg, flow, group_id, 
                            flow_cfg->data->has_group_id, 
                            flow_cfg->data->group_id);
      ASFVOLT_LOG(ASFVOLT_DEBUG, "group_id = %d\n",  flow_cfg->data->group_id);
   }

   /*Subscriber Terminal UNI index*/
   ASFVOLT_CFG_PROP_SET(cfg, flow, sub_term_uni_idx, 
                         flow_cfg->data->has_sub_term_uni_idx, 
                         flow_cfg->data->sub_term_uni_idx);
   ASFVOLT_LOG(ASFVOLT_DEBUG, "sub_term_uni_idx = %d\n",  flow_cfg->data->sub_term_uni_idx);

   /*Resolve MAC*/
   ASFVOLT_CFG_PROP_SET(cfg, flow, resolve_mac, 
                         flow_cfg->data->has_resolve_mac, 
                         flow_cfg->data->resolve_mac);
   ASFVOLT_LOG(ASFVOLT_DEBUG, "resolve_mac = %d\n",  flow_cfg->data->resolve_mac);

   /*Gem port*/
   ASFVOLT_CFG_PROP_SET(cfg, flow, svc_port_id, 
                         flow_cfg->data->has_svc_port_id, 
                         flow_cfg->data->svc_port_id);
   ASFVOLT_LOG(ASFVOLT_DEBUG, "svc_port_id = %d\n",  flow_cfg->data->svc_port_id);

   /*priority*/
   ASFVOLT_CFG_PROP_SET(cfg, flow, priority, 
                        flow_cfg->data->has_priority, 
                        flow_cfg->data->priority);
   ASFVOLT_LOG(ASFVOLT_DEBUG, "priority = %d\n",flow_cfg->data->priority);

   BalClassifier *tmp_classifier = (BalClassifier*)(flow_cfg->data->classifier);

   if (tmp_classifier != NULL)
   {
      bal_err = bal_fill_classifier_cfg(tmp_classifier, &classifier_value);

      if(bal_err != BAL_ERRNO__BAL_ERR_OK)
      {
         ASFVOLT_LOG(ASFVOLT_ERROR, 
               "Addition of flow %d failed while processing classifier\n",
               flow_cfg->key->flow_id);
         return bal_err;
      }

      ASFVOLT_CFG_PROP_SET(cfg, flow, classifier, BCMOS_TRUE, classifier_value);
   }
   else
   {
      ASFVOLT_LOG(ASFVOLT_DEBUG, "No Classifier for flow\n");
   }

   BalAction *tmp_action = (BalAction*)(flow_cfg->data->action);

   if (tmp_action != NULL)
   {
      bal_err = bal_fill_action_cfg(tmp_action, &action_value);

      if(bal_err != BAL_ERRNO__BAL_ERR_OK)
      {
         ASFVOLT_LOG(ASFVOLT_ERROR, 
               "Addition of flow %d failed while processing action\n",
               flow_cfg->key->flow_id);
         return bal_err;
      }
      ASFVOLT_CFG_PROP_SET(cfg, flow, action, BCMOS_TRUE, action_value);
   }
   else
   {
      ASFVOLT_LOG(ASFVOLT_DEBUG, "No Action for flow\n");
   }

   /*cookie*/
   ASFVOLT_CFG_PROP_SET(cfg, flow, cookie, 
                        flow_cfg->data->has_cookie, 
                        flow_cfg->data->cookie);
   ASFVOLT_LOG(ASFVOLT_DEBUG, "priority = %lx\n",flow_cfg->data->cookie);

   /*Egress queue*/
   BalTmQueueRef *tmp_que = (BalTmQueueRef*)(flow_cfg->data->queue);

   if (tmp_que != NULL)
   {
      bal_err = bal_fill_queue_cfg(tmp_que, &queue_value);

      if(bal_err != BAL_ERRNO__BAL_ERR_OK)
      {
         ASFVOLT_LOG(ASFVOLT_ERROR, 
               "Addition of flow %d failed while processing queue\n",
               flow_cfg->key->flow_id);
         return bal_err;
      }
      ASFVOLT_CFG_PROP_SET(cfg, flow, queue, BCMOS_TRUE, queue_value);
   }
   else
   {
      ASFVOLT_LOG(ASFVOLT_DEBUG, "No queue Information for flow");
   }
   /*dba_tm_sched_id*/
   ASFVOLT_CFG_PROP_SET(cfg, flow, dba_tm_sched_id, 
                        flow_cfg->data->has_dba_tm_sched_id, 
                        flow_cfg->data->dba_tm_sched_id);
   ASFVOLT_LOG(ASFVOLT_DEBUG, "tm_sched used for dba = %d\n",flow_cfg->data->dba_tm_sched_id);

   err = bcmbal_cfg_set(DEFAULT_ATERM_ID, &cfg.hdr);
   if(err != BCM_ERR_OK)
   {
        ASFVOLT_LOG(ASFVOLT_ERROR, 
              "Failed to configure the flow. FlowId(%d)\n", key.flow_id);
        return BAL_ERRNO__BAL_ERR_INTERNAL;
   }
   return BAL_ERRNO__BAL_ERR_OK;
}


/********************************************************************\
 * Function    : bal_flow_cfg_clear                                 *
 * Description : Deletes/Clears the OLT DEVICE with flow 
 *               information using BAL Apis                         *
 ********************************************************************/
uint32_t bal_flow_cfg_clear(BalFlowKey *flow_key)
{
   bcmos_errno err = BCM_ERR_OK;
   bcmbal_flow_cfg cfg;        /**< declare main API struct */
   bcmbal_flow_key key = { };  /**< declare key */

   if (flow_key->has_flow_id && flow_key->has_flow_type)
   {
      key.flow_id = flow_key->flow_id;
      key.flow_type = flow_key->flow_type;
   }
   else
   {
      ASFVOLT_LOG(ASFVOLT_DEBUG, "Invalid Flow Parameters to clear FlowId(%d), FlowType(%d)\n", 
                                  key.flow_id, key.flow_type);
      return BAL_ERRNO__BAL_ERR_PARM;
   }

   ASFVOLT_LOG(ASFVOLT_DEBUG, "FlowId(%d), FlowType(%d)\n", key.flow_id, key.flow_type);
 
   /* init the API struct */
   BCMBAL_CFG_INIT(&cfg, flow, key);

   /* call API */
   err = bcmbal_cfg_clear(DEFAULT_ATERM_ID, &cfg.hdr);
   if( err != BCM_ERR_OK)
   {
      ASFVOLT_LOG(ASFVOLT_INFO, 
          "Failed to clear the Flow. FlowID(%d) FlowType(%d)\n", 
                  key.flow_id, key.flow_type);
      return BAL_ERRNO__BAL_ERR_INTERNAL;
   }
   return BAL_ERRNO__BAL_ERR_OK;
} 


/********************************************************************\
 * Function    : bal_flow_cfg_get                                   *
 * Description : Get flow information from BAL.                     *
 ********************************************************************/
uint32_t bal_flow_cfg_get(BalFlowKey *flow_key, BalFlowCfg *flow_cfg)
{

   bcmos_errno err = BCM_ERR_OK;
   bcmbal_flow_cfg cfg;        /**< declare main API struct */
   bcmbal_flow_key key = { };  /**< declare key */

   if (flow_key->has_flow_id && flow_key->has_flow_type)
   {
      key.flow_id = flow_key->flow_id;
      key.flow_type = flow_key->flow_type;
   }
   else
   {
      ASFVOLT_LOG(ASFVOLT_DEBUG, "Invalid Flow Parameters to get cfg FlowId(%d), FlowType(%d)\n", 
                                  key.flow_id, key.flow_type);
      return BAL_ERRNO__BAL_ERR_PARM;
   }

   /* init the API struct */
   BCMBAL_CFG_INIT(&cfg, flow, key);

   BCMBAL_CFG_PROP_GET(&cfg, flow, all_properties);

   /* call API */
   err = bcmbal_cfg_get(DEFAULT_ATERM_ID, &cfg.hdr);
   if( err != BCM_ERR_OK)
   {
      ASFVOLT_LOG(ASFVOLT_INFO, 
            "Failed to get Flow Info. FlowID(%d) FlowType(%d)\n", 
            key.flow_id, key.flow_type);
      return BAL_ERRNO__BAL_ERR_INTERNAL;
   }

   ASFVOLT_LOG(ASFVOLT_INFO, 
                  "To-Do. Send Flow details to Adapter\n");

   return BAL_ERRNO__BAL_ERR_OK;
} 
