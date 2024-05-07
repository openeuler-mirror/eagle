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
    rm /usr/lib64/libsched_service.so
    rm /usr/lib64/libfreq_service.so
    rm /usr/sbin/eagle
    rm -R /etc/eagle
    rm /usr/lib/systemd/system/eagle.service
fi
# rm -R /var/log/eagle

userdel -r eagle
