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

extern uint32_t bal_tm_sched_cfg_set(BalTmSchedCfg *tm_sched_cfg);
extern uint32_t bal_tm_sched_cfg_get(BalTmSchedCfg *tm_sched_cfg);
extern uint32_t bal_tm_sched_cfg_clear(BalTmSchedKey *tm_sched_key);
extern bcmos_errno bal_tm_sched_cfg_indication_cb(bcmbal_obj *obj);

