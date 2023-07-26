#!/bin/bash

#cd build
sudo systemctl stop eagle.service
sudo systemctl disable eagle.service
#sudo make uninstall
if [ -f "./build/install_manifest.txt" ];then
    cd build
    xargs rm < install_manifest.txt
    else
    rm /usr/lib/libpwrapi_adaptor.so
    rm /usr/sbin/eagle
    rm /etc/sysconfig/eagle_config.ini
    rm /usr/lib/systemd/system/eagle.service
fi