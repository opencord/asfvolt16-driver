
/* Global definations */
pthread_cond_t cv;
pthread_mutex_t lock;

#define IND_USR_DAT_LEN  8
#define IND_USR_DAT_VAL  "brcmOLT"
#define BALCLIENT        "bal_client"
#define BALSERVER        "bal_server"
#define BAL_DEVICE_STR_LEN 20

/* A linked list (LL) node to store a queue entry */
struct QNode
{
	int obj_type;
	char device_id[BAL_DEVICE_STR_LEN];
	int status;
	int intf_id;
	int onu_id;
	struct QNode *next;
};


/* The queue, front stores the front node of LL and rear stores ths
   last node of LL */
typedef struct Queue
{
	struct QNode *front, *rear;
}bal_queue;

/* shared queue */
bal_queue *shared_queue;

void create_stub_thread(); 
bal_queue *createQueue();
struct QNode* newNode(int objKey, int status, char *device_id);
void enQueue(int objKey, struct QNode *temp);
struct QNode *deQueue();
