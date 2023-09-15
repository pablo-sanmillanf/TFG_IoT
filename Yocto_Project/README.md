# Yocto Project configurations

## Basics
  
This directory contains the configuration files of the distribution created with Yocto Project. It is created over the 
default "**core-image-base**" recipe of the Poky distribution.  
  
On the one hand, the "**meta-custom-weather-layer**" directory contains the custom layer used to adapt this default recipe to 
the specific needs of the project. On the other hand, there is the "**conf**" directory with the general settings of the 
distribution.  
In order to build this distribution, it is necessary to have the [Poky distribution](https://git.yoctoproject.org/poky), 
the [Raspberry Pi BSP layer](https://git.yoctoproject.org/meta-raspberrypi) and a number of extra 
[OpenEmbedded layers](https://git.openembedded.org/meta-openembedded). More specifically, the "**meta-oe**", "**meta-python**" 
and "**meta-networking**" layers are needed. All these components should be included in the _Kirkstone_ version of the project.

## Build

Before the distribution can be built, all 
[Yocto Project dependencies](https://docs.yoctoproject.org/4.0.12/brief-yoctoprojectqs/index.html#build-host-packages) must be 
installed. After that, the necessary components for the distribution must be downloaded with the following commands:  

```
git clone -b kirkstone git://git.yoctoproject.org/poky
git clone -b kirkstone git://git.openembedded.org/meta-openembedded
git clone -b kirkstone git://git.yoctoproject.org/meta-raspberrypi
```
  
In the directory where this download has been performed is where the "**meta-custom-weather-layer**" layer should be placed. 
Then, with the following command the "**build**" folder will be created in which the distribution will be built:  

```
source poky/oe-init-build-env
```
  
In that folder the "**conf**" directory should be replaced by the one hosted in this repository. Finally, the following command 
will start the process of building the distribution image:

```
bitbake core-image-base
```

After this process, the final image can be found as a compressed file in the path 
"**tmp/deploy/images/raspberrypi3/core-image-base-raspberrypi3.wic.bz2**" inside the "**buid**" folder. With this file it will 
only be necessary to decompress it with a tool such as "**bzip2**" and save it on the microSD that will be inserted in the 
Raspberry Pi.
