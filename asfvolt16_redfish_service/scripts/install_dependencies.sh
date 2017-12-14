#!/bin/bash
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
#Install required packages

echo ""
echo "Installing dependency Packages"
echo ""

sudo apt-get update

sudo apt-get install clang --yes
sudo apt-get install -f
sudo apt-get install libgcrypt20-dev libncurses5-dev libnl-3-dev --yes
sudo apt-get install -f
sudo apt-get install libudev-dev libglibmm-2.4-dev libglib3.0-cil-dev --yes
sudo apt-get install -f
sudo apt-get install libxml++2.6-dev libgnutls28-dev libnl-route-3-dev --yes
sudo apt-get install -f
sudo apt-get install flex bison doxygen cpp ccache build-essential linux-libc-dev --yes
sudo apt-get install -f
sudo apt-get install libmpc-dev libstdc++6 libcurl4-openssl-dev libmicrohttpd-dev libjsoncpp-dev  --yes
sudo apt-get install -f
sudo apt-get install lcov libossp-uuid-dev libxml++2.6-dev libnl-3-dev libnl-route-3-200  --yes
sudo apt-get install -f
sudo apt-get install libudev-dev libgcrypt20-dev libsysfs-dev libpopt-dev --yes
sudo apt-get install -f
sudo apt-get install libusb-dev patch libdevmapper-dev liblvm2-dev unzip libnl-genl-3-dev libblkid-dev debsigs debsig-verify gnupg --yes
sudo apt-get install -f
sudo apt-get install ipmitool --yes
sudo apt-get install -f

