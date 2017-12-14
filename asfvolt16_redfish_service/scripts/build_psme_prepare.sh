#!/bin/bash

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

# override shell variables to match custom local build environment

: ${ONL_DIR=`pwd`}
: ${ASFVOLT16_RSD_DIR:=${ONL_DIR}/asfvolt16-driver/asfvolt16_redfish_service}
: ${RSD_TOPDIR:=${ASFVOLT16_RSD_DIR}/rsd_org}
: ${PSM_TOPDIR:=${RSD_TOPDIR}/PSME}
: ${RMM_TOPDIR:=${RSD_TOPDIR}/RMM}
: ${PSM_DEPDIR:=${PSM_TOPDIR}/third_party}
: ${RMM_DEPDIR:=${RMM_TOPDIR}/src/deps}
: ${PKG_TOPDIR:=${RSD_TOPDIR}/asfvolt16_rf_packages}
: ${RMM_UUID:=${RMM_DEPDIR}/uuid}
: ${RMM_CURL:=${RMM_DEPDIR}/libcurl}
: ${RMM_OSSL:=${RMM_DEPDIR}/openssl}


cd ${ASFVOLT16_RSD_DIR}
#Remove exisiting  IntelRSD Source Code and download from google drive
rm -rf rsd_org rsd_org.tar.gz
wget -O rsd_org.tar.gz 'https://drive.google.com/uc?export=download&id=1vIbd_yGVyuLaWK0nUwm6H3xy1XLDDkR8'
tar -xf rsd_org.tar.gz
cd rsd_org

#Download accton patch from google drive and apply to IntelRSD
echo "Downloading and Applying accton.patch"
wget -O ${ASFVOLT16_RSD_DIR}/accton.patch 'https://drive.google.com/uc?export=download&id=191UUhYbqnDTuwrSl2S9P8D7fNoMQw1YE'
patch -p1 < ${ASFVOLT16_RSD_DIR}/accton.patch

#Apply Calsoft patch
echo "Applying calsoft.patch"
patch -p1 < ${ASFVOLT16_RSD_DIR}/calsoft.patch

#Install cmake version to  3.5.2 required for IntelRSD build
CMAKE_VER=`cmake -version | grep "cmake version"  | awk '{print $3}'`
if [ "${CMAKE_VER}" != "3.5.2" ]; then
    sudo apt-get purge cmake
    wget https://cmake.org/files/v3.5/cmake-3.5.2.tar.gz
    tar xzvf cmake-3.5.2.tar.gz
    cd cmake-3.5.2/
    ./bootstrap
    make -j4
    sudo make install
    if [ -e /usr/bin/cmake ]; then
         sudo rm /usr/bin/cmake
    fi
    sudo ln -s /usr/local/bin/cmake /usr/bin/cmake
fi

echo "Cmake Version ${CMAKE_VER} installed"

#Download 3rd party code required for PSME
cd ${PSM_DEPDIR}
wget https://github.com/open-source-parsers/jsoncpp/archive/1.7.2.zip
mv 1.7.2.zip jsoncpp-1.7.2.zip

wget https://github.com/cinemast/libjson-rpc-cpp/archive/v0.6.0.zip
mv v0.6.0.zip jsonrpccpp-0.6.0.zip

wget http://downloads.sourceforge.net/project/ipmitool/ipmitool/1.8.17/ipmitool-1.8.17.tar.bz2

wget  https://github.com/01org/safestringlib/archive/master.zip
mv master.zip safestring-0.0.1.zip

wget  https://github.com/google/googletest/archive/release-1.7.0.zip
mv release-1.7.0.zip gtest-1.7.0.zip

wget https://github.com/google/googlemock/archive/release-1.7.0.zip
mv release-1.7.0.zip gmock-1.7.0.zip

cd -

cd ${RMM_UUID}
wget http://www.mirrorservice.org/sites/ftp.ossp.org/pkg/lib/uuid/uuid-1.6.2.tar.gz

cd ${RMM_CURL}
wget https://curl.haxx.se/download/curl-7.40.0.tar.bz2

cd ${RMM_OSSL}
wget https://www.openssl.org/source/old/1.0.1/openssl-1.0.1u.tar.gz

cd ${RSD_TOPDIR}

#Build PSME
echo ""
echo "Building PSME"
echo ""
chmod 777 ${PSM_TOPDIR}/agent/network/acc_sw/psme.sh
chmod 777 ${RMM_TOPDIR}/utils/deb_maker/install/multi-deb/rmm-all/etc/init.d/psme
cd ${PSM_TOPDIR}
if [ ! -d "build" ]; then
    mkdir build
fi
cd build; cmake ../
grep -rl Werror . | grep flags.make | xargs sed -i 's/-Werror//g'
make psme-rest-server psme-chassis -j8


#Create Debian Packages
echo ""
echo "Creating Debian Package"
echo ""
cd ${RMM_TOPDIR}/utils
./rmm_release.sh <<!
A
!

#Copying generated packages to $BIN_DIRECTORY
echo "PSME debian packages are created in  ${RMM_TOPDIR}/build/install/multi-deb/bin/rmm-bin-2.1.3.38.0/"
echo "Copying Debian packages to ${PKG_TOPDIR}"
if [ ! -d  ${PKG_TOPDIR} ]; then
    mkdir ${PKG_TOPDIR}
fi

`cp -a ${RMM_TOPDIR}/build/install/multi-deb/bin/rmm-bin-2.1.3.38.0/* ${PKG_TOPDIR}`
`cp ${ASFVOLT16_RSD_DIR}/remove.sh ${PKG_TOPDIR}`
echo ""
echo "ASFVOLT16 Redfish service build finished !!!"
echo "Packages are available in ${PKG_TOPDIR}"

cd ${ONL_DIR}

