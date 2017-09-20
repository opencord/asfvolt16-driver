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
#include "bal_msg_type.grpc-c.h"
#include "bal_osmsg.grpc-c.h"
#include "bal_model_ids.grpc-c.h"
#include "bal_obj.grpc-c.h"
#include "bal_model_types.grpc-c.h"
#include "bal_errno.grpc-c.h"
#include "bal.grpc-c.h"

#include "asfvolt16_driver.h"
#include "bal_packet_hdlr.h"
static bcmos_mutex bal_ind_lock;


/********************************************************************\
 * Function    : bal_access_terminal_cfg_req                        *
 * Description : Configures the PON and NNI interfaces              *
 *               of OLT Device                                      *
 ********************************************************************/
uint32_t bal_packet_cfg_req(BalPacketCfg *packet)
{
   bcmos_mutex_lock(&bal_ind_lock);
   bcmos_errno err = BCM_ERR_OK;
   bcmbal_access_term_id aterm_id = 0; /* Assume a single access_terminal instance */
   bcmbal_u8_list_u32_max_2048 buf; /* A structure with a msg pointer and length value */
   switch(packet->key->packet_send_dest->type)
   {
      case BAL_DEST_TYPE__BAL_DEST_TYPE_NNI:
         {
            bcmbal_dest proxy_pkt_dest = { .type = BCMBAL_DEST_TYPE_NNI,
               .u.nni.intf_id = packet->key->packet_send_dest->nni->intf_id };
            buf.len = packet->data->pkt.len;
            buf.val = (uint8_t *)malloc((buf.len)*sizeof(uint8_t));
            memcpy(buf.val,(uint8_t *)packet->data->pkt.data,buf.len);
	    ASFVOLT_LOG(ASFVOLT_DEBUG, "\n Packet of length %d bytes with dest type as %d\n",buf.len,packet->key->packet_send_dest->type);
            err = bcmbal_pkt_send(aterm_id,
                  proxy_pkt_dest,
                  (const char *)(buf.val),
                  buf.len);
            free(buf.val);
         }
         break;
      case BAL_DEST_TYPE__BAL_DEST_TYPE_SUB_TERM:
         {
            bcmbal_dest proxy_pkt_dest = { .type = BCMBAL_DEST_TYPE_SUB_TERM,
               .u.sub_term.sub_term_id = packet->key->packet_send_dest->sub_term->sub_term_id,
               .u.sub_term.intf_id = packet->key->packet_send_dest->sub_term->intf_id };
            buf.len = packet->data->pkt.len;
            buf.val = (uint8_t *)malloc((buf.len)*sizeof(uint8_t));
            memcpy(buf.val,(uint8_t *)packet->data->pkt.data,buf.len);
	    ASFVOLT_LOG(ASFVOLT_DEBUG, "\n Packet of length %d bytes with dest type as %d\n",buf.len,packet->key->packet_send_dest->type);
            err = bcmbal_pkt_send(aterm_id,
                  proxy_pkt_dest,
                  (const char *)(buf.val),
                  buf.len);
            free(buf.val);
         }
         break;
      case BAL_DEST_TYPE__BAL_DEST_TYPE_SVC_PORT:
         {
            bcmbal_dest proxy_pkt_dest = { .type = BCMBAL_DEST_TYPE_SVC_PORT,
               .u.svc_port.svc_port_id = packet->key->packet_send_dest->svc_port->svc_port_id,
               .u.svc_port.intf_id = packet->key->packet_send_dest->svc_port->intf_id };
            buf.len = packet->data->pkt.len;
            buf.val = (uint8_t *)malloc((buf.len)*sizeof(uint8_t));
	    ASFVOLT_LOG(ASFVOLT_DEBUG, "\n Packet of length %d bytes with dest type as %d\n",buf.len,packet->key->packet_send_dest->type);
            memcpy(buf.val,(uint8_t *)packet->data->pkt.data,buf.len);
            err = bcmbal_pkt_send(aterm_id,
                  proxy_pkt_dest,
                  (const char *)(buf.val),
                  buf.len);
            free(buf.val);
         }
         break;
      case BAL_DEST_TYPE__BAL_DEST_TYPE_ITU_OMCI_CHANNEL:
         {
            /* The destination of the OMCI packet is a registered ONU on the OLT PON interface */
            bcmbal_dest proxy_pkt_dest = { .type = BCMBAL_DEST_TYPE_ITU_OMCI_CHANNEL,
               .u.itu_omci_channel.sub_term_id = packet->key->packet_send_dest->itu_omci_channel->sub_term_id,
               .u.itu_omci_channel.intf_id = packet->key->packet_send_dest->itu_omci_channel->intf_id };
            buf.len = ((strlen((const char *)(packet->data->pkt.data))/2)) > MAX_OMCI_MSG_LENGTH ? MAX_OMCI_MSG_LENGTH : ((strlen((const char *)(packet->data->pkt.data))/2)); /* is the length of your OMCI message */
            /* Send the OMCI packet using the BAL remote proxy API */
            uint16_t idx1 = 0;
            uint16_t idx2 = 0;
            uint8_t arraySend[buf.len];
            char str1[MAX_CHAR_LENGTH];
            char str2[MAX_CHAR_LENGTH];
            memset(&arraySend,0,buf.len);
            ASFVOLT_LOG(ASFVOLT_DEBUG,"\nSending omci msg to ONU of length is %d\n",buf.len);
            for(idx1=0,idx2=0; idx1<((buf.len)*2); idx1++,idx2++)
            {
               sprintf(str1,"%c",packet->data->pkt.data[idx1]);
               sprintf(str2,"%c",packet->data->pkt.data[++idx1]);
               strcat(str1,str2);
               arraySend[idx2] = strtol(str1, NULL, 16);
            }
            buf.val = (uint8_t *)malloc((buf.len)*sizeof(uint8_t));
            memcpy(buf.val,(uint8_t *)arraySend,buf.len);
            ASFVOLT_LOG(ASFVOLT_DEBUG,"\nAfter converting it into hex ");
            for(idx2=0; idx2<buf.len; idx2++)
            {
               printf("%02x", buf.val[idx2]);
            }
            printf("\n");
            err = bcmbal_pkt_send(aterm_id,
                  proxy_pkt_dest,
                  (const char *)(buf.val),
                  buf.len);
            ASFVOLT_LOG(ASFVOLT_DEBUG, "\n OMCI request msg of length(%d) sent to ONU(%d) through PON(%d)\n",
                  buf.len,
                  packet->key->packet_send_dest->itu_omci_channel->sub_term_id,
                  packet->key->packet_send_dest->itu_omci_channel->intf_id);
            free(buf.val);
         }
         break;
      case BAL_DEST_TYPE__BAL_DEST_TYPE_IEEE_OAM_CHANNEL:
         {
            ASFVOLT_LOG(ASFVOLT_DEBUG, "\n PLOAM msg not yet implemented\n");
         }
         break;
      default:
         {
            ASFVOLT_LOG(ASFVOLT_DEBUG, "\n Invalid dest type\n");
         }
         break;
   }
   if (BCM_ERR_OK != err)
   {
      /* recover from any error encountered while sending */
   }
   bcmos_mutex_unlock(&bal_ind_lock);
   return err;
}
