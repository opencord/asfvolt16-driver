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
   err = bcmbal_stat_get(DEFAULT_ATERM_ID, &interface_stats.hdr, BCMOS_FALSE);

   if(err == BCM_ERR_OK)
   {
      /*interface key*/
      statKey->has_intf_id = BAL_ELEMENT_PRES;
      statKey->intf_id = interface_stats.key.intf_id;
      statKey->has_intf_type = BAL_ELEMENT_PRES;
      statKey->intf_type = interface_stats.key.intf_type;

      /*interface stat data*/
      statData->has_rx_bytes = BAL_ELEMENT_PRES;
      statData->rx_bytes = interface_stats.data.rx_bytes;
      statData->has_rx_packets = BAL_ELEMENT_PRES;
      statData->rx_packets = interface_stats.data.rx_packets;
      statData->has_rx_data_bytes = BAL_ELEMENT_PRES;
      statData->rx_data_bytes = interface_stats.data.rx_data_bytes;
      statData->has_rx_ucast_packets = BAL_ELEMENT_PRES;
      statData->rx_ucast_packets = interface_stats.data.rx_ucast_packets;
      statData->has_rx_mcast_packets = BAL_ELEMENT_PRES;
      statData->rx_mcast_packets = interface_stats.data.rx_mcast_packets;
      statData->has_rx_bcast_packets = BAL_ELEMENT_PRES;
      statData->rx_bcast_packets = interface_stats.data.rx_bcast_packets;
      statData->rx_64_packets = BAL_ELEMENT_PRES;
      statData->rx_64_packets = interface_stats.data.rx_64_packets;
      statData->has_rx_65_127_packets = BAL_ELEMENT_PRES;
      statData->rx_65_127_packets = interface_stats.data.rx_65_127_packets;
      statData->has_rx_128_255_packets = BAL_ELEMENT_PRES;
      statData->rx_128_255_packets = interface_stats.data.rx_128_255_packets;
      statData->has_rx_256_511_packets = BAL_ELEMENT_PRES;
      statData->rx_256_511_packets = interface_stats.data.rx_256_511_packets;
      statData->has_rx_512_1023_packets = BAL_ELEMENT_PRES;
      statData->rx_512_1023_packets = interface_stats.data.rx_512_1023_packets;
      statData->has_rx_1024_1518_packets = BAL_ELEMENT_PRES;
      statData->rx_1024_1518_packets = interface_stats.data.rx_1024_1518_packets;
      statData->has_rx_1519_2047_packets = BAL_ELEMENT_PRES;
      statData->rx_1519_2047_packets = interface_stats.data.rx_1519_2047_packets;
      statData->has_rx_2048_4095_packets = BAL_ELEMENT_PRES;
      statData->rx_2048_4095_packets = interface_stats.data.rx_2048_4095_packets;
      statData->has_rx_4096_9216_packets = BAL_ELEMENT_PRES;
      statData->rx_4096_9216_packets = interface_stats.data.rx_4096_9216_packets;
      statData->has_rx_9217_16383_packets = BAL_ELEMENT_PRES;
      statData->rx_9217_16383_packets = interface_stats.data.rx_9217_16383_packets;
      statData->has_rx_error_packets = BAL_ELEMENT_PRES;
      statData->rx_error_packets = interface_stats.data.rx_error_packets;
      statData->has_rx_unknown_protos = BAL_ELEMENT_PRES;
      statData->rx_unknown_protos = interface_stats.data.rx_unknown_protos;
      statData->has_rx_crc_errors = BAL_ELEMENT_PRES;
      statData->rx_crc_errors = interface_stats.data.rx_crc_errors;
      statData->has_bip_errors = BAL_ELEMENT_PRES;
      statData->bip_errors = interface_stats.data.bip_errors;
      statData->has_rx_mpcp = BAL_ELEMENT_PRES;
      statData->rx_mpcp = interface_stats.data.rx_mpcp;
      statData->has_rx_report = BAL_ELEMENT_PRES;
      statData->rx_report = interface_stats.data.rx_report;
      statData->has_rx_oam_bytes = BAL_ELEMENT_PRES;
      statData->rx_oam_bytes = interface_stats.data.rx_oam_bytes;
      statData->has_rx_oam_packets = BAL_ELEMENT_PRES;
      statData->rx_oam_packets = interface_stats.data.rx_oam_packets;
      statData->has_tx_bytes = BAL_ELEMENT_PRES;
      statData->tx_bytes = interface_stats.data.tx_bytes;
      statData->has_tx_packets = BAL_ELEMENT_PRES;
      statData->tx_packets = interface_stats.data.tx_packets;
      statData->has_tx_data_bytes = BAL_ELEMENT_PRES;
      statData->tx_data_bytes = interface_stats.data.tx_data_bytes;
      statData->has_tx_ucast_packets = BAL_ELEMENT_PRES;
      statData->tx_ucast_packets = interface_stats.data.tx_ucast_packets;
      statData->has_tx_mcast_packets = BAL_ELEMENT_PRES;
      statData->tx_mcast_packets = interface_stats.data.tx_mcast_packets;
      statData->has_tx_bcast_packets = BAL_ELEMENT_PRES;
      statData->tx_bcast_packets = interface_stats.data.tx_bcast_packets;
      statData->has_tx_64_packets = BAL_ELEMENT_PRES;
      statData->tx_64_packets = interface_stats.data.tx_64_packets;
      statData->has_tx_65_127_packets = BAL_ELEMENT_PRES;
      statData->tx_65_127_packets = interface_stats.data.tx_65_127_packets;
      statData->has_tx_128_255_packets = BAL_ELEMENT_PRES;
      statData->tx_128_255_packets = interface_stats.data.tx_128_255_packets;
      statData->has_tx_256_511_packets = BAL_ELEMENT_PRES;
      statData->tx_256_511_packets = interface_stats.data.tx_256_511_packets;
      statData->has_tx_512_1023_packets = BAL_ELEMENT_PRES;
      statData->tx_512_1023_packets = interface_stats.data.tx_512_1023_packets;
      statData->has_tx_1024_1518_packets = BAL_ELEMENT_PRES;
      statData->tx_1024_1518_packets = interface_stats.data.tx_1024_1518_packets;
      statData->has_tx_1519_2047_packets = BAL_ELEMENT_PRES;
      statData->tx_1519_2047_packets = interface_stats.data.tx_1519_2047_packets;
      statData->has_tx_2048_4095_packets = BAL_ELEMENT_PRES;
      statData->tx_2048_4095_packets = interface_stats.data.tx_2048_4095_packets;
      statData->has_tx_4096_9216_packets = BAL_ELEMENT_PRES;
      statData->tx_4096_9216_packets = interface_stats.data.tx_4096_9216_packets;
      statData->has_tx_9217_16383_packets = BAL_ELEMENT_PRES;
      statData->tx_9217_16383_packets = interface_stats.data.tx_9217_16383_packets;
      statData->has_tx_error_packets = BAL_ELEMENT_PRES;
      statData->tx_error_packets = interface_stats.data.tx_error_packets;
      statData->has_tx_mpcp = BAL_ELEMENT_PRES;
      statData->tx_mpcp = interface_stats.data.tx_mpcp;
      statData->has_tx_gate = BAL_ELEMENT_PRES;
      statData->tx_gate = interface_stats.data.tx_gate;
      statData->has_tx_oam_bytes = BAL_ELEMENT_PRES;
      statData->tx_oam_bytes = interface_stats.data.tx_oam_bytes;
      statData->has_tx_oam_packets = BAL_ELEMENT_PRES;
      statData->tx_oam_packets = interface_stats.data.tx_oam_packets;
   }

   return err;
}
