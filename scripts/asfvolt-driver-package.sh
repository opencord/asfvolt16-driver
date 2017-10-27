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

# override shell variables to match custom local build environment
: ${ONL_TOPDIR:=`pwd`}
: ${BALSRC_RELEASE:=bal_src_release}
: ${BALREL_TOPDIR:=${ONL_TOPDIR}/${BALSRC_RELEASE}}
: ${BALSRC_TOPDIR:=${ONL_TOPDIR}/${BALSRC_RELEASE}/bal_release}
: ${ASFSCR_TOPDIR:=${ONL_TOPDIR}/${ASFVOLT_REPO_NAME}/scripts}
: ${ASFDRVR_PKGDIR:=${ONL_TOPDIR}/asfdrvr-package-dir}
: ${ASFDVR_TARBALL_NAME:=asfvolt16-voltha-bal}
: ${BUILD_FILENAME_TAG:=`date +%Y%m%d%H%M`}
: ${ASFDVR_TARBALL_FNAME:=${ASFDVR_TARBALL_NAME}-${BUILD_FILENAME_TAG}.tgz}

echo ONL_TOPDIR=${ONL_TOPDIR}
echo BALSRC_RELEASE=${BALSRC_RELEASE}
echo BALREL_TOPDIR=${BALREL_TOPDIR}
echo BALSRC_TOPDIR=${BALSRC_TOPDIR}
echo ASFSCR_TOPDIR=${ASFSCR_TOPDIR}
echo ASFDRVR_PKGDIR=${ASFDRVR_PKGDIR}
echo ASFDVR_TARBALL_NAME=${ASFDVR_TARBALL_NAME}
echo BUILD_FILENAME_TAG=${BUILD_FILENAME_TAG}
echo ASFDVR_TARBALL_FNAME=${ASFDVR_TARBALL_FNAME}

# Note: removes existing directories: asfvolt16-driver, grpc, protobuf
rm -rf ${ASFDRVR_PKGDIR}


mkdir -p ${ASFDRVR_PKGDIR}
cd ${ASFDRVR_PKGDIR}

#extract ASFvOLT16 BAL/Maple built release tarball
tar zxf ${BALREL_TOPDIR}/bcm68620_release/asfvolt16/release/release_asfvolt16_V*.tar.gz

# voltha_bal_driver and grpc/protobufs live in /broadcom dir on ASFvOLT16 target
cd broadcom

# add grpc-c libs
cp -pR ${ONL_TOPDIR}/grpc-c/build/lib/.libs/libgrpc-c.so.0.0.0 .
ln -s /broadcom/libgrpc-c.so.0.0.0 libgrpc-c.so.0
ln -s /broadcom/libgrpc-c.so.0.0.0 libgrpc-c.so

# add grpc libs
cp -pR ${ONL_TOPDIR}/grpc/libs/opt/libgrp*.so* .
cp -pR ${ONL_TOPDIR}/grpc/libs/opt/libgpr*.so* .

# add protobuf-c libs
cp -pR ${ONL_TOPDIR}/grpc-c/third_party/protobuf-c/protobuf-c/.libs/libprotobuf-c.so* .

# add protobuf libs
cp -pR ${ONL_TOPDIR}/grpc/thirdparty/protobuf/src/.libs/libprotobuf.so* .
cp -pR ${ONL_TOPDIR}/grpc/thirdparty/protobuf/src/.libs/libprotobuf-lite.so* .
cp -pR ${ONL_TOPDIR}/grpc/thirdparty/protobuf/src/.libs/libprotoc.so* .
rm *.0T

# add voltha_bal_driver
cp -pR ${ONL_TOPDIR}/grpc-c/build/examples/.libs/voltha_bal_driver .

# add voltha driver startup scripts
cp -p ${ASFSCR_TOPDIR}/voltha_init_sh voltha_init.sh
chmod 755 voltha_init.sh

#extract ASFvOLT16 BAL/Maple built release tarball
cd ..
tar cvzf ${ASFDVR_TARBALL_FNAME} broadcom opt

ls -l `pwd`/${ASFDVR_TARBALL_FNAME}
echo "${ASFVOLT_REPO_NAME} package build complete"
