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

#include "bal_stats_hdlr.h"

#define BAL_STAT_PRES   1

/********************************************************************\
 * Function : asfvolt16_bal_stats_get                               *
 * Description : Function to get stats based on                     *
 *               interface type(NNI/PON/EPON 1G/EPON 10G)           *
 *               port number                                        *
 ********************************************************************/
uint32_t asfvolt16_bal_stats_get(BalIntfType intf_type, uint32_t intf_id, BalInterfaceStatData *statData,
                                 BalInterfaceKey *statKey)
{
   bcmbal_interface_stat interface_stats;

   bcmos_errno err = BCM_ERR_OK;

   ASFVOLT_LOG(ASFVOLT_DEBUG, "Get Stats from OLT intf_type %d",intf_type);

   if(intf_type!=BAL_INTF_TYPE__BAL_INTF_TYPE_NNI && intf_type!=BAL_INTF_TYPE__BAL_INTF_TYPE_PON &&
	intf_type!=BAL_INTF_TYPE__BAL_INTF_TYPE_EPON_1G_PATH &&
	intf_type!=BAL_INTF_TYPE__BAL_INTF_TYPE_EPON_10G_PATH)
   {
      return BCM_ERR_PARM;
   }

   bcmbal_interface_key key = { .intf_id = intf_id, .intf_type = intf_type };

   /* Prepare to retrieve stat on NNI interface 0 */
   BCMBAL_STAT_INIT(&interface_stats, interface, key);

   /* Retrieve the Upstream packet and byte counts */
   BCMBAL_STAT_PROP_GET(&interface_stats, interface, all_properties);
   ASFVOLT_LOG(ASFVOLT_DEBUG, "Retrieve the Upstream packet and byte count success");

   /* Read the NNI stats.
    * NOTE: When a CLEAR is specified during a NNI stats GET operation,
    * all of the NNI stats are cleared, even the ones that are not retrieved.
    */
   err = bcmbal_stat_get(DEFAULT_ATERM_ID, &interface_stats.hdr, BCMOS_TRUE);

   if(err == BCM_ERR_OK)
   {
      /*interface key*/
      statKey->has_intf_id = BAL_STAT_PRES;
      statKey->intf_id = interface_stats.key.intf_id;
      statKey->has_intf_type = BAL_STAT_PRES;
      statKey->intf_type = interface_stats.key.intf_type;

      /*interface stat data*/
      statData->has_rx_bytes = BAL_STAT_PRES;
      statData->rx_bytes = interface_stats.data.rx_bytes;
      statData->has_rx_packets = BAL_STAT_PRES;
      statData->rx_packets = interface_stats.data.rx_packets;
      statData->has_rx_ucast_packets = BAL_STAT_PRES;
      statData->rx_ucast_packets = interface_stats.data.rx_ucast_packets;
      statData->has_rx_mcast_packets = BAL_STAT_PRES;
      statData->rx_mcast_packets = interface_stats.data.rx_mcast_packets;
      statData->has_rx_bcast_packets = BAL_STAT_PRES;
      statData->rx_bcast_packets = interface_stats.data.rx_bcast_packets;
      statData->has_rx_error_packets = BAL_STAT_PRES;
      statData->rx_error_packets = interface_stats.data.rx_error_packets;
      statData->has_rx_unknown_protos = BAL_STAT_PRES;
      statData->rx_unknown_protos = interface_stats.data.rx_unknown_protos;
      statData->has_tx_bytes = BAL_STAT_PRES;
      statData->tx_bytes = interface_stats.data.tx_bytes;
      statData->has_tx_packets = BAL_STAT_PRES;
      statData->tx_packets = interface_stats.data.tx_packets;
      statData->has_tx_ucast_packets = BAL_STAT_PRES;
      statData->tx_ucast_packets = interface_stats.data.tx_ucast_packets;
      statData->has_tx_mcast_packets = BAL_STAT_PRES;
      statData->tx_mcast_packets = interface_stats.data.tx_mcast_packets;
      statData->has_tx_bcast_packets = BAL_STAT_PRES;
      statData->tx_bcast_packets = interface_stats.data.tx_bcast_packets;
      statData->has_tx_error_packets = BAL_STAT_PRES;
      statData->tx_error_packets = interface_stats.data.tx_error_packets;
      statData->has_rx_crc_errors = BAL_STAT_PRES;
      statData->rx_crc_errors = interface_stats.data.rx_crc_errors;
      statData->has_bip_errors = BAL_STAT_PRES;
      statData->bip_errors = interface_stats.data.bip_errors;
   }

   return err;
}
