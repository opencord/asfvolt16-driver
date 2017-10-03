#
# Copyright 2017-present Open Networking Foundation
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
#
# Run from OpenNetworkLinux top directory
#
#cd /aux/OpenNetworkLinux
set -e
MAKE_JOBS=4
ASFVOLT_REPO_NAME=asfvolt16-driver

# override shell variables to match custom local build environment
: ${ONL_TOPDIR:=`pwd`}
: ${BALSRC_RELEASE=bal_src_release}
: ${VOLTHA_TOPDIR:=${HOME}/voltha/incubator/voltha}
: ${BALSRC_TOPDIR:=${ONL_TOPDIR}/${BALSRC_RELEASE}/bal_release}
: ${ASFSRC_TOPDIR:=${ONL_TOPDIR}/${ASFVOLT_REPO_NAME}/src}
: ${DEVSIM_TOPDIR:=${ONL_TOPDIR}/${ASFVOLT_REPO_NAME}/device_simulator}
: ${PATCHF_TOPDIR:=${DEVSIM_TOPDIR}}

echo ONL_TOPDIR=${ONL_TOPDIR}
echo BALSRC_RELEASE=${BALSRC_RELEASE}
echo VOLTHA_TOPDIR=${VOLTHA_TOPDIR}
echo BALSRC_TOPDIR=${BALSRC_TOPDIR}
echo ASFSRC_TOPDIR=${ASFSRC_TOPDIR}
echo DEVSIM_TOPDIR=${DEVSIM_TOPDIR}
echo PATCHF_TOPDIR=${PATCHF_TOPDIR}

# archived ZIP files from "https://github.com/opennetworkinglab/asfvolt16-driver/tree/master/third_party"
GRPC_ARCH=ed7d06af3eef1c27f10328c73b3ae3ab10d72b10
GRPC_C_ARCH=be82ab1605717f33e2e0d3038996ea46d9efe25e
PROTOBUF_ARCH=703cd8e11c8d34283d4c8bf869c61866e8211c9d
PROTOBUF_C_ARCH=6a4f9a9a67c06769aaa9f65e8f89a56483271f5a

# Note: removes existing directories: asfvolt16-driver, grpc, protobuf
rm -rf ${ASFVOLT_REPO_NAME}
rm -rf grpc protobuf grpc-* protobuf-*

#Clone asfvolt16-driver
#    - The guide assumes that the asfvolt16-driver gerrit repo is cloned under the ONL toplevel directory:
#    - git clone ssh://user@gerrit.opencord.org:29418/asfvolt16-driver ${HOME}/OpenNetworkLinux/asfvolt16-driver

git clone https://gerrit.opencord.org/${ASFVOLT_REPO_NAME} ${ONL_TOPDIR}/${ASFVOLT_REPO_NAME}

#steps to install grpc
#   - Download as zip "grpc", "grpc-c", "protobuf" and "protobuf-c" from "https://github.com/opennetworkinglab/asfvolt16-driver/tree/master/third_party"
wget https://github.com/grpc/grpc/archive/${GRPC_ARCH}.zip
wget https://github.com/Juniper/grpc-c/archive/${GRPC_C_ARCH}.zip
wget https://github.com/google/protobuf/archive/${PROTOBUF_ARCH}.zip
wget https://github.com/shadansari/protobuf-c/archive/${PROTOBUF_C_ARCH}.zip

#   - extract grpc at ${HOME}/OpenNetworkLinux/grpc
unzip -q ${GRPC_ARCH}.zip; rm ${GRPC_ARCH}.zip
mv grpc-${GRPC_ARCH} grpc

#   - extract grpc-c at ${HOME}/OpenNetworkLinux/grpc-c
unzip -q ${GRPC_C_ARCH}.zip; rm ${GRPC_C_ARCH}.zip
mv grpc-c-${GRPC_C_ARCH} grpc-c

#   - extract protobuf at ${HOME}/OpenNetworkLinux/grpc/thirdparty/protobuf
unzip -q ${PROTOBUF_ARCH}.zip; rm ${PROTOBUF_ARCH}.zip
mkdir -p grpc/thirdparty
mv protobuf-${PROTOBUF_ARCH} grpc/thirdparty/protobuf


#   - extract protobuf-c folder at ${HOME}/OpenNetworkLinux/grpc-c/third_party/protobuf-c
unzip -q ${PROTOBUF_C_ARCH}.zip; rm ${PROTOBUF_C_ARCH}.zip
(cd protobuf-c-${PROTOBUF_C_ARCH};tar cf - .)|(cd grpc-c/third_party/protobuf-c;tar xf -)
rm -rf protobuf-c-${PROTOBUF_C_ARCH}

#steps to install grpc-c
# cd ${HOME}/OpenNetworkLinux/grpc/thirdparty/protobuf
#  - ./autogen.sh
#  - ./configure
#  - make
#  - sudo make install
cd ${ONL_TOPDIR}/grpc/thirdparty/protobuf
./autogen.sh
./configure
make --jobs=${MAKE_JOBS}
sudo make install

# cd ${HOME}/OpenNetworkLinux/grpc
# - export LD_LIBRARY_PATH=/usr/local/lib
# - make
# - sudo make install

cd ${ONL_TOPDIR}/grpc
export LD_LIBRARY_PATH=/usr/local/lib
make --jobs=${MAKE_JOBS}
sudo make install

#cd ${HOME}/OpenNetworkLinux/grpc-c/third_party/protobuf-c
#- ./autogen.sh
#- ./configure
#- export PKG_CONFIG_PATH=/usr/local/lib/pkgconfig/protobuf
#- make
#- sudo make install

cd ${ONL_TOPDIR}/grpc-c/third_party/protobuf-c
./autogen.sh
./configure
export PKG_CONFIG_PATH=/usr/local/lib/pkgconfig/protobuf
make --jobs=${MAKE_JOBS}
sudo make install

#Apply grpc-c patch
#   - cd ${HOME}/OpenNetworkLinux/grpc-c/
cd ${ONL_TOPDIR}/grpc-c
#     Patch 1:
#     Apply patch in following link for grpc-c/lib - "https://github.com/Juniper/grpc-c/commit/353b40cd920cd749ed6cf71f8df17f1d5cf2c89d"
#     Note:
#        (This patch is having very few changes in two files(grpc-c/lib/client.c, grpc-c/lib/service.c. 
#         Download these two files from above link and replace at grpc-c/lib or merge these changes manually.)       
patch -p1 -i ${PATCHF_TOPDIR}/grpc-c_compile-error.patch

#     Patch 2:
#     Apply patch in service.c.patch and client.c.patch
patch -p1 -i ${PATCHF_TOPDIR}/grpc-c_asfvolt16.patch

#
#     - cd ${HOME}/OpenNetworkLinux
#     - cp asfvolt16-driver/device_simulator/Makefile.am grpc-c/examples/
#     - cp asfvolt16-driver/device_simulator/voltha_bal_driver.c grpc-c/examples/
#     - cp asfvolt16-driver/device_simulator/bal_stub.c grpc-c/examples/
#     - cp asfvolt16-driver/device_simulator/bal_stub.h grpc-c/examples/
#         - Note: Update voltha adaptor IP in bal_stub.c (Is this required? Not clear which variable needs to be updated).
cd ${ONL_TOPDIR}
cp ${DEVSIM_TOPDIR}/Makefile.am grpc-c/examples/
cp ${DEVSIM_TOPDIR}/voltha_bal_driver.c grpc-c/examples/
cp ${DEVSIM_TOPDIR}/bal_stub.c grpc-c/examples/
cp ${DEVSIM_TOPDIR}/bal_stub.h grpc-c/examples/

#
#     - cd ${HOME}/OpenNetworkLinux/grpc-c
#     - autoreconf --install
#     - mkdir build && cd build
#     - ../configure
#     - make
#     - sudo make install
cd ${ONL_TOPDIR}/grpc-c
autoreconf --install
mkdir build && cd build
../configure
make --jobs=${MAKE_JOBS}
sudo make install

#To obtain proto files - Have a repo sync of opencord voltha code base:
#     - git clone https://github.com/opencord/voltha.git ${HOME}/voltha
#     - cd ${HOME}/OpenNetworkLinux/grpc-c/
#     - cp ${HOME}/voltha/voltha/adapters/asfvolt16_olt/protos/* examples/
#
cd ${ONL_TOPDIR}/grpc-c
cp ${VOLTHA_TOPDIR}/voltha/adapters/asfvolt16_olt/protos/* examples

#To autogenerate code from proto files:
#     - cd ${HOME}/OpenNetworkLinux/grpc-c/build/examples
#     - make autogen
cd ${ONL_TOPDIR}/grpc-c/build/examples
make autogen

#
#Build voltha_bal_driver:
#     - cd ${HOME}/OpenNetworkLinux/grpc-c/build/examples
#     - Note: Remove "-O2" from Makefile
#     - Note: Set EDGECORE and BRCM_PATH in Makefile
cd ${ONL_TOPDIR}/grpc-c/build/examples
sed -i -e 's/-O2/-O0/g' \
       -e "s:^EDGECORE = /home/asfvolt/shared.*:#&\nEDGECORE = ${ASFSRC_TOPDIR}:" \
       -e "s:^BRCM_PATH = /home/asfvolt/shared.*:#&\nBRCM_PATH = ${BALSRC_TOPDIR}:" \
        Makefile

pushd ${ASFSRC_TOPDIR}
sed -i -e "s:^BRDCM_SRC=/home/asfvolt/shared.*:#&\nBRDCM_SRC = ${BALSRC_TOPDIR}:" \
       -e "s:^GRPC_C_PATH= /home/asfvolt/shared.*:#&\nGRPC_C_PATH = ${ONL_TOPDIR}/grpc-c:" \
        Makefile
popd
#```
#       EDGECORE = ${HOME}/OpenNetworkLinux/asfvolt16-driver/src/
#       BRCM_PATH = ${HOME}/OpenNetworkLinux/bal_src_release/bal_release
#```
#     - make clean_all;make
make clean_all
make
#
#The ultimate executable voltha_bal_driver can be found under ~/grpc-c/build/examples/.libs/

echo "${ASFVOLT_REPO_NAME} build complete"
