# ASFvOLT16 vOLTHA BAL DRIVER

This is an executable (voltha_bal_driver), which will receive asfvolt16_olt adapter grpc-c messages and call the respective BAL API.
It will send received responses/asynchronous indications from BAL running in the ASFvOLT16 to the asfvolt16_olt adapter in VOLTHA. 
This driver requires the Broadcom proprietary
BAL/Maple SDK for the PON MAC and the Qumran switching SDK.  These components are available from Broadcom.


## GETTING STARTED

The following **proprietary** components are required to build the voltha_bal_driver.
Contact Dave Baron at Broadcom for details on accessing these files.  dave.baronATbroadcomDOTcom Reference Case: CS3233745

| Component     | Version | Filename                        |
| ------------- |-------- | -----                           |
| BAL/Maple SDK | 2.4.7.12 | SW-BCM68620_2_4_7_12.zip         |
| Qumran SDK    | 6.5.7   | sdk-all-6.5.7.tar.gz            |

| Component          | Filename                           |SHA1|
| -------------      | -----                              |---|
| ASFvOLT16 BAL patch| ACCTON_BAL_2.4.7.12-V201712061142.patch | e2d2e7924cbc73e60d29262f4b905cbe982758ae |

The following **open-source** components are required to build the voltha_bal_driver.

| Component/Package    | Version        | Name            | From     | Git Repo|
| -------------        |---------       | -----           | ---------| ----|
| VOLTHA asfvolt16_olt adapter | voltha-1.1 | voltha          | Opencord | git clone https://gerrit.opencord.org/voltha
| ASFvOLT16 Driver             | voltha-1.1 | asfvolt16-driver| Opencord | git clone https://gerrit.opencord.org/asfvolt16-driver

## Open Network Linux
This guide assumes that Open Network Linux (ONL) build environment for ASFvOLT16 has been installed
and the build was successful. Refer to ONL build instructions
[here](https://wiki.opencord.org/download/attachments/2556712/ONL_Build_Environment_Installation_Guide.pdf?api=v2).

### Clone asfvolt16-driver repo

```
cd OpenNetworkLinux
git clone https://gerrit.opencord.org/asfvolt16-driver
```

Build helper scripts are available in `asfvolt16-driver/scripts` to automate the steps for building BAL/Maple SDK and the `voltha_bal_driver`.

| Helper Script      | Purpose |
| -------------      |---------|
| asfvolt-bal-prepare.sh    |Prepare BAL/Maple SDK build workspace (outside of docker)|
| asfvolt-bal-buildall.sh   |Build BAL/Maple SDK|
| asfvolt-driver-build.sh   |Build `voltha_bal_driver`|
| asfvolt-driver-package.sh |Repackage BAL/Maple release with `voltha_bal_driver`|

The helper scripts contain default shell variable values for component directory locations and file names for the build.

### Prepare workspace for BAL

Any of the following variables can be overridden to suit the local build environment.

```
ONL_TOPDIR=/aux/OpenNetworkLinux
BALSRC_RELEASE=bal_src_release
BALSRC_ZIPNAME=SW-BCM68620_2_4_7_12.zip
SWISDK_TARNAME=sdk-all-6.5.7
PATCH_FILENAME=ACCTON_BAL_2.4.7.12-V201712061142.patch
BROADCOM_DOWNLOAD_DIR=/home/user/broadcom/download
EDGECORE_DOWNLOAD_DIR=/home/user/edgecore/download
```

Prepare the BAL/Maple build workspace from *outside* of the docker environment.  This allows access to network and device resources
that may not be available in the docker.
```
    > cd ${ONL_TOPDIR}
    > BROADCOM_DOWNLOAD_DIR=/aux/brcm_download EDGECORE_DOWNLOAD_DIR=/aux/brcm_download ./asfvolt16-driver/scripts/asfvolt-bal-prepare.sh
```

### Build BAL/Maple SDK

All of the remaining steps are done *inside* the ONL build docker.  Start ONL build docker workspace:
```
    > docker/tools/onlbuilder -8   # enter the docker workspace
    > ./asfvolt16-driver/scripts/asfvolt-bal-buildall.sh
```

### Build `voltha_bal_driver`

This step requires access to the voltha source tree so it can find the protobuf definitions used by the
asfvolt16_olt adapter.  Be sure to use matching version of the voltha tree because asfvolt16_olt adapter
and the voltha_bal_driver need to share the same grpc protobuf definitions.

```
ONL_TOPDIR=/aux/OpenNetworkLinux
MAKE_JOBS=4
BALSRC_RELEASE=bal_src_release
VOLTHA_TOPDIR=/home/<user>/voltha/incubator/voltha
BALSRC_TOPDIR=/aux/OpenNetworkLinux/bal_src_release/bal_release
ASFSRC_TOPDIR=/aux/OpenNetworkLinux/asfvolt16-driver/src
DEVSIM_TOPDIR=/aux/OpenNetworkLinux/asfvolt16-driver/device_simulator
PATCHF_TOPDIR=/aux/OpenNetworkLinux/asfvolt16-driver/patches
```
From the ONL build docker workspace:

```
    > VOLTHA_TOPDIR=/home/<user>/voltha/incubator/voltha ./asfvolt16-driver/scripts/asfvolt-driver-build.sh
```

### Package `voltha_bal_driver` into BAL release tarball

From the ONL build docker workspace:

```
    > ./asfvolt16-driver/scripts/asfvolt-driver-package.sh
```

The tarball containing the packaged voltha_bal_driver can be found in ${ASFDRVR_PKGDIR}:

```
    > ls ${ONL_TOPDIR}/asfdrvr-package-dir/
asfvolt16-voltha-bal-<timestamp>.tgz  broadcom  opt
```

### Install `voltha_bal_driver` BAL release tarball

Transfer BAL release tarball to ASFvOLT16:

```
    > cp asfdrvr-package-dir/asfvolt16-voltha-bal-<timestamp>.tgz  root@<ASFvOLT16_IP>:.
```

Install release tarball on ASFvOLT16:

```
    > ssh root@<ASFvOLT16_IP>
    Enter passphrase for key '/home/kimk/.ssh/id_rsa':
    root@192.168.140.72's password: onl
    Last login: Tue Oct 24 11:00:48 2017 from 192.168.140.32

    root@localhost:~# ls
    asfvolt16-voltha-bal-201710231906.tgz
    root@localhost:~# cd /
    root@localhost:/# mv /broadcom /broadcom.prev
    root@localhost:/# mv /opt/bcm68620/ /opt/bcm68620.prev
    root@localhost:/# tar zxf ~/asfvolt16-voltha-bal-<timestamp>.tgz
    root@localhost:/# tail -5 /etc/rc.local # ensure rc.local has the following lines
    # By default this script does nothing.

    /opt/bcm68620/svk_init.sh
    /broadcom/voltha_init.sh
    exit 0

    root@localhost:/# shutdown -r now
```

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
  

