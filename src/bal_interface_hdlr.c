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
#include "bal_interface_hdlr.h"

/********************************************************************\
 * Function    : bal_interface_cfg_set                              *
 * Description : Configures the PON and NNI interfaces              *
 *               of OLT Device                                      *
 ********************************************************************/
uint32_t bal_interface_cfg_set(BalInterfaceCfg *interface_cfg)
{
    bcmos_errno err = BCM_ERR_OK;
    bcmbal_interface_cfg interface_obj;
    bcmbal_interface_key intf_key;

    intf_key.intf_id = interface_cfg->key->intf_id;
    intf_key.intf_type = interface_cfg->key->intf_type;

    ASFVOLT_LOG(ASFVOLT_DEBUG, "Setting Interface ID(%d) Interface Type(%d) to Admin state(%d)",
                                intf_key.intf_id, intf_key.intf_type,
                                interface_cfg->data->admin_state);

    /*
     * Set the key in the interface object and initialize the object itself
     */
    BCMBAL_CFG_INIT(&interface_obj, interface, intf_key);

    ASFVOLT_CFG_PROP_SET(interface_obj, interface, admin_state, BCMOS_TRUE,
                         interface_cfg->data->admin_state);

    ASFVOLT_CFG_PROP_SET(interface_obj, interface, min_data_agg_port_id,
                         interface_cfg->data->has_min_data_svc_port_id,
                         interface_cfg->data->min_data_agg_port_id);

    ASFVOLT_CFG_PROP_SET(interface_obj, interface, min_data_svc_port_id,
                         interface_cfg->data->has_min_data_svc_port_id,
                         interface_cfg->data->min_data_svc_port_id);

    ASFVOLT_CFG_PROP_SET(interface_obj, interface, transceiver_type,
                         BCMOS_TRUE,
                         interface_cfg->data->transceiver_type);
    ASFVOLT_LOG(ASFVOLT_INFO, "Setting transceiver_type to : %d", interface_cfg->data->transceiver_type);

    ASFVOLT_CFG_PROP_SET(interface_obj, interface, mtu,
                         interface_cfg->data->has_mtu,
                         interface_cfg->data->mtu);

    ASFVOLT_CFG_PROP_SET(interface_obj, interface, ds_tm,
                         interface_cfg->data->has_ds_tm,
                         interface_cfg->data->ds_tm);

    ASFVOLT_CFG_PROP_SET(interface_obj, interface, us_tm,
                         interface_cfg->data->has_us_tm,
                         interface_cfg->data->us_tm);

    ASFVOLT_LOG(ASFVOLT_INFO, "Bringing up the interface No: %d", intf_key.intf_id);

    err = bcmbal_cfg_set(DEFAULT_ATERM_ID, &(interface_obj.hdr));

    if(BCM_ERR_OK != err)
    {
        ASFVOLT_LOG(ASFVOLT_ERROR, "Failed to configure the interface object to ADMIN-UP");
        return BAL_ERRNO__BAL_ERR_INTERNAL;
    }
    ASFVOLT_LOG(ASFVOLT_DEBUG, "Set Interface configuration sent to OLT.Interface ID(%d) Interface Type(%d)",
                                intf_key.intf_id, intf_key.intf_type);
    return BAL_ERRNO__BAL_ERR_OK;
}


/********************************************************************\
 * Function    : bal_interface_cfg_get                              *
 * Description : get the PON and NNI interfaces                     *
 *               of OLT Device                                      *
 ********************************************************************/
uint32_t bal_interface_cfg_get(BalInterfaceCfg *interface_cfg)
{
    bcmos_errno err = BCM_ERR_OK;
    bcmbal_interface_cfg interface_obj;
    bcmbal_interface_key intf_key;

    intf_key.intf_id = interface_cfg->key->intf_id;
    intf_key.intf_type = interface_cfg->key->intf_type;

    ASFVOLT_LOG(ASFVOLT_DEBUG, "Get interface cfg(for OLT) starts");
    /*
     * Set the key in the interface object and initialize the object itself
     */
    BCMBAL_CFG_INIT(&interface_obj, interface, intf_key);

    /* request all properties, include everything */
    BCMBAL_CFG_PROP_GET(&interface_obj, interface, all_properties);

    err = bcmbal_cfg_get(DEFAULT_ATERM_ID, &interface_obj.hdr);

    if(BCM_ERR_OK != err)
    {
        ASFVOLT_LOG(ASFVOLT_ERROR, "Failed to get the interface Cfg(OLT)");
        return BAL_ERRNO__BAL_ERR_INTERNAL;
    }

    ASFVOLT_LOG(ASFVOLT_INFO, "Get Interface cfg sent to OLT. "
                              "Interface ID(%d) Interface Type(%d)",
                               intf_key.intf_id, intf_key.intf_type);

    memcpy(interface_cfg->data, &interface_obj, sizeof(bcmbal_interface_cfg_data));

    return BAL_ERRNO__BAL_ERR_OK;
}


/********************************************************************\
 * Function    : bal_interface_cfg_clear                            *
 * Description : Clears the PON and NNI interfaces                  *
 *               of OLT Device                                      *
 ********************************************************************/
uint32_t bal_interface_cfg_clear(BalInterfaceKey *interface_cfg_key)
{
    bcmos_errno err = BCM_ERR_OK;
    bcmbal_interface_cfg interface_obj;
    bcmbal_interface_key intf_key;

    if((interface_cfg_key->has_intf_id) && (interface_cfg_key->has_intf_type))
    {
       intf_key.intf_id = interface_cfg_key->intf_id;
       intf_key.intf_type = interface_cfg_key->intf_type;
    }
    else
    {
       ASFVOLT_LOG(ASFVOLT_ERROR, "Failed to clear the interface cfg(OLT): Missing Key values "
                                  "Received key values intf-id(%d), intf-type(%d)",
                                   interface_cfg_key->intf_id, interface_cfg_key->intf_type);
       return BAL_ERRNO__BAL_ERR_NOENT;
    }


    ASFVOLT_LOG(ASFVOLT_DEBUG, "Clearing of OLT(interface cfg) starts");
    /*
     * Set the key in the interface object and initialize the object itself
     */
    BCMBAL_CFG_INIT(&interface_obj, interface, intf_key);

    err = bcmbal_cfg_clear(DEFAULT_ATERM_ID, &interface_obj.hdr);

    if(BCM_ERR_OK != err)
    {
        ASFVOLT_LOG(ASFVOLT_ERROR, "Failed to clear the interface Cfg(OLT)");
        return BAL_ERRNO__BAL_ERR_INTERNAL;
    }

    ASFVOLT_LOG(ASFVOLT_INFO, "Clear Interface cfg sent to OLT. "
                              "Interface ID(%d) Interface Type(%d)",
                               intf_key.intf_id, intf_key.intf_type);
    return BAL_ERRNO__BAL_ERR_OK;
}
