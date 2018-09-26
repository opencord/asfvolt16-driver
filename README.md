# Building ONL Environment for ASFvOLT16
  Requirements:
  ```
  	Hardware:
		CPU: Dual-core (4 Threads) up.
		Memory: 6G bytes.
		Hard Disk: 40G of disk free space.
	Software:
		A. docker # to grab the build workspace 
		B. binfmt-support # kernel support for ppc builds 
		C. At least 4G of ram and 4G of swap # compilation is memory intensive
		D. All of the testing is done with Debian, other Linux distributions may
                   work, but we suggest using Debian 8.
		   # apt-get install binfmt-support then follow the instructions at: 
 		     https://docs.docker.com/engine/installation/debian/

  ```
  Download source code:
  ```
	Download ONL source code with Kernel 3.7.10.
	#> git clone https://github.com/Lewis-Kang/OpenNetworkLinux.git -b kernel_3.7.10
  ```

  Build system:
  ```
	#> cd OpenNetworkLinux
	#> sudo docker/tools/onlbuilder -8   # enter the docker workspace
	#> apt-cacher-ng
	#> source setup.env    # pull in necessary environment variables
	#> make amd64
  ```

  ONL Installer:
  ```
	You can find the installer file in 
	#> OpenNetworkLinux/RELEASE/jessie/amd64/ONL-<VERSION>_AMD64_INSTALLED_INSTALLER 
  ```

# ASFvOLT16 vOLTHA BAL DRIVER

This is an executable (voltha_bal_driver), which will receive asfvolt16_olt adapter grpc-c messages and call the respective BAL API.
It will send received responses/asynchronous indications from BAL running in the ASFvOLT16 to the asfvolt16_olt adapter in VOLTHA. 
This driver requires the Broadcom proprietary
BAL/Maple SDK for the PON MAC and the Qumran switching SDK.  These components are available from Broadcom.


## GETTING STARTED

The following **proprietary** components are required to build the voltha_bal_driver.
Contact Dave Baron at Broadcom for details on accessing these files.  dave.baronATbroadcomDOTcom Reference Case: CS3233745

| Component     | Version | Filename                |
| ------------- |-------- | ------------------------|
| BAL/Maple SDK | 2_6_0_1 | SW-BCM68620_2_6_0_1.zip |
| Qumran SDK    | 6.5.7   | sdk-all-6.5.7.tar.gz    |

| Component          | Filename                               |
| -------------      | -----                                  |
| ASFvOLT16 BAL patch| ACCTON_BAL_2.6.0.1-V201804301043.patch |

The following **open-source** components are required to build the voltha_bal_driver.

| Component/Package    | Version        | Name            | From     | Git Repo|
| -------------        |---------       | -----           | ---------| ----|
| VOLTHA asfvolt16_olt adapter | voltha-1.3 | voltha          | Opencord | git clone https://github.com/opencord/voltha.git
| ASFvOLT16 Driver             | voltha-1.3 | asfvolt16-driver| Opencord | git clone https://userid@gerrit.opencord.org/asfvolt16-driver

### Clone asfvolt16-driver repo

```
cd /home/<user>/OpenNetworkLinux
git clone https://github.com/opencord/voltha.git
git clone https://userid@gerrit.opencord.org/asfvolt16-driver
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

These below variables MUST be overridden to suit the local build environment.

```
### Directory where OpenNetworkLinux is present
ONL_TOPDIR=/home/<user>/OpenNetworkLinux
BALSRC_RELEASE=bal_src_release
BALSRC_ZIPNAME=SW-BCM68620_2_6_0_1.zip
SWISDK_TARNAME=sdk-all-6.5.7
PATCH_FILENAME=ACCTON_BAL_2.6.0.1-V201804301043.patch
BROADCOM_DOWNLOAD_DIR=/home/<user>/OpenNetworkLinux
EDGECORE_DOWNLOAD_DIR=/home/<user>/OpenNetworkLinux
```

Prepare the BAL/Maple build workspace from *outside* of the docker environment.  This allows access to network and device resources
that may not be available in the docker.
```
    > cd ${ONL_TOPDIR}
    > ## Copy all the artifacts from Broadcom under the folder brcm_code and then execute the next script
    > BROADCOM_DOWNLOAD_DIR=/home/<user>/OpenNetworkLinux EDGECORE_DOWNLOAD_DIR=/home/<user>/OpenNetworkLinux ./asfvolt16-driver/scripts/asfvolt-bal-prepare.sh
```

### Build BAL/Maple SDK

All of the remaining steps are done *inside* the ONL build docker.  Start ONL build docker workspace:
```
    > docker/tools/onlbuilder -8   # enter the docker workspace
    > BROADCOM_DOWNLOAD_DIR=/home/<user>/OpenNetworkLinux ./asfvolt16-driver/scripts/asfvolt-bal-buildall.sh
```

### Build `voltha_bal_driver`

This step requires access to the voltha source tree so it can find the protobuf definitions used by the
asfvolt16_olt adapter.  Be sure to use matching version of the voltha tree because asfvolt16_olt adapter
and the voltha_bal_driver need to share the same grpc protobuf definitions.

```
ONL_TOPDIR=/home/<user>/OpenNetworkLinux
MAKE_JOBS=4
BALSRC_RELEASE=bal_src_release
VOLTHA_TOPDIR=/home/<user>/voltha
BALSRC_TOPDIR=<BROADCOM_DOWNLOAD_DIR>/bal_src_release/bal_release
ASFSRC_TOPDIR=<EDGECORE_DOWNLOAD_DIR>/asfvolt16-driver/src
PATCHF_TOPDIR=<EDGECORE_DOWNLOAD_DIR>/asfvolt16-driver/patches
```
From the ONL build docker workspace:

```
    > BROADCOM_DOWNLOAD_DIR=/home/<user>/OpenNetworkLinux  EDGECORE_DOWNLOAD_DIR=/home/<user>/OpenNetworkLinux  VOLTHA_TOPDIR=/home/<user>/voltha ./asfvolt16-driver/scripts/asfvolt-driver-build.sh
```

### Package `voltha_bal_driver` into BAL release tarball

From the ONL build docker workspace:

```
    > BROADCOM_DOWNLOAD_DIR=/home/<user>/OpenNetworkLinux  EDGECORE_DOWNLOAD_DIR=/home/<user>/OpenNetworkLinux ./asfvolt16-driver/scripts/asfvolt-driver-package.sh
```

The tarball containing the packaged voltha_bal_driver can be found in ${ASFDRVR_PKGDIR}:

```
    > ls ${EDGECORE_DOWNLOAD_DIR}/asfdrvr-package-dir/
asfvolt16-voltha-bal-<timestamp>.tgz  broadcom  opt
```

### Install `voltha_bal_driver` BAL release tarball

Transfer BAL release tarball to ASFvOLT16:

```
    > scp ${EDGECORE_DOWNLOAD_DIR}/asfdrvr-package-dir/asfvolt16-voltha-bal-<timestamp>.tgz  root@<ASFvOLT16_IP>:.
```

Install release tarball on ASFvOLT16:

```
    > ssh root@<ASFvOLT16_IP>
    root@<ASFvOLT16_IP>'s password: onl

    root@localhost:~# ls
    asfvolt16-voltha-bal-<timestamp>.tgz
    root@localhost:~# cd /
    root@localhost:/# mv /broadcom /broadcom.prev
    root@localhost:/# mv /opt/bcm68620/ /opt/bcm68620.prev
    root@localhost:/# tar zxf ~/asfvolt16-voltha-bal-<timestamp>.tgz
    root@localhost:/# shutdown -r now
```

To set OLT NNI Port speed:

```
    The OLT NNI speed can be changed as below in /broadcom/config.bcm in OLT as below:
    If the config is changed, the OLT needs a reboot for the changes to take effect
    
    Default config in /broadcom/config.bcm:
    #100G
    ucode_port_128.BCM88470=CGE3:core_0.128
    ucode_port_129.BCM88470=CGE2:core_0.129
    ucode_port_130.BCM88470=CGE1:core_0.130
    ucode_port_131.BCM88470=CGE0:core_0.131

    #40G(Comment the config for 100G if set)
    ucode_port_128.BCM88470=XLGE3:core_0.128
    ucode_port_129.BCM88470=XLGE2:core_0.129
    ucode_port_130.BCM88470=XLGE1:core_0.130
    ucode_port_131.BCM88470=XLGE0:core_0.131

    #4*10G(Comment the config for 100G/40G if set)
    ucode_port_128.BCM88470=10GBase-R12:core_0.128
    ucode_port_129.BCM88470=10GBase-R13:core_0.129
    ucode_port_130.BCM88470=10GBase-R14:core_0.130
    ucode_port_131.BCM88470=10GBase-R15:core_0.131
    ucode_port_132.BCM88470=10GBase-R8:core_0.132
    ucode_port_133.BCM88470=10GBase-R9:core_0.133
    ucode_port_134.BCM88470=10GBase-R10:core_0.134
    ucode_port_135.BCM88470=10GBase-R11:core_0.135
    ucode_port_136.BCM88470=10GBase-R4:core_0.136
    ucode_port_137.BCM88470=10GBase-R5:core_0.137
    ucode_port_138.BCM88470=10GBase-R6:core_0.138
    ucode_port_139.BCM88470=10GBase-R7:core_0.139
    ucode_port_140.BCM88470=10GBase-R0:core_0.140
    ucode_port_141.BCM88470=10GBase-R1:core_0.141
    ucode_port_142.BCM88470=10GBase-R2:core_0.142
    ucode_port_143.BCM88470=10GBase-R3:core_0.143
```

USAGE(if executing in foreground):
```
    ./voltha_bal_driver "serverIP:port1" -C "serverIP:port2"
    ./bal_core_dist -C "serverIP:port2"
```

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
