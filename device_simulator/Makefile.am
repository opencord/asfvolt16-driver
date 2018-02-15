bin_PROGRAMS = voltha_bal_driver

.PHONY = brcm_lib

HOSTPROG=yes

DEFS += -UBAL_STUB

voltha_bal_driver_SOURCES = \
    voltha_bal_driver.c \
    bal_stub.c \
    grpc-c.h \
    bal_errno.grpc-c.h bal_indications.grpc-c.h bal_model_types.grpc-c.h \
    bal_obj.grpc-c.h bal.grpc-c.h bal_model_ids.grpc-c.h \
    bal_msg_type.grpc-c.h bal_osmsg.grpc-c.h asfvolt.grpc-c.h asfvolt16_driver.h \
    bal.grpc-c.service.c \
    bal_errno.grpc-c.c bal_indications.grpc-c.c \
    bal_model_types.grpc-c.c bal_obj.grpc-c.c asfvolt.grpc-c.c \
    bal_errno.grpc-c.service.c bal_indications.grpc-c.service.c \
    bal_model_types.grpc-c.service.c bal_obj.grpc-c.service.c \
    bal.grpc-c.c bal_model_ids.grpc-c.c \
    bal_msg_type.grpc-c.c bal_osmsg.grpc-c.c \
    bal_model_ids.grpc-c.service.c \
    bal_msg_type.grpc-c.service.c bal_osmsg.grpc-c.service.c \
    asfvolt.grpc-c.service.c

replace:
$(shell find . -type f \( -name '*.h' -o -name '*.c' \) -a -exec sed -i -e 's/bal_obj.proto.h/bal_obj.grpc-c.h/g' {} + )
$(shell find . -type f \( -name '*.h' -o -name '*.c' \) -a -exec sed -i -e 's/bal_model_types.proto.h/bal_model_types.grpc-c.h/g' {} + )
$(shell find . -type f \( -name '*.h' -o -name '*.c' \) -a -exec sed -i -e 's/bal_errno.proto.h/bal_errno.grpc-c.h/g' {} + )
$(shell find . -type f \( -name '*.h' -o -name '*.c' \) -a -exec sed -i -e 's/bal_msg_type.proto.h/bal_msg_type.grpc-c.h/g' {} + )
$(shell find . -type f \( -name '*.h' -o -name '*.c' \) -a -exec sed -i -e 's/bal_osmsg.proto.h/bal_osmsg.grpc-c.h/g' {} + )
$(shell find . -type f \( -name '*.h' -o -name '*.c' \) -a -exec sed -i -e 's/bal_model_ids.proto.h/bal_model_ids.grpc-c.h/g' {} + )
$(shell find . -type f \( -name '*.h' -o -name '*.c' \) -a -exec sed -i -e 's/bal.proto.h/bal.grpc-c.h/g' {} + )
$(shell find . -type f \( -name '*.h' -o -name '*.c' \) -a -exec sed -i -e 's/bal_indications.proto.h/bal_indications.grpc-c.h/g' {} + )
$(shell find . -type f \( -name '*.h' -o -name '*.c' \) -a -exec sed -i -e 's/asfvolt.proto.h/asfvolt.grpc-c.h/g' {} + )

autogen:
	protoc -I ${srcdir} --grpc-c_out=. --plugin=protoc-gen-grpc-c=../compiler/protoc-gen-grpc-c ${srcdir}/bal_errno.proto ${srcdir}/bal_model_ids.proto ${srcdir}/bal_msg_type.proto ${srcdir}/bal_osmsg.proto ${srcdir}/bal_indications.proto ${srcdir}/bal_model_types.proto ${srcdir}/bal_obj.proto ${srcdir}/bal.proto ${srcdir}/asfvolt.proto $(replace)

LDADD = \
    ../lib/libgrpc-c.la \
    /usr/local/lib/libgrpc.a \
    /usr/local/lib/libgpr.a \
    /usr/local/lib/libprotobuf-c.a \
    -lssl \
    -lcrypto \
    -lz \
    -lpthread

EXTRA_voltha_bal_driver_DEPENDENCIES=$(EDGECORE_DRIV_LIB) $(BRCM_BAL_LIB)

EDGECORE = /home/asfvolt/shared/OpenNetworkLinux/EdgeCore/driver-app
BRCM_PATH = /home/asfvolt/shared/OpenNetworkLinux/bal_src_release_2.4.3.6/bal_release
EDGECORE_DRIV_LIB=./libEdgecoreDriver.a
BRCM_BAL_LIB=./libbal_api_dist.so

LIBS = $(EDGECORE_DRIV_LIB) $(BRCM_BAL_LIB) -lrt -lm

AM_CFLAGS = \
    -I. \
    -I${srcdir}/../lib/h/ \
    -I${srcdir}/../third_party/protobuf-c \
    -I${srcdir}/../third_party/grpc/include \
    -I${EDGECORE}/.

$(BRCM_BAL_LIB):
	@cp $(BRCM_PATH)/build/core/lib/libbal_api_dist.so .

$(EDGECORE_DRIV_LIB):
	@cd $(EDGECORE);make 
	@cp $(EDGECORE)/libEdgecoreDriver.a .

edgecore_lib_clean:
	@cd $(EDGECORE);make clean
	@rm -f libEdgecoreDriver.a

clean_all: edgecore_lib_clean clean

all:voltha_bal_driver$(EXEEXT)

