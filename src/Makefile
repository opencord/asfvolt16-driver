
CC=/usr/bin/gcc
AR=/usr/bin/ar

LIB=libEdgecoreDriver.a

BRDCM_SRC=/home/asfvolt/shared/OpenNetworkLinux/bal_src_release/bal_release
GRPC_C_PATH=/home/asfvolt/shared/OpenNetworkLinux/grpc-c

GRPC_C_INC= -I$(GRPC_C_PATH)/build/ \
	-I$(GRPC_C_PATH)/build/examples/ \
	-I$(GRPC_C_PATH)/lib/h/

INCL=*.h
SRC=asfvolt16_driver.c \
	bal_access_terminal_hdlr.c \
	bal_flow_hdlr.c \
	bal_group_hdlr.c \
        bal_indications_queue.c \
	bal_indications_hdlr.c \
	bal_interface_hdlr.c \
	bal_packet_hdlr.c \
	bal_stats_hdlr.c \
	bal_subscriber_terminal_hdlr.c \
	bal_tmqueue_hdlr.c \
	bal_tmsched_hdlr.c

C_OPTS=-std=c99 -Wall -Wextra -Wbad-function-cast -Wcast-align -Wcast-qual -Wchar-subscripts -Wmissing-prototypes -Wnested-externs -Wpointer-arith -Wredundant-decls -Wstrict-prototypes -Wparentheses -Wswitch -Wswitch-default -Wunused -Wuninitialized -Wunused-but-set-variable -Wno-unused-parameter -Wno-missing-field-initializers -Wshadow -MMD -MP -Wno-strict-aliasing  -g -gdwarf-3 -O0 -Wall -DBCMOS_MSG_QUEUE_DOMAIN_SOCKET -DBCMOS_MSG_QUEUE_UDP_SOCKET -DBCMOS_MEM_DEBUG -DBCMOS_SYS_UNITTEST -DENABLE_LOG -DDEV_LOG_SYSLOG -DENABLE_CLI -DCONFIG_LINENOISE -DCONFIG_EDITLINE -DQAX_SWITCH -Wno-redundant-decls -DOMCI_SVC -Wno-redundant-decls -DBCM_SUBSYSTEM_CORE -DBCM_PLATFORM_MAPLE -DBCM_OS_POSIX -DBCM_SUBSYSTEM_HOST -D_KERNEL_SPACE -DBCM_EPON -DBCM_GPON -DBCM_XGPON -DCONFIG_MAC_RPC -DCONFIG_SWITCH_RPC -DSIMULATION_BUILD -DBAL_MONOLITHIC -D_XOPEN_SOURCE=600

INC_PATH= -I. -I$(BRDCM_SRC)/src/common/os_abstraction \
	-I$(BRDCM_SRC)/src/common/os_abstraction/posix \
	-I$(BRDCM_SRC)/src/core/platform \
	-I$(BRDCM_SRC)/src/common/config \
	-I$(BRDCM_SRC)/src/common/include \
	-I$(BRDCM_SRC)/build/core/src/common/include \
	-I$(BRDCM_SRC)/src/common/utils \
	-I$(BRDCM_SRC)/build/core/src/common/utils\
	-I$(BRDCM_SRC)/3rdparty/linenoise \
	-I$(BRDCM_SRC)/build/core/3rdparty/linenoise \
	-I$(BRDCM_SRC)/build/core/src/common/bal_dist_utils  \
	-I$(BRDCM_SRC)/src/common/bal_dist_utils  \
	-I$(BRDCM_SRC)/src/common/cli \
	-I$(BRDCM_SRC)/build/core/src/common/cli \
	-I$(BRDCM_SRC)/src/common/dev_log \
	-I$(BRDCM_SRC)/build/core/src/common/dev_log \
	-I$(BRDCM_SRC)/src/common/os_abstraction/os_cli \
	-I$(BRDCM_SRC)/build/core/src/common/os_abstraction/os_cli \
	-I$(BRDCM_SRC)/3rdparty/maple/sdk/host_driver/api \
	-I$(BRDCM_SRC)/3rdparty/maple/sdk/host_driver/host_api \
	-I$(BRDCM_SRC)/3rdparty/maple/sdk/host_customized/embedded_cli \
	-I$(BRDCM_SRC)/3rdparty/maple/sdk/host_customized/os_abstraction/os_cli \
	-I$(BRDCM_SRC)/3rdparty/maple/sdk/host_driver/transport \
	-I$(BRDCM_SRC)/3rdparty/maple/sdk/host_driver/model \
	-I$(BRDCM_SRC)/3rdparty/maple/sdk/host_driver/common_gpon \
	-I$(BRDCM_SRC)/3rdparty/maple/sdk/host_driver/sw_version \
	-I$(BRDCM_SRC)/3rdparty/maple/sdk/host_reference/api_dev_log \
	-I$(BRDCM_SRC)/3rdparty/maple/sdk/host_reference/api_cli \
	-I$(BRDCM_SRC)/3rdparty/maple/sdk/host_reference/device_selector \
	-I$(BRDCM_SRC)/src/lib/libobjmsg \
	-I$(BRDCM_SRC)/build/core/src/lib/libobjmsg \
	-I$(BRDCM_SRC)/src/lib/libbalapi \
	-I$(BRDCM_SRC)/build/core/src/lib/libbalapi \
	-I$(BRDCM_SRC)/src/lib/libbalapicli \
	-I$(BRDCM_SRC)/build/core/src/lib/libbalapicli \
	-I$(BRDCM_SRC)/src/common/bal_app_utils \
	-I$(BRDCM_SRC)/build/core/src/common/bal_app_utils \
	-I$(BRDCM_SRC)/src/balapiend \
	-I$(BRDCM_SRC)/build/core/src/balapiend \
	-I$(BRDCM_SRC)/src/common/bal_intg_app_calls    \
	-I$(BRDCM_SRC)/build/core/src/common/bal_intg_app_calls    \
	-I$(BRDCM_SRC)/src/lib/libautostart \
	-I$(BRDCM_SRC)/build/core/src/lib/libautostart \
	-I$(BRDCM_SRC)/src/lib/libutils \
	-I$(BRDCM_SRC)/build/core/src/lib/libutils \
	-I$(BRDCM_SRC)/src/lib/libtopology \
	-I$(BRDCM_SRC)/build/core/src/lib/libtopology \
	-I$(BRDCM_SRC)/src/core/util/mac \
	-I$(BRDCM_SRC)/build/core/src/core/util/mac \
	-I$(BRDCM_SRC)/3rdparty/bcm-sdk/build/sdk-all-6.5.7/include \
	-I$(BRDCM_SRC)/3rdparty/bcm-sdk/build/sdk-all-6.5.7/libs/phymod/include \
	-I$(BRDCM_SRC)/src/core/util/switch/dpp \
	-I$(BRDCM_SRC)/build/core/src/core/util/switch/dpp \
	-I$(BRDCM_SRC)/src/core/util/switch/esw \
	-I$(BRDCM_SRC)/build/core/src/core/util/switch/esw \
	-I$(BRDCM_SRC)/src/core/util/switch \
	-I$(BRDCM_SRC)/build/core/src/core/util/switch \
	-I$(BRDCM_SRC)/src/lib/librscmgr \
	-I$(BRDCM_SRC)/build/core/src/lib/librscmgr \
	-I$(BRDCM_SRC)/src/lib/libcmdline \
	-I$(BRDCM_SRC)/build/core/src/lib/libcmdline \
	-I$(BRDCM_SRC)/src/core/main \
	-I$(BRDCM_SRC)/build/core/src/core/main  \
	-I$(BRDCM_SRC)/src/core/main \
	-I$(BRDCM_SRC)/src/common/include \
	-I$(BRDCM_SRC)/build/core/src/common/include \
	-I$(BRDCM_SRC)/build/core/include \
	$(GRPC_C_INC)

VERBOSE=@
ifeq ($V, 1)
	VERBOSE=
endif

OBJ=$(SRC:.c=.o)

%.o: %.c
	@echo "(CC) $*.c ..."
	$(VERBOSE) $(CC) $(C_OPTS) $(INC_PATH) -c $*.c

$(LIB):$(OBJ)
	@echo "Creating Archive  $(LIB) ..."
	$(VERBOSE) $(AR) rcs $(LIB) $(OBJ)

all: $(LIB)

clean:
	rm -f *.o \
	rm -f *.d \
	rm -f *.a
