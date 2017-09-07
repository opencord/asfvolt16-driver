GRPC_C DEVICE SIMULATOR

This is a executable(voltha_bal_driver), which will received asfvolt16 adapter grpc-c messages and call respective device stub.
It will send recevied responses/asynchronous indications from Device stub to Adapter.

GETTING STARTED

prerequisites to install grpc
   - sudo apt-get install build-essential libxml2-dev libgeos++-dev libpq-dev libbz2-dev libtool automake openssl libssl-dev golang-goprotobuf-dev


INSTALLATION

Open Network Linux
This guide assumes that Open Network Linux (ONL) is available at ${HOME}/OpenNetworkLinux. All the below steps need to be executed in the ONL build docker context. Refer to ONL build instructions (https://wiki.opencord.org/download/attachments/2556712/ONL_Build_Environment_Installation_Guide.pdf?api=v2).

    - Download ONL source code with Kernel 3.7.10.
```
    > git clone https://github.com/Lewis-Kang/OpenNetworkLinux.git -b kernel_3.7.10
```

    - Start ONL build docker workspace
```
    > cd OpenNetworkLinux
    > docker/tools/onlbuilder -8   # enter the docker workspace
```

Clone asfvolt16-driver
    - The guide assumes that the asfvolt16-driver gerrit repo is cloned under the ONL toplevel directory:
    - git clone ssh://user@gerrit.opencord.org:29418/asfvolt16-driver ${HOME}/OpenNetworkLinux/asfvolt16-driver

steps to install grpc
   - Download as zip "grpc", "grpc-c", "protobuf" and "protobuf-c" from "https://github.com/opennetworkinglab/asfvolt16-driver/tree/master/third_party"
   - extract grpc at ${HOME}/OpenNetworkLinux/grpc
   - extract grpc-c at ${HOME}/OpenNetworkLinux/grpc-c
   - extract protobuf at ${HOME}/OpenNetworkLinux/grpc/thirdparty/protobuf
   - extract protobuf-c folder at ${HOME}/OpenNetworkLinux/grpc-c/third_party/protobuf-c
   - cd ${HOME}/OpenNetworkLinux/grpc/thirdparty/protobuf
      - ./autogen.sh
      - ./configure
      - make
      - sudo make install
   - cd ${HOME}/OpenNetworkLinux/grpc
     - export LD_LIBRARY_PATH=/usr/local/lib
     - make
     - sudo make install

steps to install grpc-c
   - cd ${HOME}/OpenNetworkLinux/grpc-c/third_party/protobuf-c
     - ./autogen.sh
     - ./configure
     - export PKG_CONFIG_PATH=/usr/local/lib/pkgconfig/protobuf
     - make
     - sudo make install

Apply grpc-c patch
   - cd ${HOME}/OpenNetworkLinux/grpc-c/
     Patch 1:
     Apply patch in following link for grpc-c/lib - "https://github.com/Juniper/grpc-c/commit/353b40cd920cd749ed6cf71f8df17f1d5cf2c89d"
     Note:
        (This patch is having very few changes in two files(grpc-c/lib/client.c, grpc-c/lib/service.c. 
         Download these two files from above link and replace at grpc-c/lib or merge these changes manually.)       
     Patch 2:
     Apply patch in service.c.patch and client.c.patch

     - cd ${HOME}/OpenNetworkLinux
     - cp asfvolt16-driver/device_simulator/Makefile.am grpc-c/examples/
     - cp asfvolt16-driver/device_simulator/voltha_bal_driver.c grpc-c/examples/
     - cp asfvolt16-driver/device_simulator/bal_stub.c grpc-c/examples/
     - cp asfvolt16-driver/device_simulator/bal_stub.h grpc-c/examples/
         - Note: Update voltha adaptor IP in bal_stub.c (Is this required? Not clear which variable needs to be updated).

     - cd ${HOME}/OpenNetworkLinux/grpc-c
     - autoreconf --install
     - mkdir build && cd build
     - ../configure
     - make
     - sudo make install
 
To obtain proto files - Have a repo sync of opencord voltha code base:
     - git clone https://github.com/opencord/voltha.git ${HOME}/voltha
     - cd ${HOME}/OpenNetworkLinux/grpc-c/
     - cp ${HOME}/voltha/voltha/adapters/asfvolt16_olt/protos/* examples/

To autogenerate code from proto files:
     - cd ${HOME}/OpenNetworkLinux/grpc-c/build/examples
     - make autogen

Build voltha_bal_driver:
     - cd ${HOME}/OpenNetworkLinux/grpc-c/build/examples
     - Note: Remove "-O2" from Makefile
     - Note: Set EDGECORE and BRCM_PATH in Makefile
```
       EDGECORE = ${HOME}/OpenNetworkLinux/asfvolt16-driver/src/
       BRCM_PATH = ${HOME}/OpenNetworkLinux/bal_src_release/bal_release
```
     - make clean_all;make

The ultimate executable voltha_bal_driver can be found under ~/grpc-c/build/examples/.libs/

USAGE:
    ./voltha_bal_driver "serverIP:port1" -C "serverIP:port2" -A "serverIP:port3"
    ./bal_core_dist -C "serverIP:port2" -A "serverIP:port3"

NOTE: bal_core_dist is a broadcom executable

CONTRIBUTING
     <TBD>
 
VERSIONING
     <TBD>

AUTHORS
     Radisys Adapter Team

Licence
     <TBD>

Acknowledgments
     <TBD>
  

