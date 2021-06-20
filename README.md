# Kernel-Stop-Machine

## Overview
キャラクデバイスからシステムを停止

## Requirement
- Ubuntu20.04.2
- kernel5.8.0-53
- GCC

## How to install

HTTP
```shell
git clone https://github.com/fumiya5863/Kernel-Stop-Machine.git
```

SSH
```shell
git clone git@github.com:fumiya5863/Kernel-Stop-Machine.git
```

## How to use(HelloWorld)

```shell
sudo apt install linux-headers-$(uname -r)
cd Kernel-Stop-Machine
make
sudo insmod chardev_stop_machine.ko
sudo chown $USER:$USER /dev/stop-machine
echo 5000 > /dev/stop-machine

```

## Author
[fumiya5863](https://github.com/fumiya5863)
