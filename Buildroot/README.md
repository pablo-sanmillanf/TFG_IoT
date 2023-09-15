# Buildroot configurations

## Basics
  
This directory contains the configuration files of the distribution created with Buildroot. This set of files is composed of a main file located in the "**Main_Configurations**" directory, which details the main configurations of the distribution, a secondary file located in the "**LinuxKernel_Configurations**", that contains some kernel configurations used in the build of the distribution, the directory "**RootFile_System_Files**" with a bunch of files that have to be placed in the rootfile system and the directory "**Boot_Files**" with the files that have to be placed in the boot partition.  

## Build

Before the distribution can be built, all [Buildroot dependencies](https://buildroot.org/downloads/manual/manual.html#requirement) must be installed. After that, you should download the tool from the [official website](https://buildroot.org/download.html) and descompress it. Within the tool files, the "**.config**" file must be replaced by the one located in the "Main_Configurations" directory. After this, the following command should be executed to perform the process of building the embedded system:

```
make
```

This will generate an "**output**" folder inside the Buildroot files. Within this directory, access the path "**build/linux-custom**" and replace the "**.config**" file with the one provided in the "**LinuxKernel_Configurations**" folder of this repository, so that all the necessary configurations will be applied to the Kernel.  
Then, inside the "**target**" folder of the "**output**" directory you will find the RootFile System used in the embedded system. Here, the files of the "**RootFile_System_Files**" folder should be placed as shown below:

```
interfaces           ->  target/etc/network/
S70weatherApp        ->  target/etc/init.d/
sshd_config          ->  target/etc/ssh/
syslog.conf          ->  target/etc/
TFG                  ->  target/root/
wpa_supplicant.conf  ->  target/etc/
```

Afterwards, inside the path "**output/images/rpi-firmware/**" you must add all the files that you want to include in the boot partition. In this case, the files for this embedded system are in the "**Boot_Files**" directory of this repository, and should be added to the "**rpi-firmware**" folder as shown below:
```
config.txt  ->  images/rpi-firmware/
```
Finally, the first command of this section must be executed again to save the configurations in the resulting image. This image will be located in the path "**output/images/sdcard.img**".
