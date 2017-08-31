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
#include "bal_access_terminal_hdlr.h"


/********************************************************************\
 * Function    : bal_access_terminal_cfg_set                        *
 * Description : Configures the OLT device                          *
 ********************************************************************/

uint32_t bal_access_terminal_cfg_set(BalAccessTerminalCfg *access_term_cfg)
{
    bcmos_errno err = BCM_ERR_OK;
    bcmbal_access_terminal_cfg acc_term_obj;         /**< declare main API struct */
    bcmbal_access_terminal_key key = { };   /**< declare key */

    key.access_term_id = access_term_cfg->key->access_term_id;

    ASFVOLT_LOG(ASFVOLT_DEBUG, "Configuration of OLT starts\n");

    /* init the API struct */
    BCMBAL_CFG_INIT(&acc_term_obj, access_terminal, key);

    /* decode API parameters from CLI */
    ASFVOLT_CFG_PROP_SET(acc_term_obj, access_terminal, admin_state, BCMOS_TRUE, BCMBAL_STATE_UP);

    err = bcmbal_cfg_set(DEFAULT_ATERM_ID, &(acc_term_obj.hdr));

    if(BCM_ERR_OK != err)
    {
        ASFVOLT_LOG(ASFVOLT_ERROR, "Failed to configure the access-terminal(OLT) to ADMIN-UP\n");
        return BAL_ERRNO__BAL_ERR_INTERNAL;
    }

    ASFVOLT_LOG(ASFVOLT_INFO, "Access Terminal UP configuration sent to OLT\n");
    return BAL_ERRNO__BAL_ERR_OK;
}


/********************************************************************\
 * Function    : bal_access_term_indication_cb                      *
 * Description : Call back function registered with BAL to handle   *
 *               event related to access terminal                   *
 ********************************************************************/
bcmos_errno bal_access_term_indication_cb(bcmbal_obj *obj)
{
    /*bcmos_errno result = BCM_ERR_OK;*/
    ASFVOLT_LOG(ASFVOLT_INFO, "Processing API \'%s\' IND callback (status is %s)\n",
		    bcmbal_objtype_str(obj->obj_type),
		    bcmos_strerror(obj->status));
    
    return BAL_ERRNO__BAL_ERR_OK;
}


/********************************************************************\
 * Function    : bal_access_terminal_cfg_clear                      *
 * Description : Handles the clear/Delete of access terminal        *
 ********************************************************************/
uint32_t bal_access_terminal_cfg_clear(BalAccessTerminalKey *access_term_key)
{
    bcmos_errno err = BCM_ERR_OK;
    bcmbal_access_terminal_cfg cfg_clear;         /**< declare main API struct */
    bcmbal_access_terminal_key key = { };   /**< declare key */

    ASFVOLT_LOG(ASFVOLT_INFO, "processing the clear request on access terminal\n");

    key.access_term_id = access_term_key->access_term_id;

    /* init the API struct */
    BCMBAL_CFG_INIT(&cfg_clear, access_terminal, key);


    /* call API */
    err = bcmbal_cfg_clear(DEFAULT_ATERM_ID, &cfg_clear.hdr);

    if( err != BCM_ERR_OK)
    {
       ASFVOLT_LOG(ASFVOLT_INFO, "Failed to clear the access terminal\n");
       return BAL_ERRNO__BAL_ERR_INTERNAL;
    }

    ASFVOLT_LOG(ASFVOLT_INFO, "Successfully cleared the access terminal\n");
    return BAL_ERRNO__BAL_ERR_OK;
}


/********************************************************************\
 * Function    : bal_access_terminal_cfg_get                        *
 * Description : Handles the clear/Delete of access terminal        *
 ********************************************************************/
uint32_t bal_access_terminal_cfg_get (BalAccessTerminalKey *access_term_key,
                                      BalAccessTerminalCfg *access_term_cfg)
{
    bcmos_errno err = BCM_ERR_OK;
    bcmbal_access_terminal_cfg cfg;         /**< declare main API struct */
    bcmbal_access_terminal_key key = { };   /**< declare key */

    ASFVOLT_LOG(ASFVOLT_INFO, "processing the get request on access terminal\n");
    /* init the API struct */
    BCMBAL_CFG_INIT(&cfg, access_terminal, key);

    BCMBAL_CFG_PROP_GET(&cfg, access_terminal, all_properties);

    /* call API */
    err = bcmbal_cfg_get(DEFAULT_ATERM_ID, &cfg.hdr);
    if (err != BCM_ERR_OK)
    {
       ASFVOLT_LOG(ASFVOLT_ERROR, "Failed to get access terminal configuration\n");
       return BAL_ERRNO__BAL_ERR_INTERNAL;
    }

    ASFVOLT_LOG(ASFVOLT_INFO, 
                  "To-Do. Send access terminal details to Adapter\n");
   
    return BAL_ERRNO__BAL_ERR_OK;
}
