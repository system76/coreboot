# coreboot-configurator ![alt text](images/StarLabs_Logo.png "Star Labs Systems")

A simple GUI to change settings in coreboot's CBFS, via the nvramtool utility.

![coreboot-configurator](images/coreboot-configurator.gif)
# How to install
## Ubuntu, Linux Mint, elementary OS, Zorin OS and other derivates
##### Install
```
sudo add-apt-repository ppa:starlabs/coreboot
sudo apt update
sudo apt install coreboot-configurator
```
##### Uninstall
```
sudo apt purge coreboot-configurator
```

## Debian 11
##### Install
```
echo "deb http://ppa.launchpad.net/starlabs/ppa/ubuntu focal main" | sudo tee -a /etc/apt/sources.list.d/starlabs-ubuntu-ppa-focal.list
sudo apt-key adv --keyserver keyserver.ubuntu.com --recv-keys 17A20BAF70BEC3904545ACFF8F21C26C794386E3
sudo apt update
sudo apt install coreboot-configurator
```

##### Uninstall
```
sudo apt purge coreboot-configurator
```

## Manjaro
##### Install
```
sudo pamac install coreboot-configurator
```
##### Uninstall
```
sudo pamac remove coreboot-configurator
```

## Other Distributions
##### Install
```
git clone https://github.com/StarLabsLtd/coreboot-configurator.git
cd coreboot-configurator
meson build
ninja -C build install
```
##### Uninstall
```
sudo ninja -C uninstall
```

# Advanced Mode
Enabling advanced mode will all you to see all settings contained inside coreboot. Tread carefully :)

## Copying or Reusing
Included scripts are free software licensed under the terms of the [GNU General Public License, version 2](https://www.gnu.org/licenses/gpl-2.0.txt).

# [© Star Labs® / All Rights Reserved.](https://starlabs.systems)
Any issues or questions, please contact us at [support@starlabs.systems](mailto:supportstarlabs.systems)

View our full range of Linux laptops at: [https://starlabs.systems](https://starlabs.systems)
