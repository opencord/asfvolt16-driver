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
uint32_t asfvolt16_bal_stats_get(BalIntfType intf_type, uint32_t intf_id, BalInterfaceStatData *statData)
{
	bcmbal_interface_stat interface_stats;

	bcmos_errno err = BCM_ERR_OK;

	if(intf_type!=BAL_INTF_TYPE__BAL_INTF_TYPE_NNI || intf_type!=BAL_INTF_TYPE__BAL_INTF_TYPE_PON||
		intf_type!=BAL_INTF_TYPE__BAL_INTF_TYPE_EPON_1G_PATH ||
		intf_type!=BAL_INTF_TYPE__BAL_INTF_TYPE_EPON_10G_PATH)
	{
		err = BCM_ERR_PARM;
	}

	if(err == BCM_ERR_OK)
	{
		bcmbal_interface_key key = { .intf_id = intf_id,
								.intf_type = intf_type };

		/* Prepare to retrieve stat on NNI interface 0 */
		BCMBAL_STAT_INIT(&interface_stats, interface, key);

		/* Retrieve the Upstream packet and byte counts */
		BCMBAL_STAT_PROP_GET(&interface_stats, interface, rx_bytes);
		BCMBAL_STAT_PROP_GET(&interface_stats, interface, rx_packets);
		BCMBAL_STAT_PROP_GET(&interface_stats, interface, rx_ucast_packets);
		BCMBAL_STAT_PROP_GET(&interface_stats, interface, rx_mcast_packets);
		BCMBAL_STAT_PROP_GET(&interface_stats, interface, rx_bcast_packets);
		BCMBAL_STAT_PROP_GET(&interface_stats, interface, rx_error_packets);
		BCMBAL_STAT_PROP_GET(&interface_stats, interface, rx_unknown_protos);
		BCMBAL_STAT_PROP_GET(&interface_stats, interface, tx_bytes);
		BCMBAL_STAT_PROP_GET(&interface_stats, interface, tx_packets);
		BCMBAL_STAT_PROP_GET(&interface_stats, interface, tx_ucast_packets);
		BCMBAL_STAT_PROP_GET(&interface_stats, interface, tx_mcast_packets);
		BCMBAL_STAT_PROP_GET(&interface_stats, interface, tx_bcast_packets);
		BCMBAL_STAT_PROP_GET(&interface_stats, interface, tx_error_packets);
		BCMBAL_STAT_PROP_GET(&interface_stats, interface, rx_crc_errors);
		BCMBAL_STAT_PROP_GET(&interface_stats, interface, bip_errors);
   
		/* Read the NNI stats.
			* NOTE: When a CLEAR is specified during a NNI stats GET operation,
			* all of the NNI stats are cleared, even the ones that are not retrieved.
			*/
		err = bcmbal_stat_get(DEFAULT_ATERM_ID, &interface_stats.hdr, BCMOS_TRUE);

		if(err == BCM_ERR_OK)
		{
			statData->rx_bytes = interface_stats.data.rx_bytes;
        	statData->rx_packets = interface_stats.data.rx_packets;
        	statData->rx_ucast_packets = interface_stats.data.rx_ucast_packets;        
        	statData->rx_mcast_packets = interface_stats.data.rx_mcast_packets;
        	statData->rx_bcast_packets = interface_stats.data.rx_bcast_packets;
        	statData->rx_error_packets = interface_stats.data.rx_error_packets;
        	statData->rx_unknown_protos = interface_stats.data.rx_unknown_protos;
        	statData->tx_bytes = interface_stats.data.tx_bytes;
        	statData->tx_packets = interface_stats.data.tx_packets;
        	statData->tx_ucast_packets = interface_stats.data.tx_ucast_packets;
        	statData->tx_mcast_packets = interface_stats.data.tx_mcast_packets;
        	statData->tx_bcast_packets = interface_stats.data.tx_bcast_packets;
        	statData->tx_error_packets = interface_stats.data.tx_error_packets;
        	statData->rx_crc_errors = interface_stats.data.rx_crc_errors;
        	statData->bip_errors = interface_stats.data.bip_errors;
		}
	}

	return err;
}
