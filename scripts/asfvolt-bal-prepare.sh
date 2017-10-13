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

# override shell variables to match custom local build environment
: ${ONL_TOPDIR:=`pwd`}
: ${BALSRC_RELEASE:=bal_src_release}
: ${BALSRC_ZIPNAME:=SW-BCM68620_2_4_3_6.zip}
: ${SWISDK_TARNAME:=sdk-all-6.5.7}

: ${BROADCOM_DOWNLOAD_DIR:=${HOME}/broadcom/download}  # directory containing SW-BCM68620_2_4_x.zip
: ${EDGECORE_DOWNLOAD_DIR:=${HOME}/edgecore/download}  # directory containing ASFvOLT16 patch file for BAL

: ${PATCH_FILENAME:=ACCTON_BAL_2.4.3.6-V201710131639.patch}   # use latest known ASFvOLT16 patch file as default

echo ONL_TOPDIR=${ONL_TOPDIR}
echo BALSRC_RELEASE=${BALSRC_RELEASE}
echo BALSRC_ZIPNAME=${BALSRC_ZIPNAME}
echo SWISDK_TARNAME=${SWISDK_TARNAME}
echo PATCH_FILENAME=${PATCH_FILENAME}
echo BROADCOM_DOWNLOAD_DIR=${BROADCOM_DOWNLOAD_DIR}
echo EDGECORE_DOWNLOAD_DIR=${EDGECORE_DOWNLOAD_DIR}

mkdir -p $ONL_TOPDIR/${BALSRC_RELEASE}
cd $ONL_TOPDIR/${BALSRC_RELEASE}

unzip ${BROADCOM_DOWNLOAD_DIR}/${BALSRC_ZIPNAME}
cp $BROADCOM_DOWNLOAD_DIR/${SWISDK_TARNAME}.tar.gz bal_release/3rdparty/bcm-sdk/

cp $EDGECORE_DOWNLOAD_DIR/${PATCH_FILENAME}  .
chmod -R 744 .
patch -p1 < ${PATCH_FILENAME}

mkdir -p bcm68620_release/asfvolt16/kernels
cd bcm68620_release/asfvolt16/kernels
ln -s ../../../../packages/base/amd64/kernels/kernel-3.7-x86-64-all/builds/linux-3.7.10 linux-3.7.10
ln -s ../../../../packages/base/any/kernels/archives/linux-3.7.10.tar.xz linux-3.7.10.tar.xz
ln -s ../../../../packages/base/any/kernels/3.7/configs/x86_64-all/x86_64-all.config x86_64-all.config

echo "BAL build area prepared"
