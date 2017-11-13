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
#include "bal_subscriber_terminal_hdlr.h"

/********************************************************************\
 * Function    : bal_subscriber_terminal_cfg_set                    *
 * Description : Configures the subscriber terminal(ONU)            *
 ********************************************************************/
uint32_t bal_subscriber_terminal_cfg_set(BalSubscriberTerminalCfg *onu_cfg)
{

    bcmos_errno err = BCM_ERR_OK;
    bcmbal_subscriber_terminal_cfg sub_term_obj = {};
    bcmbal_subscriber_terminal_key subs_terminal_key;
    bcmos_bool skip_onu = BCMOS_FALSE;

    size_t idx;

    subs_terminal_key.sub_term_id = onu_cfg->key->sub_term_id;
    subs_terminal_key.intf_id = onu_cfg->key->intf_id;

    ASFVOLT_LOG(ASFVOLT_INFO, "Bringing up the subscriber terminal: %d", onu_cfg->key->sub_term_id);

    /*
     * Set the key in the subscriber terminal object
     */
    BCMBAL_CFG_INIT(&sub_term_obj, subscriber_terminal, subs_terminal_key);

    {
       bcmbal_serial_number serial_num = {} ;
       bcmbal_serial_number zero_serial_num =  {};
       bcmbal_registration_id registration_id =  {};
       int has_serial_num = BCMOS_FALSE;
       int has_registration_id = BCMOS_FALSE;
       char two_digit_buf[3];

       two_digit_buf[2] = 0;

       ASFVOLT_LOG(ASFVOLT_DEBUG, "Before encoding,Vendor Id(%s),Vendor Specific Id(%s), Registration Id(%s)",
		       onu_cfg->data->serial_number->vendor_id,
		       onu_cfg->data->serial_number->vendor_specific,
		       onu_cfg->data->registration_id);

       char vendor_id[20];
       memset(&vendor_id, 0, 20);
       sprintf(vendor_id,"%2X%2X%2X%2X",
		       onu_cfg->data->serial_number->vendor_id[0],
		       onu_cfg->data->serial_number->vendor_id[1],
		       onu_cfg->data->serial_number->vendor_id[2],
		       onu_cfg->data->serial_number->vendor_id[3]);
       onu_cfg->data->serial_number->vendor_id = vendor_id;
       ASFVOLT_LOG(ASFVOLT_DEBUG, "After encoding,Vendor Id(%s),Vendor Specific Id(%s), Registration Id(%s)",
		       onu_cfg->data->serial_number->vendor_id,
		       onu_cfg->data->serial_number->vendor_specific,
		       onu_cfg->data->registration_id);

       /* Vendor Id is totally 16 byte string and should be
          send in hexadecimmal format */
       for(idx=0; idx<2*sizeof(serial_num.vendor_id); idx+=2)
       {
          memcpy(two_digit_buf, &onu_cfg->data->serial_number->vendor_id[idx], 2);
          serial_num.vendor_id[idx>>1] = strtol(two_digit_buf, NULL, 16);
          has_serial_num = BCMOS_TRUE;
       }
       for(idx=0; idx<2*sizeof(serial_num.vendor_specific); idx+=2)
       {
          memcpy(two_digit_buf, &onu_cfg->data->serial_number->vendor_specific[idx], 2);
          serial_num.vendor_specific[idx>>1] = strtol(two_digit_buf, NULL, 16);
       }

       ASFVOLT_CFG_PROP_SET(sub_term_obj, subscriber_terminal, serial_number,
                  has_serial_num, serial_num);

       /* Registration ID is a string and should be given in hexadecimal format */
       for(idx=0; idx<strlen(onu_cfg->data->registration_id) && idx<2*sizeof(registration_id.arr); idx+=2)
       {
          memcpy(two_digit_buf, &onu_cfg->data->registration_id[idx], 2);
          registration_id.arr[idx>>1] = strtol(two_digit_buf, NULL, 16);
          has_registration_id = BCMOS_TRUE;
       }

       ASFVOLT_CFG_PROP_SET(sub_term_obj, subscriber_terminal, registration_id,
                  has_registration_id, registration_id);

       if (!memcmp(&serial_num, &zero_serial_num, sizeof(serial_num)))
          skip_onu = BCMOS_TRUE;
    }

    ASFVOLT_CFG_PROP_SET(sub_term_obj, subscriber_terminal, admin_state,
                  BCMOS_TRUE, BCMBAL_STATE_UP);

    if (!skip_onu)
    {

       /*ASFVOLT_LOG(ASFVOLT_DEBUG, "Onu's(%d) Serial number %02x%02x%02x%2x%02x%02x%02x%02x",
             onu_cfg->key->sub_term_id,
             sub_term_obj.data->serial_number.vendor_id[0], sub_term_obj.data->serial_number.vendor_id[1],
             sub_term_obj.data->serial_number.vendor_id[2], sub_term_obj.data->serial_number.vendor_id[3],
             sub_term_obj.data->serial_number.vendor_specific[0], sub_term_obj.data->serial_number.vendor_specific[1],
             sub_term_obj.data->serial_number.vendor_specific[2], sub_term_obj.data->serial_number.vendor_specific[3]);*/

       err = bcmbal_cfg_set(DEFAULT_ATERM_ID, &(sub_term_obj.hdr));

       ASFVOLT_LOG(ASFVOLT_DEBUG,
                   "....SENT SUBSCRIBER TERMINAL %sUP %d on interface %d...",
                   (BCM_ERR_OK != err) ? "NOT " : "",
                   subs_terminal_key.sub_term_id,
                   subs_terminal_key.intf_id);
    }
    else
    {
       ASFVOLT_LOG(ASFVOLT_DEBUG,
                   "Skipping activation of subscriber terminal %d on interface %d",
                   subs_terminal_key.sub_term_id,
                   subs_terminal_key.intf_id);
       err = BAL_ERRNO__BAL_ERR_PARM;
    }

    return BAL_ERRNO__BAL_ERR_OK;
}


/********************************************************************\
 * Function    : bal_subscriber_terminal_indication_cb              *
 * Description : Call Back indication registered with BAL to handle *
 *               events related to subscriber terminal(ONU)         *
 ********************************************************************/
bcmos_errno bal_subscriber_terminal_indication_cb(bcmbal_obj *obj)
{
    bcmos_errno result = BCM_ERR_OK;

#if 0
    bcmbal_subscriber_terminal_cfg *cfg = ((bcmbal_subscriber_terminal_cfg *)obj);

    if(BCMBAL_OBJ_ID_SUBSCRIBER_TERMINAL == obj->obj_type)
    {
       bcmbal_serial_number *p_serial_number =
                 (((bcmbal_subscriber_terminal_cfg *)obj)->data->serial_number);

       if(BCMBAL_SUB_ID_UNKNOWN == (((bcmbal_subscriber_terminal_cfg *)obj)->key.sub_term_id))
       {
          ASFVOLT_LOG(ASFVOLT_INFO, "New ONU Discovered. serial number "
                "%2X%2X%2X%2X%1X%1X%1X%1X%1X%1X%1X%1X "
                "on PON %d",
                p_serial_number->vendor_id[0],
                p_serial_number->vendor_id[1],
                p_serial_number->vendor_id[2],
                p_serial_number->vendor_id[3],
                p_serial_number->vendor_specific[0]>>4 & 0x0f,
                p_serial_number->vendor_specific[0] & 0x0f,
                p_serial_number->vendor_specific[1]>>4 & 0x0f,
                p_serial_number->vendor_specific[1] & 0x0f,
                p_serial_number->vendor_specific[2]>>4 & 0x0f,
                p_serial_number->vendor_specific[2] & 0x0f,
                p_serial_number->vendor_specific[3]>>4 & 0x0f,
                p_serial_number->vendor_specific[3] & 0x0f,

                ((bcmbal_subscriber_terminal_cfg *)obj)->key.intf_id);

       }
       else
       {
          ASFVOLT_LOG(ASFVOLT_INFO, "Event on existing ONU. serial number "
                "%2X%2X%2X%2X%1X%1X%1X%1X%1X%1X%1X%1X "
                "on PON %d",
                p_serial_number->vendor_id[0],
                p_serial_number->vendor_id[1],
                p_serial_number->vendor_id[2],
                p_serial_number->vendor_id[3],
                p_serial_number->vendor_specific[0]>>4 & 0x0f,
                p_serial_number->vendor_specific[0] & 0x0f,
                p_serial_number->vendor_specific[1]>>4 & 0x0f,
                p_serial_number->vendor_specific[1] & 0x0f,
                p_serial_number->vendor_specific[2]>>4 & 0x0f,
                p_serial_number->vendor_specific[2] & 0x0f,
                p_serial_number->vendor_specific[3]>>4 & 0x0f,
                p_serial_number->vendor_specific[3] & 0x0f,

                ((bcmbal_subscriber_terminal_cfg *)obj)->key.intf_id);

       }
    }
    else
    {
       ASFVOLT_LOG(ASFVOLT_ERROR,
                   "Invalid object type %d for subscriber terminal indication",
                   obj->obj_type);

    }

#endif
    return result;
}

/********************************************************************\
 * Function    : bal_subscriber_terminal_cfg_clear                  *
 * Description : clears the subscriber terminal(ONU) configuration  *
 ********************************************************************/
uint32_t bal_subscriber_terminal_cfg_clear(BalSubscriberTerminalKey *terminal_key)
{
    bcmos_errno err = BCM_ERR_OK;
    bcmbal_subscriber_terminal_cfg cfg;
    bcmbal_subscriber_terminal_key key = { };

    ASFVOLT_LOG(ASFVOLT_INFO,
                    "Processing subscriber terminal cfg clear: %d",
                     terminal_key->sub_term_id);

    if (terminal_key->has_sub_term_id &&
                    terminal_key->has_intf_id)
    {
        key.sub_term_id = terminal_key->sub_term_id ;
        key.intf_id = terminal_key->intf_id ;
    }
    else
    {
       ASFVOLT_LOG(ASFVOLT_ERROR,
                   "Invalid Key to handle subscriber terminal clear Req.subscriber_terminal_id(%d), Interface ID(%d)",
                   key.sub_term_id, key.intf_id);

        return BAL_ERRNO__BAL_ERR_PARM;
    }

    err = bcmbal_cfg_clear(DEFAULT_ATERM_ID, &cfg.hdr);
    err = BAL_ERRNO__BAL_ERR_OK;
    return err;
}

/********************************************************************\
 * Function    : bal_subscriber_terminal_cfg_get                    *
 * Description : Get the subscriber terminal(ONU) configuration     *
 ********************************************************************/
uint32_t bal_subscriber_terminal_cfg_get(BalSubscriberTerminalKey *terminal_key,
                                         BalSubscriberTerminalCfg *onu_cfg)
{

    bcmos_errno err = BCM_ERR_OK;
    bcmbal_subscriber_terminal_cfg cfg;         /**< declare main API struct */
    bcmbal_subscriber_terminal_key key = { };   /**< declare key */
    uint8_t *list_mem;  /**< declare memory buffer for variable-sized lists */

    ASFVOLT_LOG(ASFVOLT_INFO,
                    "Processing subscriber terminal cfg get: %d",
                     onu_cfg->key->sub_term_id);

    if (terminal_key->has_sub_term_id &&
                    terminal_key->has_intf_id)
    {
        key.sub_term_id = terminal_key->sub_term_id ;
        key.intf_id = terminal_key->intf_id ;
    }
    else
    {
       ASFVOLT_LOG(ASFVOLT_ERROR,
                   "Invalid Key to handle subscriber terminal Cfg Get subscriber_terminal_id(%d), Interface ID(%d)",
                   key.sub_term_id, key.intf_id);

        return BAL_ERRNO__BAL_ERR_PARM;
    }

    /* init the API struct */
    BCMBAL_CFG_INIT(&cfg, subscriber_terminal, key);

    BCMBAL_CFG_PROP_GET(&cfg, subscriber_terminal, all_properties);

    /* set memory to use for variable-sized lists */
    list_mem = malloc(BAL_DYNAMIC_LIST_BUFFER_SIZE);
    if (list_mem == NULL)
    {

       ASFVOLT_LOG(ASFVOLT_ERROR,
                   "Memory allocation failed while handling subscriber terminal cfg get subscriber_terminal_id(%d), Interface ID(%d)",
                   key.sub_term_id, key.intf_id);
        return BAL_ERRNO__BAL_ERR_NOMEM;
    }

    memset(list_mem, 0, BAL_DYNAMIC_LIST_BUFFER_SIZE);
    BCMBAL_CFG_LIST_BUF_SET(&cfg, subscriber_terminal, list_mem, BAL_DYNAMIC_LIST_BUFFER_SIZE);

    /* call API */
    err = bcmbal_cfg_get(DEFAULT_ATERM_ID, &cfg.hdr);
    if (err != BCM_ERR_OK)
    {
       ASFVOLT_LOG(ASFVOLT_ERROR,
                   "Failed to get information from BAL subscriber_terminal_id(%d), Interface ID(%d)",
                   key.sub_term_id, key.intf_id);
        return BAL_ERRNO__BAL_ERR_INTERNAL;
    }

    ASFVOLT_LOG(ASFVOLT_INFO,
                  "To-Do. Send subscriber terminal details to Adapter");
    return BAL_ERRNO__BAL_ERR_OK;
}
