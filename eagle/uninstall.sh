#!/bin/bash

#cd build
sudo systemctl stop eagle.service
sudo systemctl disable eagle.service
#sudo make uninstall
if [ -f "./build/install_manifest.txt" ];then
    cd build
    xargs rm < install_manifest.txt
else
    rm /usr/lib64/libpwrapi_adaptor.so
    rm /etc/eagle/plugin/libsched_service.so
    rm /etc/eagle/plugin/libfreq_service.so
    rm /usr/sbin/eagle
    rm /usr/lib/systemd/system/eagle.service
fi
rm -Rf /etc/eagle
# rm -Rf /var/log/eagle

userdel -r eagle
