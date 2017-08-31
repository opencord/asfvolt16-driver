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

/* Includes related to proto buf */
#include "asfvolt16_driver.h"
#include "bal_tmsched_hdlr.h"

/********************************************************************\
 * Function    : bal_tm_sched_cfg_set                               *
 * Description : Configures the tm scheduler for OLT device         *
 ********************************************************************/

uint32_t bal_tm_sched_cfg_set(BalTmSchedCfg *tm_sched_cfg)
{
    bcmos_errno err = BCM_ERR_OK;
    bcmbal_tm_sched_cfg tm_sched_obj;   /**< declare main API struct */
    bcmbal_tm_sched_key key = { };      /**< declare key */
    
    if((tm_sched_cfg->key->has_dir) && (tm_sched_cfg->key->has_id))
    {
       key.dir = tm_sched_cfg->key->dir;
       key.id = tm_sched_cfg->key->id;
    }
    else
    {
       ASFVOLT_LOG(ASFVOLT_ERROR, "Failed to configure the tm schedule cfg(OLT): Missing Key values\
                                   Recevied key values Sched-Dir(%d), Sched-Id(%d) \n", 
                                   tm_sched_cfg->key->dir, tm_sched_cfg->key->id);
       return BAL_ERRNO__BAL_ERR_NOENT;
    }

    ASFVOLT_LOG(ASFVOLT_DEBUG, "Configuration of OLT(tm Sched) starts\n");
    /* init the API struct */
    BCMBAL_CFG_INIT(&tm_sched_obj, tm_sched, key);

    /* decode API parameters from GRPC */
    /* scheduler owner */
    BalTmSchedOwner *tmScOwn = (BalTmSchedOwner *)tm_sched_cfg->data->owner;
    bcmbal_tm_sched_owner valtmScOwn = {};
    if(tmScOwn->has_type)
    {
       valtmScOwn.type = tmScOwn->type;
       switch(valtmScOwn.type)
       {
           case BCMBAL_TM_SCHED_OWNER_TYPE_INTERFACE:
              if(tmScOwn->interface->has_intf_type)
              {
                 valtmScOwn.u.interface.intf_type = tmScOwn->interface->intf_type;
              }
              if(tmScOwn->interface->has_intf_id)
              {
                 valtmScOwn.u.interface.intf_id = tmScOwn->interface->intf_id;
              }
              break;
           case BCMBAL_TM_SCHED_OWNER_TYPE_SUB_TERM:
              if(tmScOwn->sub_term->has_intf_id)
              {
                 valtmScOwn.u.sub_term.intf_id = tmScOwn->sub_term->intf_id;
              }
              if(tmScOwn->sub_term->has_sub_term_id)
              {
                 valtmScOwn.u.sub_term.sub_term_id = tmScOwn->sub_term->sub_term_id;
              }
              break;
           case BCMBAL_TM_SCHED_OWNER_TYPE_AGG_PORT:
              if(tmScOwn->agg_port->has_presence_mask)
              {
                 valtmScOwn.u.agg_port.presence_mask = tmScOwn->agg_port->presence_mask;
                 if(tmScOwn->agg_port->has_intf_id)
                 {
                    valtmScOwn.u.agg_port.intf_id= tmScOwn->agg_port->intf_id;
                 }
                 if(tmScOwn->agg_port->has_sub_term_id)
                 {
                    valtmScOwn.u.agg_port.sub_term_id = tmScOwn->agg_port->sub_term_id;
                 }
                 if(tmScOwn->agg_port->has_agg_port_id)
                 {
                    valtmScOwn.u.agg_port.agg_port_id = tmScOwn->agg_port->agg_port_id;
                 }
              }
              break;
           case BCMBAL_TM_SCHED_OWNER_TYPE_UNI:
              if(tmScOwn->uni->has_intf_id)
              {
                 valtmScOwn.u.uni.intf_id = tmScOwn->uni->intf_id;
              }
              if(tmScOwn->uni->has_sub_term_id)
              {
                 valtmScOwn.u.uni.sub_term_id = tmScOwn->uni->sub_term_id;
              }
              if(tmScOwn->uni->has_idx)
              {
                 valtmScOwn.u.uni.idx = tmScOwn->uni->idx;
              }
              break;
           case BCMBAL_TM_SCHED_OWNER_TYPE_VIRTUAL:
              if(tmScOwn->virtual_->has_idx)
              {
                 valtmScOwn.u.virtual.idx = tmScOwn->virtual_->idx;
              }
              break;
           default:
              break;
       }
       ASFVOLT_CFG_PROP_SET(tm_sched_obj, tm_sched, owner, BCMOS_TRUE, valtmScOwn);
    }

    ASFVOLT_CFG_PROP_SET(tm_sched_obj, tm_sched, sched_type, 
                         tm_sched_cfg->data->has_sched_type, 
                         tm_sched_cfg->data->sched_type);

    /* scheduler parent */
    BalTmSchedParent *schedPar = (BalTmSchedParent *)tm_sched_cfg->data->sched_parent;
    bcmbal_tm_sched_parent valSchedPar = {};
    if(schedPar != NULL && schedPar->has_presence_mask)
    {
       valSchedPar.presence_mask = schedPar->presence_mask;
       if(schedPar->has_sched_id)
       {
          valSchedPar.sched_id = schedPar->sched_id;
       } 
       if(schedPar->has_priority)
       {
          valSchedPar.priority = schedPar->priority;
       } 
       if(schedPar->has_weight)
       {
          valSchedPar.weight = schedPar->weight;
       } 
       ASFVOLT_CFG_PROP_SET(tm_sched_obj, tm_sched, sched_parent, BCMOS_TRUE, valSchedPar);
    }
     
    ASFVOLT_CFG_PROP_SET(tm_sched_obj, tm_sched, sched_child_type, 
                         tm_sched_cfg->data->has_sched_child_type, 
                         tm_sched_cfg->data->sched_child_type);
   
    /* rating/shaping */
    BalTmShaping *balShaping = (BalTmShaping *)tm_sched_cfg->data->rate;
    bcmbal_tm_shaping val = {};
    if (balShaping != NULL && balShaping->has_presence_mask)
    {
       val.presence_mask = balShaping->presence_mask;
       if (balShaping->has_cir)
       {
          val.cir = balShaping->cir;
       }
       if (balShaping->has_pir)
       {
          val.pir = balShaping->pir;
       }
       if (balShaping->has_burst)
       {
          val.burst = balShaping->burst;
       }
       ASFVOLT_CFG_PROP_SET(tm_sched_obj, tm_sched, rate, BCMOS_TRUE, val);
    }

    /* Extended itu dba parameters */
    BalExtendedItuDba *tItuDba = (BalExtendedItuDba *)tm_sched_cfg->data->ext_itu_dba;
    bcmbal_extended_itu_dba valItuDda = {}; 
    if (tItuDba != NULL && tItuDba->has_presence_mask)
    {
       valItuDda.presence_mask = tItuDba->presence_mask;
       if(tItuDba->has_extra_bw_elig)
       {
          valItuDda.extra_bw_elig = tItuDba->extra_bw_elig;
       }
       if (tItuDba->has_nrt_cbr)
       {
          valItuDda.nrt_cbr = tItuDba->nrt_cbr;
       }
       if (tItuDba->has_rt_cbr)
       {
          valItuDda.rt_cbr = tItuDba->rt_cbr;
       }
       if (tItuDba->has_nrt_profile)
       {
          valItuDda.nrt_profile = tItuDba->nrt_profile;
       }
       if (tItuDba->has_rt_profile)
       {
          valItuDda.rt_profile = tItuDba->rt_profile;
       }
       ASFVOLT_CFG_PROP_SET(tm_sched_obj, tm_sched, ext_itu_dba, BCMOS_TRUE, valItuDda);
    }

    /* Creation mode */
    ASFVOLT_CFG_PROP_SET(tm_sched_obj, tm_sched, creation_mode, 
                         tm_sched_cfg->data->has_creation_mode, 
                         tm_sched_cfg->data->creation_mode);

    /* Extended epon dba parameters */
    BalExtendedEponDba *teponDba = (BalExtendedEponDba*)tm_sched_cfg->data->ext_epon_dba;
    bcmbal_extended_epon_dba valeponDda = {}; 
    if (teponDba != NULL && teponDba->has_presence_mask)
    {
       valeponDda.presence_mask = teponDba->presence_mask;
       if (teponDba->has_polling_interval_us)
       {
          valeponDda.polling_interval_us = teponDba->polling_interval_us;
       }
       if (teponDba->has_grant_threshold_tq)
       {
          valeponDda.grant_threshold_tq = teponDba->grant_threshold_tq;
       }
       if (teponDba->has_cir_priority)
       {
          valeponDda.cir_priority = teponDba->cir_priority;
       }
       if (teponDba->has_cir_weight_tq)
       {
          valeponDda.cir_weight_tq = teponDba->cir_weight_tq;
       }
       if (teponDba->has_pir_priority)
       {
          valeponDda.pir_priority = teponDba->pir_priority;
       }
       if (teponDba->has_pir_weight_tq)
       {
          valeponDda.pir_weight_tq = teponDba->pir_weight_tq;
       }
       if (teponDba->has_tdm_grant_size_tq)
       {
          valeponDda.tdm_grant_size_tq = teponDba->tdm_grant_size_tq;
       }
       if (teponDba->has_tdm_grant_interval_us)
       {
          valeponDda.tdm_grant_interval_us = teponDba->tdm_grant_interval_us;
       }
       ASFVOLT_CFG_PROP_SET(tm_sched_obj, tm_sched, ext_epon_dba, BCMOS_TRUE, valeponDda);
    }

    /* Subsidiary queues */
    BalIdList *balQueues = (BalIdList *)tm_sched_cfg->data->queues;
    bcmbal_tm_queue_id_list_u8 valQueues = {};
    if(balQueues != NULL && balQueues->n_val)
    {
       valQueues.len = balQueues->n_val;
       valQueues.val = (bcmbal_tm_queue_id *)malloc((valQueues.len)*sizeof(bcmbal_tm_queue_id));
       if(!valQueues.val)
       {
          ASFVOLT_LOG(ASFVOLT_ERROR, 
                     "Failed to configure the tm scheduler cfg(OLT): Memory Exhausted\n");
          return BAL_ERRNO__BAL_ERR_NOMEM;
       }
       memcpy((void *)valQueues.val, (const void *)balQueues->val, 
              (balQueues->n_val)*sizeof(uint32_t));
       ASFVOLT_CFG_PROP_SET(tm_sched_obj, tm_sched, queues, BCMOS_TRUE, valQueues); 
    }

    /* Subsidiary schedulers */
    BalIdList *balSubScheds = (BalIdList *)tm_sched_cfg->data->sub_scheds;
    bcmbal_tm_sched_id_list_u8 valSubScheds = {};
    if(balSubScheds != NULL && balSubScheds->n_val)
    {
       valSubScheds.len = balSubScheds->n_val;
       valSubScheds.val = (uint32_t *)malloc((valSubScheds.len)*sizeof(uint32_t));
       if(!valSubScheds.val)
       {
          ASFVOLT_LOG(ASFVOLT_ERROR, 
                     "Failed to configure the tm scheduler cfg(OLT): Memory Exhausted\n");
          return BAL_ERRNO__BAL_ERR_NOMEM;
       }
       memcpy((void *)valSubScheds.val, (const void *)balSubScheds->val, 
              (balSubScheds->n_val)*sizeof(uint32_t));
       ASFVOLT_CFG_PROP_SET(tm_sched_obj, tm_sched, sub_scheds, BCMOS_TRUE, valSubScheds); 
    }

    ASFVOLT_CFG_PROP_SET(tm_sched_obj, tm_sched, num_priorities, 
                         tm_sched_cfg->data->has_num_priorities, 
                         tm_sched_cfg->data->num_priorities);

    err = bcmbal_cfg_set(DEFAULT_ATERM_ID, &(tm_sched_obj.hdr));

    if(BCM_ERR_OK != err)
    {
        ASFVOLT_LOG(ASFVOLT_ERROR, "Failed to configure the tm scheduler Cfg(OLT)\n");
        return err;
    }

    ASFVOLT_LOG(ASFVOLT_INFO, "Set tm scheduler configuration sent to OLT. \
                               Sched ID(%d) Sched Dir(%d)\n", key.id, key.dir);
    return err;
}


/********************************************************************\
 * Function    : bal_tm_sched_cfg_get                               *
 * Description : get the OLT device tm queue configuration          *
 ********************************************************************/

uint32_t bal_tm_sched_cfg_get(BalTmSchedKey *tm_sched_key, BalTmSchedCfg *tm_sched_cfg)
{
    bcmos_errno err = BCM_ERR_OK;
    bcmbal_tm_sched_cfg tm_sched_obj;   /**< declare main API struct */
    bcmbal_tm_sched_key key = { };      /**< declare key */
    
    if((tm_sched_cfg->key->has_dir) && (tm_sched_cfg->key->has_id))
    {
       key.dir = tm_sched_cfg->key->dir;
       key.id = tm_sched_cfg->key->id;
    }
    else
    {
       ASFVOLT_LOG(ASFVOLT_ERROR, "Failed to get the tm schedule cfg(OLT): Missing Key values\
                                   Recevied key values Sched-Dir(%d), Sched-Id(%d) \n", 
                                   tm_sched_cfg->key->dir, tm_sched_cfg->key->id);
       return BAL_ERRNO__BAL_ERR_NOENT;
    }
    

    ASFVOLT_LOG(ASFVOLT_DEBUG, "Gem tm scheduler cfg (for OLT) starts\n");

    /* init the API struct */
    BCMBAL_CFG_INIT(&tm_sched_obj, tm_sched, key);

    /* request all properties, include everything */
    BCMBAL_CFG_PROP_GET(&tm_sched_obj, tm_sched, all_properties);

    err = bcmbal_cfg_get(DEFAULT_ATERM_ID, &tm_sched_obj.hdr);

    if(BCM_ERR_OK != err)
    {
        ASFVOLT_LOG(ASFVOLT_ERROR, "Failed to get the tm scheduler Cfg(OLT)\n");
        return err;
    }

    ASFVOLT_LOG(ASFVOLT_INFO, "Get tm scheduler cfg sent to OLT. \
                               Sched ID(%d) Sched Dir(%d)\n", key.id, key.dir);
    return err;
}


/********************************************************************\
 * Function    : bal_tm_sched_cfg_clear                             *
 * Description : Clears the OLT device tm queue configuration       *
 ********************************************************************/

uint32_t bal_tm_sched_cfg_clear(BalTmSchedKey *tm_sched_key)
{
    bcmos_errno err = BCM_ERR_OK;
    bcmbal_tm_sched_cfg tm_sched_obj;   /**< declare main API struct */
    bcmbal_tm_sched_key key = { };      /**< declare key */
    
    if((tm_sched_key->has_dir) && (tm_sched_key->has_id))
    {
       key.dir = tm_sched_key->dir;
       key.id = tm_sched_key->id;
    }
    else
    {
       ASFVOLT_LOG(ASFVOLT_ERROR, "Failed to clear the tm schedule cfg(OLT): Missing Key values\
                                   Recevied key values Sched-Dir(%d), Sched-Id(%d) \n", 
                                   tm_sched_key->dir, tm_sched_key->id);
       return BAL_ERRNO__BAL_ERR_NOENT;
    }

    ASFVOLT_LOG(ASFVOLT_DEBUG, "Clear tm scheduler cfg(for OLT) starts\n");

    /* init the API struct */
    BCMBAL_CFG_INIT(&tm_sched_obj, tm_sched, key);

    err = bcmbal_cfg_clear(DEFAULT_ATERM_ID, &tm_sched_obj.hdr);

    if(BCM_ERR_OK != err)
    {
        ASFVOLT_LOG(ASFVOLT_ERROR, "Failed to clear the tm scheduler Cfg(OLT)\n");
        return err;
    }

    ASFVOLT_LOG(ASFVOLT_INFO, "Clear tm scheduler clear sent to OLT. \
                               Sched ID(%d) Sched Dir(%d)\n", key.id, key.dir);
    return err;
}


/********************************************************************\
 * Function    : bal_tm_sched_indication_cb                         *
 * Description : Call back function registered with BAL to handle   *
 *               event related to access terminal                   *
 ********************************************************************/
bcmos_errno bal_tm_sched_cfg_indication_cb(bcmbal_obj *obj)
{
    bcmos_errno result = BCM_ERR_OK;
    ASFVOLT_LOG(ASFVOLT_INFO, "Processing API (%s) IND callback status is (%s)\n",
		    bcmbal_objtype_str(obj->obj_type),
		    bcmos_strerror(obj->status));
    
    return result;
}


