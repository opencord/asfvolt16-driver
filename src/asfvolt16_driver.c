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
#include <bcmos_system.h>
#include <bal_api.h>

#undef _SYS_QUEUE_H_

#include "asfvolt16_driver.h"
#include "bal_access_terminal_hdlr.h"
#include "bal_subscriber_terminal_hdlr.h"
#include "bal_flow_hdlr.h"
#include "bal_group_hdlr.h"
#include "bal_interface_hdlr.h"
#include "bal_tmqueue_hdlr.h"
#include "bal_tmsched_hdlr.h"
#include "bal_packet_hdlr.h"
#include "bal_indications_hdlr.h"

bcmbal_access_term_id access_term_id = DEFAULT_ATERM_ID;

/********************************************************************\
 * Function : bal_register_indication_cbs                           *
 * Description : Registering the Call back function to handle the   *
 *               indciations from BAL                               *
 ********************************************************************/
uint32_t bal_register_indication_cbs()
{
    bcmos_errno err = BCM_ERR_OK;
    bcmbal_cb_cfg cb_cfg = {};
    uint16_t ind_subgroup;
    ASFVOLT_LOG(ASFVOLT_DEBUG, "Subscription for other messages\n");

    cb_cfg.module = BCMOS_MODULE_ID_NONE;

    /* Register to get indications for interface objects
     */
    cb_cfg.obj_type = BCMBAL_OBJ_ID_INTERFACE;

    /* Interface los */
    cb_cfg.ind_cb_hdlr = (f_bcmbal_ind_handler)bal_interface_los_indication_cb;
    ind_subgroup = bcmbal_interface_auto_id_los;
    cb_cfg.p_subgroup = &ind_subgroup;
    err = err ? err : bcmbal_subscribe_ind(access_term_id, &cb_cfg);

    /* Interface Indication */
    cb_cfg.ind_cb_hdlr = (f_bcmbal_ind_handler)bal_interface_indication_cb;
    ind_subgroup = bcmbal_interface_auto_id_ind;
    cb_cfg.p_subgroup = &ind_subgroup;
    err = err ? err : bcmbal_subscribe_ind(access_term_id, &cb_cfg);

    /* Register to get indications for subscriber terminal objects
     */
    cb_cfg.obj_type = BCMBAL_OBJ_ID_SUBSCRIBER_TERMINAL;

    /* Subscriber Terminal Alarm */
    cb_cfg.ind_cb_hdlr = (f_bcmbal_ind_handler)bal_sub_term_alarm_indication_cb;
    ind_subgroup = bcmbal_subscriber_terminal_auto_id_sub_term_alarm;
    cb_cfg.p_subgroup = &ind_subgroup;
    err = err ? err : bcmbal_subscribe_ind(access_term_id, &cb_cfg);

    /* Subscriber Terminal Discovery */
    cb_cfg.ind_cb_hdlr = (f_bcmbal_ind_handler)bal_sub_term_disc_indication_cb;
    ind_subgroup = bcmbal_subscriber_terminal_auto_id_sub_term_disc;
    cb_cfg.p_subgroup = &ind_subgroup;
    err = err ? err : bcmbal_subscribe_ind(access_term_id, &cb_cfg);

    /* Subscriber Terminal Indication */
    cb_cfg.ind_cb_hdlr = (f_bcmbal_ind_handler)bal_sub_term_indication_cb;
    ind_subgroup = bcmbal_subscriber_terminal_auto_id_ind;
    cb_cfg.p_subgroup = &ind_subgroup;
    err = err ? err : bcmbal_subscribe_ind(access_term_id, &cb_cfg);

    /* Register to get indication callbacks for OMCI objects
     */
    cb_cfg.obj_type = BCMBAL_OBJ_ID_PACKET;

    ind_subgroup = BCMBAL_IND_SUBGROUP(packet, itu_omci_channel_rx);
    cb_cfg.p_object_key_info = NULL;
    cb_cfg.p_subgroup = &ind_subgroup;
    cb_cfg.ind_cb_hdlr = (f_bcmbal_ind_handler )bal_omci_data_indication_cb;
    bcmbal_subscribe_ind(0, &cb_cfg);

#if 0
    /* Access Terminal Operational State Change */
    cb_cfg.ind_cb_hdlr = (f_bcmbal_ind_handler)bal_acc_term_osc_indication_cb;
    ind_subgroup = bcmbal_access_terminal_auto_id_oper_status_change;
    cb_cfg.p_subgroup = &ind_subgroup;
    err = err ? err : bcmbal_subscribe_ind(access_term_id, &cb_cfg);

    /* Register to get indication callbacks for flow objects
     */
    cb_cfg.obj_type = BCMBAL_OBJ_ID_FLOW;

    /* Flow Operational State Change */
    cb_cfg.ind_cb_hdlr = (f_bcmbal_ind_handler)bal_flow_osc_indication_cb;
    ind_subgroup = bcmbal_flow_auto_id_oper_status_change;
    cb_cfg.p_subgroup = &ind_subgroup;
    err = bcmbal_subscribe_ind(access_term_id, &cb_cfg);

    /* Flow Indication */
    cb_cfg.ind_cb_hdlr = (f_bcmbal_ind_handler)bal_flow_indication_cb;
    ind_subgroup = bcmbal_flow_auto_id_ind;
    cb_cfg.p_subgroup = &ind_subgroup;
    err = bcmbal_subscribe_ind(access_term_id, &cb_cfg);

    /* Register to get indication callbacks for group objects
     */
    cb_cfg.obj_type = BCMBAL_OBJ_ID_GROUP;
    cb_cfg.ind_cb_hdlr = (f_bcmbal_ind_handler)bal_group_indication_cb;
    ind_subgroup = bcmbal_group_auto_id_ind;
    cb_cfg.p_subgroup = &ind_subgroup;
    err = err ? err : bcmbal_subscribe_ind(access_term_id, &cb_cfg);
    
    /* Interface Operational State Change */
    cb_cfg.ind_cb_hdlr = (f_bcmbal_ind_handler)bal_interface_osc_indication_cb;
    ind_subgroup = bcmbal_interface_auto_id_oper_status_change;
    cb_cfg.p_subgroup = &ind_subgroup;
    err = err ? err : bcmbal_subscribe_ind(access_term_id, &cb_cfg);

    /* Interface los */
    cb_cfg.ind_cb_hdlr = (f_bcmbal_ind_handler)bal_interface_los_indication_cb;
    ind_subgroup = bcmbal_interface_auto_id_los;
    cb_cfg.p_subgroup = &ind_subgroup;
    err = err ? err : bcmbal_subscribe_ind(access_term_id, &cb_cfg);

    /* Subscriber Terminal Operational State Change */
    cb_cfg.ind_cb_hdlr = (f_bcmbal_ind_handler)bal_sub_term_osc_indication_cb;
    ind_subgroup = bcmbal_subscriber_terminal_auto_id_oper_status_change;
    cb_cfg.p_subgroup = &ind_subgroup;
    err = err ? err : bcmbal_subscribe_ind(access_term_id, &cb_cfg);

    /* Subscriber Terminal dgi */
    cb_cfg.ind_cb_hdlr = (f_bcmbal_ind_handler)bal_sub_term_dgi_indication_cb;
    ind_subgroup = bcmbal_subscriber_terminal_auto_id_dgi;
    cb_cfg.p_subgroup = &ind_subgroup;
    err = err ? err : bcmbal_subscribe_ind(access_term_id, &cb_cfg);

    /* Register to get indication callbacks for tm queue objects
     */
    cb_cfg.obj_type = BCMBAL_OBJ_ID_TM_QUEUE;
    cb_cfg.ind_cb_hdlr = (f_bcmbal_ind_handler)bal_tm_queue_indication_cb;
    ind_subgroup = bcmbal_tm_queue_auto_id_ind;
    cb_cfg.p_subgroup = &ind_subgroup;
    err = err ? err : bcmbal_subscribe_ind(access_term_id, &cb_cfg);

    /* Register to get indication callbacks for tm sched objects
     */
    cb_cfg.obj_type = BCMBAL_OBJ_ID_TM_SCHED;
    cb_cfg.ind_cb_hdlr = (f_bcmbal_ind_handler)bal_tm_sched_indication_cb;
    ind_subgroup = bcmbal_tm_sched_auto_id_ind;
    cb_cfg.p_subgroup = &ind_subgroup;
    err = err ? err : bcmbal_subscribe_ind(access_term_id, &cb_cfg);
    
    /* Bearer Channel Data */
    cb_cfg.ind_cb_hdlr = (f_bcmbal_ind_handler)bal_packet_data_indication_cb;
    ind_subgroup = bcmbal_packet_auto_id_bearer_channel_rx;
    cb_cfg.p_subgroup = &ind_subgroup;
    err = err ? err : bcmbal_subscribe_ind(access_term_id, &cb_cfg);
    
    /* OAM Channel Data - oam response indication */
    cb_cfg.ind_cb_hdlr = (f_bcmbal_ind_handler)bal_oam_data_indication_cb;
    ind_subgroup = bcmbal_packet_auto_id_ieee_oam_channel_rx;
    cb_cfg.p_subgroup = &ind_subgroup;
    err = err ? err : bcmbal_subscribe_ind(access_term_id, &cb_cfg);
    
#endif
    return err;
}


/********************************************************************
 *                                                                  *
 *     gRPC service RPC function implementation                     *
 *                                                                  *   
 ********************************************************************/

/********************************************************************\
 * Function    : asfvolt16_bal_init                                 *
 * Description : This function will initialize the BAL module       *
 ********************************************************************/
uint32_t asfvolt16_bal_init(BalInit *bal_init, balCoreIpInfo *coreInfo)

{
    bcmos_errno err = BCM_ERR_OK;

    char *ip_and_port = NULL;
    if (NULL == bal_init)
    {
       ASFVOLT_LOG(ASFVOLT_ERROR, "Recvied NULL balInit Structure from VOLTHA \n");
       return BAL_ERRNO__BAL_ERR_PERM;
    }
    else
    {
       ip_and_port = bal_init->voltha_adapter_ip_port;
       ASFVOLT_LOG(ASFVOLT_DEBUG,"\nRecevied Adapter IP and Port from VOLTHA is %s\n",ip_and_port);
    }
    char *argv[6];
    /* Initialize BAL */
    argv[1] = coreInfo->bal_core_arg1;
    argv[2] = coreInfo->bal_core_ip_port;
    argv[3] = coreInfo->bal_core_arg2;
    argv[4] = coreInfo->bal_shared_lib_ip_port;
    int argc = 5; 
    client = grpc_c_client_init(ip_and_port, "bal_client", NULL);

    /* Init BAL */
    err = bcmbal_apiend_init_all(argc, argv, NULL,3);
    if(err != BCM_ERR_OK)
    {
      printf("\n Failed in bcmbal_init \n");
    }

#if 0
    err = (err != BCM_ERR_OK) ? err : bcmbal_autostart(); /* Execute bal_autostart.ini script if any */

    if (err)
    {
        /* Let logger task have enough time to drain its message queue. */
        usleep(1000000);
        ASFVOLT_LOG(ASFVOLT_ERROR, "failed to Initialize the BAL\n");
        return err;
    }
#endif

    /* Register the call back functions to handle any 
     * indications from the BAL */
    bcmbal_cb_cfg cb_cfg = {};
    uint16_t ind_subgroup;

    cb_cfg.module = BCMOS_MODULE_ID_NONE;
    /* Register to get indications for access terminal objects
     */
    cb_cfg.obj_type = BCMBAL_OBJ_ID_ACCESS_TERMINAL;

    /* Access Terminal Indication */
    cb_cfg.ind_cb_hdlr = (f_bcmbal_ind_handler)bal_acc_term_indication_cb;
    ind_subgroup = bcmbal_access_terminal_auto_id_ind;
    cb_cfg.p_subgroup = &ind_subgroup;
    err = err ? err : bcmbal_subscribe_ind(access_term_id, &cb_cfg);

    if (err)
    {
      ASFVOLT_LOG(ASFVOLT_ERROR, "failed to register call back functions to BAL\n");
    }

    return err;
}

/********************************************************************\
 * Function    : asfvolt16_bal_finish                               *
 * Description : This function will Un-initialize the BAL module    *
 ********************************************************************/
uint32_t asfvolt16_bal_finish(void)
{
    bcmos_errno err = BCM_ERR_OK;

    /* Un-Initialize the BAL function */
    bcmbal_apiend_finish();

    return err;
}


/********************************************************************\
 * Function    : asfvolt16_bal_cfg_set                              *
 * Description : Handles below configuration                        *
 *               1) Access Terminal Cfg                             *
 *               2) Interface(PON & NNI) Cfg                        *
 *               3) Subscriber Terminal (ONU) cfg                   *
 *               4) Flow Cfg                                        *             
 *               5) Group Cfg (In case of Multicast)                *
 ********************************************************************/
uint32_t asfvolt16_bal_cfg_set(BalCfg *cfg)
{
    bcmos_errno err = BCM_ERR_OK;
    memset(&voltha_device_id, 0, ASFVOLT_MAX_DEVICE_ID_SIZE);
    strcpy(voltha_device_id,cfg->device_id);

    switch(cfg->hdr->obj_type)
    {
       case BAL_OBJ_ID__BAL_OBJ_ID_ACCESS_TERMINAL:
       {
          err = bal_access_terminal_cfg_set(cfg->cfg);
          break;
       }
       case BAL_OBJ_ID__BAL_OBJ_ID_INTERFACE:
       {
          err = bal_interface_cfg_set(cfg->interface);
          break;
       }
       case BAL_OBJ_ID__BAL_OBJ_ID_SUBSCRIBER_TERMINAL:
       {
          err = bal_subscriber_terminal_cfg_set(cfg->terminal);
          break;
       }
       case BAL_OBJ_ID__BAL_OBJ_ID_FLOW:
       {
          err = bal_flow_cfg_set(cfg->flow);
          break;
       }
       case BAL_OBJ_ID__BAL_OBJ_ID_GROUP:
       {
          err = bal_group_cfg_set(cfg->group);
          break;
       }
       case BAL_OBJ_ID__BAL_OBJ_ID_TM_QUEUE:
       {
          err = bal_tm_queue_cfg_set(cfg->tm_queue_cfg);
          break;
       }
       case BAL_OBJ_ID__BAL_OBJ_ID_TM_SCHED:
       {
          err = bal_tm_sched_cfg_set(cfg->tm_sched_cfg);
          break;
       }
       case BAL_OBJ_ID__BAL_OBJ_ID_PACKET:
       {
          err = bal_packet_cfg_req(cfg->packet);
          break;
       }
       default:
       {
          ASFVOLT_LOG(ASFVOLT_ERROR, "Invalid objet type for configuration\n");
          err = BAL_ERRNO__BAL_ERR_INVALID_OP;
          break;
       }
    }
    return err;
}

/********************************************************************\
 * Function    : asfvolt16_bal_cfg_clear                            *
 * Description : Clears the configuration related to below objects  *
 *               1) Access Terminal Cfg                             *
 *               2) Interface(PON & NNI) Cfg                        *
 *               3) Subscriber Terminal (ONU) cfg                   *
 *               4) Flow Cfg                                        *             
 *               5) Group Cfg (In case of Multicast)                *
 ********************************************************************/
uint32_t asfvolt16_bal_cfg_clear(BalKey *key)
{
    bcmos_errno err = BCM_ERR_OK;

    if(!key->hdr->has_obj_type)
    {
       ASFVOLT_LOG(ASFVOLT_ERROR, "object type is not present for clear\n");
       return BAL_ERRNO__BAL_ERR_INVALID_OP;
    }

    switch(key->hdr->obj_type)
    {
       case BAL_OBJ_ID__BAL_OBJ_ID_ACCESS_TERMINAL:
       {
          err = bal_access_terminal_cfg_clear(key->access_term_key);
          break;
       }
       case BAL_OBJ_ID__BAL_OBJ_ID_INTERFACE:
       {
          err = bal_interface_cfg_clear(key->interface_key);
          break;
       }
       case BAL_OBJ_ID__BAL_OBJ_ID_SUBSCRIBER_TERMINAL:
       {
          err = bal_subscriber_terminal_cfg_clear(key->terminal_key);
          break;
       }
       case BAL_OBJ_ID__BAL_OBJ_ID_FLOW:
       {
          err = bal_flow_cfg_clear(key->flow_key);
          break;
       }
       case BAL_OBJ_ID__BAL_OBJ_ID_GROUP:
       {
          break;
       }
       case BAL_OBJ_ID__BAL_OBJ_ID_TM_QUEUE:
       {
          err = bal_tm_queue_cfg_clear(key->tm_queue_key);
          break;
       }
       case BAL_OBJ_ID__BAL_OBJ_ID_TM_SCHED:
       {
          err = bal_tm_sched_cfg_clear(key->tm_sched_key);
          break;
       }
       /*case BAL_KEY__OBJ_PACKET_KEY:
       {
          break;
       }*/
       default:
       {
          ASFVOLT_LOG(ASFVOLT_ERROR, "Invalid objet type to handle clear\n");
          err = BAL_ERRNO__BAL_ERR_INVALID_OP;
          break;
       }
    }
    return err;
}


/********************************************************************\
 * Function    : asfvolt16_bal_cfg_get                            *
 * Description : Get the configuration related to below objects  *
 *               1) Access Terminal Cfg                             *
 *               2) Interface(PON & NNI) Cfg                        *
 *               3) Subscriber Terminal (ONU) cfg                   *
 *               4) Flow Cfg                                        *             
 *               5) Group Cfg (In case of Multicast)                *
 ********************************************************************/
uint32_t asfvolt16_bal_cfg_get(BalKey *key, BalCfg *cfg)
{
    bcmos_errno err = BCM_ERR_OK;

    if(!key->hdr->has_obj_type)
    {
       ASFVOLT_LOG(ASFVOLT_ERROR, "object type is not present for get\n");
       return BAL_ERRNO__BAL_ERR_INVALID_OP;
    }

    switch(key->hdr->obj_type)
    {
       case BAL_OBJ_ID__BAL_OBJ_ID_ACCESS_TERMINAL:
       {
          err = bal_access_terminal_cfg_get(key->access_term_key, cfg->cfg );
          break;
       }
       case BAL_OBJ_ID__BAL_OBJ_ID_INTERFACE:
       {
          err = bal_interface_cfg_get(key->interface_key, cfg->interface);
          break;
       }
       case BAL_OBJ_ID__BAL_OBJ_ID_SUBSCRIBER_TERMINAL:
       {
          err = bal_subscriber_terminal_cfg_get(key->terminal_key, cfg->terminal);
          break;
       }
       case BAL_OBJ_ID__BAL_OBJ_ID_FLOW:
       {
          err = bal_flow_cfg_get(key->flow_key, cfg->flow);
          break;
       }
       case BAL_OBJ_ID__BAL_OBJ_ID_GROUP:
       {
          err = bal_group_cfg_get(key->group_key, cfg->group);
          break;
       }
       case BAL_OBJ_ID__BAL_OBJ_ID_TM_QUEUE:
       {
          err = bal_tm_queue_cfg_get(key->tm_queue_key, cfg->tm_queue_cfg);
          break;
       }
       case BAL_OBJ_ID__BAL_OBJ_ID_TM_SCHED:
       {
          err = bal_tm_sched_cfg_get(key->tm_sched_key, cfg->tm_sched_cfg);
          break;
       }
       case BAL_OBJ_ID__BAL_OBJ_ID_PACKET:
       {
          break;
       }
       default:
       {
          ASFVOLT_LOG(ASFVOLT_ERROR, "Invalid objet type to handle Get\n");
          err = BAL_ERRNO__BAL_ERR_INVALID_OP;
          break;
       }
    }
    return err;
}


