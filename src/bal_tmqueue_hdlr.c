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
#include "bal_tmqueue_hdlr.h"


/********************************************************************\
 * Function    : bal_tm_queue_cfg_set                           *
 * Description : Configures the tm queue for OLT device             *
 ********************************************************************/

uint32_t bal_tm_queue_cfg_set(BalTmQueueCfg *tm_queue_cfg)
{
    bcmos_errno err = BCM_ERR_OK;
    bcmbal_tm_queue_cfg tm_queue_obj;   /**< declare main API struct */
    bcmbal_tm_queue_key key = { };      /**< declare key */

    if((tm_queue_cfg->key->has_id) && (tm_queue_cfg->key->has_sched_id) && (tm_queue_cfg->key->has_sched_dir))
    {
       key.id = tm_queue_cfg->key->id;
       key.sched_id = tm_queue_cfg->key->sched_id;
       key.sched_dir = tm_queue_cfg->key->sched_dir;
    }
    else
    {
       ASFVOLT_LOG(ASFVOLT_ERROR, "Failed to configure the tm queue cfg(OLT): Missing Key values\
                                   Recevied key values Id(%d), Sched-Dir(%d), Sched-Id(%d) \n", 
                                   tm_queue_cfg->key->sched_id, tm_queue_cfg->key->sched_dir, tm_queue_cfg->key->id);
       return BAL_ERRNO__BAL_ERR_NOENT;
    }

    ASFVOLT_LOG(ASFVOLT_DEBUG, "Configuration of OLT(tm Queue) starts\n");

    /* init the API struct */
    BCMBAL_CFG_INIT(&tm_queue_obj, tm_queue, key);

    /* decode API parameters from GRPC */
    ASFVOLT_CFG_PROP_SET(tm_queue_obj, tm_queue, priority, 
                         tm_queue_cfg->data->has_priority, 
                         tm_queue_cfg->data->priority);
    ASFVOLT_CFG_PROP_SET(tm_queue_obj, tm_queue, weight, 
                         tm_queue_cfg->data->has_weight, 
                         tm_queue_cfg->data->weight);
    ASFVOLT_CFG_PROP_SET(tm_queue_obj, tm_queue, creation_mode, 
                         tm_queue_cfg->data->has_creation_mode, 
                         tm_queue_cfg->data->creation_mode);
    ASFVOLT_CFG_PROP_SET(tm_queue_obj, tm_queue, ref_count, 
                         tm_queue_cfg->data->has_ref_count, 
                         tm_queue_cfg->data->ref_count);

    /* rating/shaping */
    BalTmShaping *balShaping = (BalTmShaping *)tm_queue_cfg->data->rate;
    bcmbal_tm_shaping val = {};
    if (balShaping->has_presence_mask)
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
       ASFVOLT_CFG_PROP_SET(tm_queue_obj, tm_queue, rate, BCMOS_TRUE, val);
    }

    /* bac - Buffer addmission Control */
    BalTmBac *balBac = (BalTmBac *)tm_queue_cfg->data->bac;
    bcmbal_tm_bac valBac = {};
    if(balBac->has_type)
    {
       valBac.type = balBac->type;
       switch(valBac.type)
       {
           case BAL_TM_BAC__U_TAILDROP:
             if(balBac->taildrop->has_max_size)
             {
                valBac.u.taildrop.max_size = balBac->taildrop->max_size;
             }
             break;
           case BAL_TM_BAC__U_RED:
             if(balBac->red->red->has_min_threshold)
             {
                 valBac.u.red.red.min_threshold = balBac->red->red->min_threshold;
             }
             if(balBac->red->red->has_max_threshold)
             {
                 valBac.u.red.red.max_threshold = balBac->red->red->max_threshold;
             }
             if(balBac->red->red->has_max_probability)
             {
                 valBac.u.red.red.max_probability = balBac->red->red->max_probability;
             }
             break;
           case BAL_TM_BAC__U_WRED:
             if(balBac->wred->red->has_min_threshold)
             {
                 valBac.u.wred.red.min_threshold = balBac->wred->red->min_threshold;
             }
             if(balBac->wred->red->has_max_threshold)
             {
                 valBac.u.wred.red.max_threshold = balBac->wred->red->max_threshold;
             }
             if(balBac->wred->red->has_max_probability)
             {
                 valBac.u.wred.red.max_probability= balBac->wred->red->max_probability;
             }
             if(balBac->wred->green->has_min_threshold)
             {
                 valBac.u.wred.green.min_threshold = balBac->wred->green->min_threshold;
             }
             if(balBac->wred->green->has_max_threshold)
             {
                 valBac.u.wred.green.max_threshold = balBac->wred->green->max_threshold;
             }
             if(balBac->wred->green->has_max_probability)
             {
                 valBac.u.wred.green.max_probability= balBac->wred->green->max_probability;
             }
             if(balBac->wred->yellow->has_min_threshold)
             {
                 valBac.u.wred.yellow.min_threshold = balBac->wred->yellow->min_threshold;
             }
             if(balBac->wred->yellow->has_max_threshold)
             {
                 valBac.u.wred.yellow.max_threshold = balBac->wred->yellow->max_threshold;
             }
             if(balBac->wred->yellow->has_max_probability)
             {
                 valBac.u.wred.yellow.max_probability= balBac->wred->yellow->max_probability;
             }
             break;
           default:
             break;
       }
       ASFVOLT_CFG_PROP_SET(tm_queue_obj, tm_queue, bac, BCMOS_TRUE, valBac);
    }

    err = bcmbal_cfg_set(DEFAULT_ATERM_ID, &(tm_queue_obj.hdr));

    if(BCM_ERR_OK != err)
    {
        ASFVOLT_LOG(ASFVOLT_ERROR, "Failed to configure the tm Queue Cfg(OLT)\n");
        return err;
    }

    ASFVOLT_LOG(ASFVOLT_INFO, "\nSet tm Queue configuration sent to OLT. \
                               Queue ID(%d) Sched ID(%d) Sched Dir(%d)\n", 
                               key.id, key.sched_id, key.sched_dir );
    return err;
}


/********************************************************************\
 * Function    : bal_tm_queue_cfg_clear                         *
 * Description : clear the OLT device tm queue configuration        *
 ********************************************************************/

uint32_t bal_tm_queue_cfg_clear(BalTmQueueKey *tm_queue_key)
{
    bcmos_errno err = BCM_ERR_OK;
    bcmbal_tm_queue_cfg tm_queue_obj;   /**< declare main API struct */
    bcmbal_tm_queue_key key = { };      /**< declare key */
    
    if((tm_queue_key->has_id) && (tm_queue_key->has_sched_id) && (tm_queue_key->has_sched_dir))
    {
       key.id = tm_queue_key->id;
       key.sched_id = tm_queue_key->sched_id;
       key.sched_dir = tm_queue_key->sched_dir;
    }
    else
    {
       ASFVOLT_LOG(ASFVOLT_ERROR, "Failed to clear the tm queue cfg(OLT): Missing Key values\
                                   Recevied key values Id(%d), Sched-Dir(%d), Sched-Id(%d) \n", 
                                   tm_queue_key->sched_id, tm_queue_key->sched_dir, tm_queue_key->id);
       return BAL_ERRNO__BAL_ERR_NOENT;
    }

    ASFVOLT_LOG(ASFVOLT_DEBUG, "Clearing of OLT(tm Queue) starts\n");

    /* init the API struct */
    BCMBAL_CFG_INIT(&tm_queue_obj, tm_queue, key);

    err = bcmbal_cfg_clear(DEFAULT_ATERM_ID, &tm_queue_obj.hdr);

    if(BCM_ERR_OK != err)
    {
        ASFVOLT_LOG(ASFVOLT_ERROR, "Failed to clear the tm Queue Cfg(OLT)\n");
        return err;
    }

    ASFVOLT_LOG(ASFVOLT_INFO, "\nClear tm Queue sent to OLT. \
                               Queue ID(%d) Sched ID(%d) Sched Dir(%d)\n", 
                               key.id, key.sched_id, key.sched_dir );
    return err;
}


/********************************************************************\
 * Function    : bal_tm_queue_cfg_get                               *
 * Description : Get the OLT device tm queue configuration          *
 ********************************************************************/

uint32_t bal_tm_queue_cfg_get(BalTmQueueKey *tm_queue_key, BalTmQueueCfg *tm_queue_cfg)
{
    bcmos_errno err = BCM_ERR_OK;
    bcmbal_tm_queue_cfg tm_queue_obj;   /**< declare main API struct */
    bcmbal_tm_queue_key key = { };      /**< declare key */
    
    if((tm_queue_key->has_id) && (tm_queue_key->has_sched_id) && (tm_queue_key->has_sched_dir))
    {
       key.id = tm_queue_key->id;
       key.sched_id = tm_queue_key->sched_id;
       key.sched_dir = tm_queue_key->sched_dir;
    }
    else
    {
       ASFVOLT_LOG(ASFVOLT_ERROR, "Failed to get the tm queue cfg(OLT): Missing Key values\
                                   Recevied key values Id(%d), Sched-Dir(%d), Sched-Id(%d) \n", 
                                   tm_queue_key->sched_id, tm_queue_key->sched_dir, tm_queue_key->id);
       return BAL_ERRNO__BAL_ERR_NOENT;
    }

    ASFVOLT_LOG(ASFVOLT_DEBUG, "Get tm queue cfg(for OLT) starts\n");

    /* init the API struct */
    BCMBAL_CFG_INIT(&tm_queue_obj, tm_queue, key);

    /* request all properties, include everything */
    BCMBAL_CFG_PROP_GET(&tm_queue_obj, tm_queue, all_properties);

    err = bcmbal_cfg_get(DEFAULT_ATERM_ID, &tm_queue_obj.hdr);

    if(BCM_ERR_OK != err)
    {
        ASFVOLT_LOG(ASFVOLT_ERROR, "Failed to get the tm Queue Cfg(OLT)\n");
        return err;
    }

    ASFVOLT_LOG(ASFVOLT_INFO, "\nGet tm Queue sent to OLT. \
                               Queue ID(%d) Sched ID(%d) Sched Dir(%d)\n", 
                               key.id, key.sched_id, key.sched_dir );
    return err;
}


/********************************************************************\
 * Function    : bal_tm_queue_indication_cb                         *
 * Description : Call back function registered with BAL to handle   *
 *               event related to access terminal                   *
 ********************************************************************/
bcmos_errno bal_tm_queue_cfg_indication_cb(bcmbal_obj *obj)
{
    bcmos_errno result = BCM_ERR_OK;
    ASFVOLT_LOG(ASFVOLT_INFO, "Processing API \'%s\' IND callback (status is %s)\n",
		    bcmbal_objtype_str(obj->obj_type),
		    bcmos_strerror(obj->status));
    
    return result;
}


