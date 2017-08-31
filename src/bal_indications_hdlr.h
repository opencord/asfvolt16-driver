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
#include "bal_indications.grpc-c.h"
#include "bal.grpc-c.h"

#include "asfvolt16_driver.h"

#define BAL_GRPC_PRES    1 

extern bcmos_errno bal_indication_cb(bcmbal_obj *obj);
extern bcmos_errno bal_acc_term_osc_indication_cb(bcmbal_obj *obj);
extern bcmos_errno bal_acc_term_indication_cb(bcmbal_obj *obj);
extern bcmos_errno bal_flow_osc_indication_cb(bcmbal_obj *obj);
extern bcmos_errno bal_flow_indication_cb(bcmbal_obj *obj);
extern bcmos_errno bal_group_indication_cb(bcmbal_obj *obj);
extern bcmos_errno bal_interface_osc_indication_cb(bcmbal_obj *obj);
extern bcmos_errno bal_interface_los_indication_cb(bcmbal_obj *obj);
extern bcmos_errno bal_interface_indication_cb(bcmbal_obj *obj);
extern bcmos_errno bal_sub_term_osc_indication_cb(bcmbal_obj *obj);
extern bcmos_errno bal_sub_term_disc_indication_cb(bcmbal_obj *obj);
extern bcmos_errno bal_sub_term_alarm_indication_cb(bcmbal_obj *obj);
extern bcmos_errno bal_sub_term_dgi_indication_cb(bcmbal_obj *obj);
extern bcmos_errno bal_sub_term_indication_cb(bcmbal_obj *obj);
extern bcmos_errno bal_tm_queue_indication_cb(bcmbal_obj *obj);
extern bcmos_errno bal_tm_sched_indication_cb(bcmbal_obj *obj);
extern bcmos_errno bal_packet_data_indication_cb(bcmbal_obj *obj);
extern bcmos_errno bal_omci_data_indication_cb(bcmbal_obj *obj);
extern bcmos_errno bal_oam_data_indication_cb(bcmbal_obj *obj);
void fill_bal_tm_red(BalTmred *grpc_red, bcmbal_tm_red *bal_red);
