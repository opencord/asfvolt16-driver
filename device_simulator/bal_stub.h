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

#include "bal_msg_type.grpc-c.h"
#include "bal_osmsg.grpc-c.h"
#include "bal_model_ids.grpc-c.h"
#include "bal_obj.grpc-c.h"
#include "bal_model_types.grpc-c.h"
#include "bal_errno.grpc-c.h"
#include "bal_indications.grpc-c.h"
#include "bal.grpc-c.h"

/* Global definations */
pthread_cond_t cv;
pthread_mutex_t lock;
unsigned int is_stub_reboot;

#define IND_USR_DAT_LEN  8
#define IND_USR_DAT_VAL  "brcmOLT"
#define BALCLIENT        "bal_client"
#define BALSERVER        "bal_server"
#define BAL_DEVICE_STR_LEN 20
#define ASFVOLT_ERROR 1
#define ASFVOLT_INFO  2
#define ASFVOLT_DEBUG 3

typedef struct BalCoreIpInfo
{
   char bal_core_arg1[4];
   char bal_core_ip_port[24];
   char bal_core_arg2[4];
   char bal_shared_lib_ip_port[24];
}balCoreIpInfo;

/* A linked list (LL) node to store a queue entry */
struct QNode
{
	int obj_type;
	char device_id[BAL_DEVICE_STR_LEN];
	int status;
	int intf_id;
	int onu_id;
	char vendor_id[BAL_DEVICE_STR_LEN];
	char vendor_specific[BAL_DEVICE_STR_LEN];
	struct QNode *next;
};


/* The queue, front stores the front node of LL and rear stores ths
   last node of LL */
typedef struct Queue
{
	struct QNode *front, *rear;
}bal_queue;

#define ASFVOLT_LOG(log_type, format, args...) \
   if(log_type) \
   {\
      printf("File(%s): Line(%d): ", __FUNCTION__, __LINE__);\
      printf(format, ## args);\
   }

/* shared queue */
bal_queue *shared_queue;

void create_stub_thread();
bal_queue *createQueue();
struct QNode* newNode(int objKey, int status, char *device_id);
void enQueue(int objKey, struct QNode *temp);
struct QNode *deQueue();
grpc_c_client_t *client;
void stub_bal_init(BalInit *bal_init);
