# STM32 ILI9341 spi
TOC:
1. [Title1](#1-title-one)
    1. [Title11](#11-subtitle-oneone)
    2. [Title12](#12-subtitle-onetwo)
2. [Title2](#2-title-two)
    1. [Title21](#21-subtitle-twoone)
3. [Development build (for developer PC)](#3-development-build-for-developer-pc)

## 1. Title one

stm-ILI9341-spi-master

### 1.1. Subtitle OneOne
Subtitle text 1-1

### 1.2. Subtitle OneTwo
Subtitle text 1-2

```bash
 openocd -f interface/stlink-v2.cfg -f target/stm32f1x.cfg -c "program build/$(TARGET).elf verify exit reset"
```

## 2. Development build (for developer PC)
### 2.1. Configure `lte_monitor` build:
``` bash
meson setup -Duser=$USER build
```

Or configure `lte_monitor` **debug** build:
``` bash
meson setup -Duser=$USER --buildtype=debug build
```

> Note! To test with LTE-operator without real IP there is an option **FAKE_REAL_IP** in `app.h.in`
``` c
#define FAKE_REAL_IP        false
```
Default value is **false**. With **true** any local ip is considered as real one, and 0.0.0.0 is used as bind address for sshd

### 2.2. Compile `lte_monitor`:
``` bash
ninja -C build
```

### 2.3. Install/reinstall `lte_monitor` as **service**:
``` bash
sudo systemctl stop lte_monitor
sudo ninja install -C build
```

## 3. Production build (for Emak)

### 3.1. Configure `lte_monitor` build:
``` bash
meson setup --cross-file emak.ini --strip dist
```

### 3.2. Compile `lte_monitor` and create deb-package:
``` bash
ninja -C dist install
```
 > deb file would be generated in `deb.out` folder.

## 4. Run on Emak
 > Note: In this examples considered Emak IP is **10.0.0.200** and **VERSION** is a utility version

### 4.1. Copy deb-package to Emak
``` bash
scp deb.out/defigo-lte-monitor_VERSION_arm64.deb defigo@10.0.0.200:/home/defigo
```

### 4.2. Install deb-package on Emak
``` bash
ssh defigo@10.0.0.200
sudo dpkg -i defigo-lte-monitor_VERSION_arm64.deb
```

### 4.3. Run `lte_monitor` *service*:
``` bash
sudo systemctl start lte_monitor
```

## 5. Additional info
There are two log files:
 - In **text** format in `/var/log/defigo/lte_monitor.log`
 - In **SQLite** DB format in `/var/local/defigo/lte_monitor.db` with `log` table:

| id  |  dt  | file | func | line | msg  |
| --- | ---- | ---- | ---- | ---- | ---- |
| 1 | 2023-04-13 08:38:09.809869 | ../src/main.c | main | 94  | Started |
| 2 | 2023-04-13 08:38:09.828333 | ../src/service.c | service_init | 43  | Unique name: :1.297 |