/*
 * Copyright (c) 2016, Juniper Networks, Inc.
 * All rights reserved.
 */

#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include "bal_msg_type.grpc-c.h"
#include "bal_osmsg.grpc-c.h"
#include "bal_model_ids.grpc-c.h"
#include "bal_obj.grpc-c.h"
#include "bal_model_types.grpc-c.h"
#include "bal_errno.grpc-c.h"
#include "bal.grpc-c.h"

#ifdef BAL_STUB
#include "bal_stub.h"
#endif

static grpc_c_server_t *test_server;

static void sigint_handler (int x) { 
        grpc_c_server_destroy(test_server);
        exit(0);
}

/*
 * This functions gets invoked whenever bal RPC gets called
 */
void bal__bal_cfg_get_cb(grpc_c_context_t *context)
{
        BalCfg *get_cfg;

        /*
         * Read incoming message into get_cfg
         */
        if (context->gcc_payload) {
                context->gcc_stream->read(context, (void **)&get_cfg, 0);
        }
}

/*
 * This functions gets invoked whenever bal RPC gets called
 */
void bal__bal_cfg_set_cb(grpc_c_context_t *context)
{
        BalCfg *set_cfg;
        BalErr bal_err;

        /*
         * Read incoming message into set_cfg
         */
        if (context->gcc_payload) {
                context->gcc_stream->read(context, (void **)&set_cfg, 0);
        }

        /*
         * send it to BAL
         */

        bal_err__init(&bal_err);

        bal_err.err= 0;

        /*
         * Write reply back to the client
         */
        if (!context->gcc_stream->write(context, &bal_err, 0)) {
        } else {
                printf("Failed to write\n");
                exit(1);
        }

        grpc_c_status_t status;
        status.gcs_code = 0;

        /*
         * Finish response for RPC
         */
        if (context->gcc_stream->finish(context, &status)) {
                printf("Failed to write status\n");
                exit(1);
        }

#ifdef BAL_STUB
        pthread_mutex_lock(&lock);
        if(set_cfg->hdr->has_obj_type)
        {
                enQueue(set_cfg->hdr->obj_type, BAL_ERRNO__BAL_ERR_OK);
                switch(set_cfg->hdr->obj_type)
                {
                        case BAL_OBJ_ID__BAL_OBJ_ID_ACCESS_TERMINAL:
                                {
                                        printf("\n***************************************************\n");  
                                        printf("Received Access Terminal Configuration msg\n");
                                        printf("***************************************************\n");  
                                }
                                break;
                        case BAL_OBJ_ID__BAL_OBJ_ID_INTERFACE:
                                {
                                        printf("\n***************************************************\n");  
                                        printf("Received PON Interface Configuration msg\n");
                                        printf("***************************************************\n");  
                                }
                                break;
                        case BAL_OBJ_ID__BAL_OBJ_ID_SUBSCRIBER_TERMINAL:
                                {
                                        printf("\n*****************************************************\n");  
                                        printf("Received ONU Activation msg\n");
                                        printf("*****************************************************\n");  
                                }
                        default:
                                break;
                }
        }
        else
        {
                printf("BALSTUB:Cfg Set recevied without object type");
        }
        pthread_mutex_unlock(&lock);
        sleep(2);   
        pthread_cond_signal(&cv);
        if(BAL_OBJ_ID__BAL_OBJ_ID_INTERFACE == set_cfg->hdr->obj_type)
        {
                sleep(5); 
                enQueue(BAL_OBJ_ID__BAL_OBJ_ID_SUBSCRIBER_TERMINAL, BAL_ERRNO__BAL_ERR_OK);
                pthread_cond_signal(&cv);
        }
#endif
}


/*
 * This functions gets invoked whenever bal clear RPC gets called
 */
void bal__bal_cfg_clear_cb(grpc_c_context_t *context)
{
        BalKey *clear_key;

        /*
         * Read incoming message into clear_key 
         */
        if (context->gcc_payload) {
                context->gcc_stream->read(context, (void **)&clear_key, 0);
        }

}


/*
 * This functions gets invoked whenever bal Init RPC gets called
 */
void bal__bal_api_init_cb(grpc_c_context_t *context)
{
        BalInit *bal_init;

        /*
         * Read incoming message into set_cfg
         */
        if (context->gcc_payload) {
                context->gcc_stream->read(context, (void **)&bal_init, 0);
        }

}


/*
 * This functions gets invoked whenever bal finish RPC gets called
 */
void bal__bal_api_finish_cb(grpc_c_context_t *context)
{
#if 0
        void *finish_init;

        /*
         * Read incoming message into set_cfg
         */
        if (context->gcc_payload) {
                context->gcc_stream->read(context, (void **)&finish_init);
        }
#endif
}


/*
 * This functions gets invoked whenever bal finish RPC gets called
 */
void bal_ind__bal_ind_info_cb(grpc_c_context_t *context)
{
#if 0
        void *finish_init;

        /*
         * Read incoming message into set_cfg
         */
        if (context->gcc_payload) {
                context->gcc_stream->read(context, (void **)&finish_init);
        }
#endif
}


/*
 * This functions gets invoked whenever bal RPC gets called
 */
void bal__bal_omci_msg_cb(grpc_c_context_t *context)
{
        char *omci_msg;

        /*
         * Read incoming message into omci_msg
         */
        if (context->gcc_payload) {
                context->gcc_stream->read(context, (void **)&omci_msg, 0);
        }
}

/*
 * Takes socket path as argument
 */
int main (int argc, char **argv) 
{
        int i = 0;
        grpc_c_server_t *server = NULL;

        printf("*************\n");
        printf("BAL STUB\n");
        printf("*************\n");
        if (argc < 2) {
                fprintf(stderr, "Missing socket path argument\n");
                exit(1);
        }

        signal(SIGINT, sigint_handler);
        /*
         * Initialize grpc-c library to be used with vanilla gRPC
         */
        grpc_c_init(GRPC_THREADS, NULL);

        /*
         * Create server object
         */
        test_server = grpc_c_server_create(argv[1]); 
        if (test_server == NULL) {
                printf("Failed to create server\n");
                exit(1);
        }

        /*
         * Initialize greeter service
         */
        printf("\nCreating a test server\n");
        bal__service_init(test_server);

        /*
         * Start server
         */
        grpc_c_server_start(test_server);

#ifdef BAL_STUB
        printf("\nCreating a stub thread\n");
        create_stub_thread();
#endif

        /*
         * Blocks server to wait to completion
         */
        grpc_c_server_wait(test_server);

        bal__service_init(server);

}
