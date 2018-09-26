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
ASFVOLT_REPO_NAME=asfvolt16-driver
: ${MAKE_JOBS:=4}

# override shell variables to match custom local build environment
: ${ONL_TOPDIR:=`pwd`}
: ${BALSRC_RELEASE=${BROADCOM_DOWNLOAD_DIR}/bal_src_release}
: ${VOLTHA_TOPDIR:=${HOME}/voltha}
: ${BALSRC_TOPDIR:=${BALSRC_RELEASE}/bal_release}
: ${ASFSRC_TOPDIR:=${EDGECORE_DOWNLOAD_DIR}/${ASFVOLT_REPO_NAME}/src}
: ${PATCHF_TOPDIR:=${EDGECORE_DOWNLOAD_DIR}/${ASFVOLT_REPO_NAME}/patches}
: ${GRPC_C_PATH:=${EDGECORE_DOWNLOAD_DIR}/grpc-c}

echo ONL_TOPDIR=${ONL_TOPDIR}
echo MAKE_JOBS=${MAKE_JOBS}
echo BALSRC_RELEASE=${BALSRC_RELEASE}
echo VOLTHA_TOPDIR=${VOLTHA_TOPDIR}
echo BALSRC_TOPDIR=${BALSRC_TOPDIR}
echo ASFSRC_TOPDIR=${ASFSRC_TOPDIR}
echo PATCHF_TOPDIR=${PATCHF_TOPDIR}

#To obtain proto files - Have a repo sync of opencord voltha code base:
#     - git clone https://github.com/opencord/voltha.git ${HOME}/voltha
#     - cd ${HOME}/OpenNetworkLinux/grpc-c/
#     - cp ${HOME}/voltha/voltha/adapters/asfvolt16_olt/protos/* examples/
#
cd ${EDGECORE_DOWNLOAD_DIR}/grpc-c
cp ${VOLTHA_TOPDIR}/voltha/adapters/asfvolt16_olt/protos/* examples/

#To autogenerate code from proto files:
#     - cd ${HOME}/OpenNetworkLinux/grpc-c/build/examples
#     - make autogen
cd ${EDGECORE_DOWNLOAD_DIR}/grpc-c/build/examples
cp ${ASFSRC_TOPDIR}/voltha_bal_driver.c ${EDGECORE_DOWNLOAD_DIR}/grpc-c/examples/
make autogen

#
#Build voltha_bal_driver:
#     - cd ${HOME}/OpenNetworkLinux/grpc-c/build/examples
#     - Note: Remove "-O2" from Makefile
#     - Note: Set EDGECORE and BRCM_PATH in Makefile
cd ${EDGECORE_DOWNLOAD_DIR}/grpc-c/build/examples
sed -i -e 's/-O2/-O0/g' \
       -e "s:^EDGECORE = /home/asfvolt/shared.*:#&\nEDGECORE = ${ASFSRC_TOPDIR}:" \
       -e "s:^BRCM_PATH = /home/asfvolt/shared.*:#&\nBRCM_PATH = ${BALSRC_TOPDIR}:" \
        Makefile

pushd ${ASFSRC_TOPDIR}
sed -i -e "s:^BRDCM_SRC=/home/asfvolt/shared.*:#&\nBRDCM_SRC = ${BALSRC_TOPDIR}:" \
       -e "s:^GRPC_C_PATH=/home/asfvolt/shared.*:#&\nGRPC_C_PATH = ${GRPC_C_PATH}:" \
        Makefile
popd

#```
#       EDGECORE = ${EDGECORE_DOWNLOAD_DIR}/asfvolt16-driver/src/
#       BRCM_PATH = ${BROADCOM_DOWNLOAD_DIR}/bal_src_release/bal_release
#```
#     - make clean_all;make
make clean_all
make
#
#The ultimate executable voltha_bal_driver can be found under ~/grpc-c/build/examples/.libs/

echo "${ASFVOLT_REPO_NAME} build complete"
