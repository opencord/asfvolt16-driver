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

#ifndef DRIVERAPP_ASFVOLT16_DRIVER
#define DRIVERAPP_ASFVOLT16_DRIVER

/* Includes related to proto buf */
#include "bal_msg_type.grpc-c.h"
#include "bal_osmsg.grpc-c.h"
#include "bal_model_ids.grpc-c.h"
#include "bal_obj.grpc-c.h"
#include "bal_model_types.grpc-c.h"
#include "bal_errno.grpc-c.h"
#include "bal.grpc-c.h"

#define ASFVOLT_ERROR 1
#define ASFVOLT_INFO  2
#define ASFVOLT_DEBUG 3
#define ASFVOLT_MAX_PKT_SIZE 500
#define ASFVOLT_MAX_DEVICE_ID_SIZE 50
#define MAX_CHAR_LENGTH  20
#define MAX_OMCI_MSG_LENGTH 44

char voltha_device_id[ASFVOLT_MAX_DEVICE_ID_SIZE];

typedef struct BalCoreIpInfo
{
   char bal_core_arg1[4]; 
   char bal_core_ip_port[24]; 
   char bal_core_arg2[4]; 
   char bal_shared_lib_ip_port[24]; 
}balCoreIpInfo;

#define ASFVOLT_LOG(log_type, format, args...) \
   if(log_type) \
   {\
      printf("File(%s): Line(%d): ", __FUNCTION__, __LINE__);\
      printf(format, ## args);\
   }


#define ASFVOLT_CFG_PROP_SET(cfg, obj, param, pram_prst, param_val) \
   if(pram_prst)\
   {\
       BCMBAL_CFG_PROP_SET(&cfg, obj, param, param_val);\
   }

/***************************************************************
*                                                              *
*              Function Declarations                           *
*                                                              *
****************************************************************/
extern uint32_t asfvolt16_bal_init(BalInit *bal_init, balCoreIpInfo *coreInfo);
extern uint32_t asfvolt16_bal_finish(void);
extern uint32_t bal_register_indication_cbs(void);
extern uint32_t asfvolt16_bal_cfg_set(BalCfg *cfg);
extern uint32_t asfvolt16_bal_cfg_clear(BalKey *key);
extern uint32_t asfvolt16_bal_cfg_get(BalKey *key, BalCfg *cfg);
extern void asfvolt16_send_omci_msg(uint16_t olt_no, uint32_t onu_nu, char* packet_to_send);
extern uint32_t asfvolt16_bal_stats_get(BalIntfType intf_type, uint32_t intf_id, BalInterfaceStatData *statData);
grpc_c_client_t *client;

#endif /* DRIVERAPP_ASFVOLT16_DRIVER */
