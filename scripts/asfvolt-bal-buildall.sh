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
# Run from OpenNetworkLinux top directory INSIDE of onlbuilder docker workspace
#
#cd /aux/OpenNetworkLinux
set -e

# override shell variables to match custom local build environment
: ${ONL_TOPDIR:=`pwd`}
: ${MAKE_JOBS:=1}
: ${BALSRC_RELEASE:=bal_src_release}
: ${SWISDK_TARNAME:=sdk-all-6.5.7}

echo ONL_TOPDIR=${ONL_TOPDIR}
echo MAKE_JOBS=${MAKE_JOBS}
echo BALSRC_RELEASE=${BALSRC_RELEASE}

cd ${BALSRC_RELEASE}/bal_release
make BOARD=asfvolt16 maple_sdk_dir
make BOARD=asfvolt16 maple_sdk
make BOARD=asfvolt16 switch_sdk_dir
make BOARD=asfvolt16 switch_sdk

pushd 3rdparty/bcm-sdk/build-asfvolt16/${SWISDK_TARNAME}/systems/linux/user/x86-generic_64-2_6
KERNDIR=$ONL_TOPDIR/packages/base/amd64/kernels/kernel-3.7-x86-64-all/builds/linux-3.7.10 BOARD=asfvolt16 ARCH=x86_64 SDKBUILD=build_bcm_user make
popd

make BOARD=asfvolt16 bal
make BOARD=asfvolt16 release_board

ls -l 3rdparty/maple/cur/asfvolt16/release

date
echo "ASFvOLT16 build finished!"
