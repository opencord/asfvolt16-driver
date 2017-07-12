GRPC_C DEVICE SIMULATOR

 This is a executable(bal_server), which will recevie asfvolt16 adapter grpc-c messages and call respective device stub.
 It will send recevied responses/asynchronous indications from Device stub to Adapter.

GETTING STARTED

prerequisites to install grpc
   - sudo apt-get install build-essential libxml2-dev libgeos++-dev libpq-dev libbz2-dev libtool automake openssl libssl-dev golang-goprotobuf-dev


INSTALLATION

steps to install grpc
   - Download as zip "grpc", "grpc-c", "protobuf" and "protobuf-c" from "https://github.com/opennetworkinglab/asfvolt16-driver/tree/master/third_party"
   - place downloaded protobuf folder at grpc/thirdparty
   - place downloaded protobuf-c folder at grpc-c/thirdparty 
   - cd grpc/thirdparty/protobuf
      - ./autogen.sh
      - ./configure
      - make
      - sudo make install
   - cd grpc/
     - make 
     - sudo make install

steps to install grpc-c 
   - cd grpc-c/thirdparty/protobuf-c
     - ./autogen.sh
     - ./configure
     - export PKG_CONFIG_PATH=/usr/local/lib/pkgconfig/protobuf 
     - export LD_LIBRARY_PATH=/usr/local/lib
     - make
     - sudo make install
   
Apply grpc-c patch 
   - cd grpc-c/ 
     Patch 1:
     Apply patch in following link for grpc-c/lib - "https://github.com/Juniper/grpc-c/commit/353b40cd920cd749ed6cf71f8df17f1d5cf2c89d"
     Note:
        (This patch is having very few changes in two files(grpc-c/lib/client.c, grpc-c/lib/service.c. 
         Download these two files from above link and replace at grpc-c/lib or merge these changes manually.)       
     Patch 2:
     Apply patch in service.c.patch and client.c.patch 
     - cp Makefile.am grpc-c/examples/
     - cp bal_server.c grpc-c/examples/
     - cp bal_stub.c grpc-c/examples/
     - cp bal_stub.h grpc-c/examples/
     - autoreconf --install
     - mkdir build && cd build
     - ../configure
     - make
     - sudo make install
   
To obtain proto files - Have a repo sync of opencord code base:
     - cp opencord/incubator/voltha/voltha/adapters/asfvolt16_olt/protos/* examples/

To autogenerate code from proto files:
     - make autoGenrate
       
Build bal_server:
     - make clean_all
     - make V=0 

The ultimate executable bal_server can be found under ~/grpc-c/build/examples/.libs/

USAGE
    ./bal_server test

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
  

