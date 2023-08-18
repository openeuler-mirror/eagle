#!/bin/bash

#cd build
sudo systemctl stop eagle.service
sudo systemctl disable eagle.service
#sudo make uninstall
if [ -f "./build/install_manifest.txt" ];then
    cd build
    xargs rm < install_manifest.txt
else
    rm /usr/local/lib/libpwrapi_adaptor.so
    rm /usr/local/lib/libsched_service.so
    rm /usr/local/lib/libfreq_service.so
    rm /usr/sbin/eagle
    rm /etc/eagle/eagle_config.ini
    rm /usr/lib/systemd/system/eagle.service
fi