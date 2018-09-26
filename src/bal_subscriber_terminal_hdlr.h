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

extern uint32_t bal_subscriber_terminal_cfg_set(BalSubscriberTerminalCfg *onu_cfg);
extern bcmos_errno bal_subscriber_terminal_indication_cb(bcmbal_obj *obj);
extern uint32_t bal_subscriber_terminal_cfg_clear(BalSubscriberTerminalKey *terminal_key);
extern uint32_t bal_subscriber_terminal_cfg_get(BalSubscriberTerminalCfg *onu_cfg);

#define BAL_DYNAMIC_LIST_BUFFER_SIZE (32 * 1024)
