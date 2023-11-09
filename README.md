# LR11XX RF API implementation example

## Description

This **LR11XX RF API** is a low level implementation example of the [Sigfox EP library](https://github.com/sigfox-tech-radio/sigfox-ep-lib), showing the `manuf/rf_api.c` file implementation for the [LR11xx](https://www.semtech.com/products/wireless-rf/lora-edge/lr1110) transceiver from Semtech.

> [!WARNING]  
> The resulting radio performances of your device strongly depends on your **hardware design** (schematic, PCB routing, crystal oscillator placement, good RF practices, etc...). **Sigfox certification remains mandatory** whatever the software embedded in your device (including the Sigfox End-Point library and its implementation examples).

The table below shows the versions compatibility between this radio example and the Sigfox End-Point library.

| **LR11XX_RF_API** | **EP_LIB** |
|:---:|:---:|
| [v1.2](https://github.com/sigfox-tech-radio/sigfox-ep-rf-api-semtech-lr11xx/releases/tag/v1.2) | >= [v3.4](https://github.com/sigfox-tech-radio/sigfox-ep-lib/releases/tag/v3.4) |
| [v1.1](https://github.com/sigfox-tech-radio/sigfox-ep-rf-api-semtech-lr11xx/releases/tag/v1.1) | [v3.2](https://github.com/sigfox-tech-radio/sigfox-ep-lib/releases/tag/v3.2) to [v3.3](https://github.com/sigfox-tech-radio/sigfox-ep-lib/releases/tag/v3.3)|
| [v1.0](https://github.com/sigfox-tech-radio/sigfox-ep-rf-api-semtech-lr11xx/releases/tag/v1.0) | [v3.2](https://github.com/sigfox-tech-radio/sigfox-ep-lib/releases/tag/v3.2) to [v3.3](https://github.com/sigfox-tech-radio/sigfox-ep-lib/releases/tag/v3.3)|

## Architecture

<p align="center">
<img src="https://github.com/sigfox-tech-radio/sigfox-ep-rf-api-semtech-lr11xx/wiki/images/sigfox_ep_rf_api_semtech_lr11xx_architecture.drawio.png" width="600"/>
</p>

## Compilation flags for optimization

This radio example inherits all the [Sigfox End-Point library flags](https://github.com/sigfox-tech-radio/sigfox-ep-lib/wiki/compilation-flags-for-optimization) and can be optimized accordingly.

The `LOW_LEVEL_OPEN_CLOSE` flag must be enabled to use this example.

## How to add LR11XX RF API example to your project

### Dependencies

The **LR11XX RF API** is based on the [official LR11xx driver](https://github.com/Lora-net/SWDR001) from Semtech. This driver exposes a functional interface to configure the chip registers, which is used to perform the Sigfox uplink modulation and optional downlink reception. It relies on **low level functions** (called board drivers) which need to be implemented to run on your specific hardware. There are divided in 2 groups:

* **lr11xx_hal** : low level functions of the LR11XX driver from Semtech (registers access through SPI).
* **LR11XX_HW_API** : additional hardware dependent functions required to get a generic RF API implementation of the LR11xx.

The templates are located in the `src/board` folder.

### Submodule

The best way to embed the LR11XX RF API example into your project is to use a [Git submodule](https://git-scm.com/book/en/v2/Git-Tools-Submodules), in a similar way to the library. The radio driver will be seen as a sub-repository with independant history. It will be much easier to **upgrade the radio driver** or to **switch between versions** when necessary, by using the common `git pull` and `git checkout` commands within the `sigfox-ep-rf-api-st-s2lp` folder.

In order to keep the repository clean, you will need to **exclude** the `src/board` folder from build (where the board templates are defined) and implement the functions in another location of your project.

To add the LR11XX RF API submodule, go to your project location and run the following commands:

```bash
mkdir lib
cd lib/
git submodule add https://github.com/sigfox-tech-radio/sigfox-ep-rf-api-semtech-lr11xx.git
```

This will clone the LR11XX RF API repository. At project level, you can commit the submodule creation with the following commands:

```bash
git commit --message "Add Sigfox LR11XX RF API submodule."
git push
```

With the submodule, you can easily:

* Update the radio driver to the **latest version**:

```bash
cd lib/sigfox-ep-rf-api-semtech-lr11xx/
git pull
git checkout master
```

* Use a **specific release**:

```bash
cd lib/sigfox-ep-rf-api-semtech-lr11xx/
git pull
git checkout <tag>
```

### Raw source code

You can [download](https://github.com/sigfox-tech-radio/sigfox-ep-rf-api-semtech-lr11xx/releases) or clone any release of the LR11XX RF API example and copy all files into your project.

```bash
git clone https://github.com/sigfox-tech-radio/sigfox-ep-rf-api-semtech-lr11xx.git
```

### Precompiled source code

You can [download](https://github.com/sigfox-tech-radio/sigfox-ep-rf-api-semtech-lr11xx/releases) or clone any release of the LR11XX RF API example and copy all files into your project. If you do not plan to change your compilation flags in the future, you can perform a **precompilation step** before copying the file in your project. The precompilation will **remove all preprocessor directives** according to your flags selection, in order to produce a more **readable code**. Then you can copy the new files into your project.

```bash
git clone https://github.com/sigfox-tech-radio/sigfox-ep-rf-api-semtech-lr11xx.git
```

To perform the precompilation, you have to install `cmake` and `unifdef` tools, and run the following commands:

```bash
cd sigfox-ep-rf-api-semtech-lr11xx/
mkdir build
cd build/
```

* Precompiling by reading the `sigfox_ep_flags.h` file:

```bash
cmake -DSIGFOX_EP_LIB_DIR=<sigfox-ep-lib path> \
      -DUSE_SIGFOX_EP_FLAGS_H=ON ..
make precompil_lr11xx_rf_api
```

* Precompiling by entering the flags selection on command line:

```bash
cmake -DSIGFOX_EP_LIB_DIR=<sigfox-ep-lib path> \
      -DUSE_SIGFOX_EP_FLAGS_H=OFF \
      -DRC1=ON \
      -DRC2=ON \
      -DRC3C=ON \
      -DRC3D=ON \
      -DRC4=ON \
      -DRC5=ON \
      -DRC6=ON \
      -DRC7=ON \
      -DAPPLICATION_MESSAGES=ON \
      -DCONTROL_KEEP_ALIVE_MESSAGE=ON \
      -DBIDIRECTIONAL=ON \
      -DASYNCHRONOUS=ON \
      -DLOW_LEVEL_OPEN_CLOSE=ON \
      -DREGULATORY=ON \
      -DLATENCY_COMPENSATION=ON \
      -DSINGLE_FRAME=ON \
      -DUL_BIT_RATE_BPS=OFF \
      -DTX_POWER_DBM_EIRP=OFF \
      -DT_IFU_MS=OFF \
      -DT_CONF_MS=OFF \
      -DUL_PAYLOAD_SIZE=OFF \
      -DCRC_HW=OFF \
      -DMESSAGE_COUNTER_ROLLOVER=OFF \
      -DPARAMETERS_CHECK=ON \
      -DCERTIFICATION=ON \
      -DPUBLIC_KEY_CAPABLE=ON \
      -DVERBOSE=ON \
      -DERROR_CODES=ON \
      -DERROR_STACK=12 ..
make precompil_lr11xx_rf_api
```

The new files will be generated in the `build/precompil` folder.

### Static library

You can also [download](https://github.com/sigfox-tech-radio/sigfox-ep-rf-api-semtech-lr11xx/releases) or clone any release of the LR11XX RF API example and build a **static library**.
```bash
git clone https://github.com/sigfox-tech-radio/sigfox-ep-rf-api-semtech-lr11xx.git
```

To build a static library, you have to install `cmake` tool and run the following commands:

```bash
cd sigfox-ep-rf-api-semtech-lr11xx/
mkdir build
cd build/
```

* Building by reading the `sigfox_ep_flags.h` file:

```bash
cmake -DSIGFOX_EP_LIB_DIR=<sigfox-ep-lib path> \
      -DUSE_SIGFOX_EP_FLAGS_H=ON ..
make lr11xx_rf_api
```

* Building by entering the flags selection on command line:

```bash
cmake -DSIGFOX_EP_LIB_DIR=<sigfox-ep-lib path> \
      -DUSE_SIGFOX_EP_FLAGS_H=OFF \
      -DRC1=ON \
      -DRC2=ON \
      -DRC3C=ON \
      -DRC3D=ON \
      -DRC4=ON \
      -DRC5=ON \
      -DRC6=ON \
      -DRC7=ON \
      -DAPPLICATION_MESSAGES=ON \
      -DCONTROL_KEEP_ALIVE_MESSAGE=ON \
      -DBIDIRECTIONAL=ON \
      -DASYNCHRONOUS=ON \
      -DLOW_LEVEL_OPEN_CLOSE=ON \
      -DREGULATORY=ON \
      -DLATENCY_COMPENSATION=ON \
      -DSINGLE_FRAME=ON \
      -DPARAMETERS_CHECK=ON \
      -DCERTIFICATION=ON \
      -DPUBLIC_KEY_CAPABLE=ON \
      -DVERBOSE=ON \
      -DCRC_HW=OFF \
      -DERROR_CODES=ON \
      -DUL_BIT_RATE_BPS=OFF \
      -DT_IFU_MS=OFF \
      -DT_CONF_MS=OFF \
      -DUL_PAYLOAD_SIZE=OFF \
      -DMESSAGE_COUNTER_ROLLOVER=OFF \
      -DERROR_STACK=12 ..
make lr11xx_rf_api
```

The archive will be generated in the `build/lib` folder.
