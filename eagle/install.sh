#!/bin/bash

cd build
sudo make install
useradd -m eagle
chown -R eagle /etc/eagle

mkdir /var/log/eagle
chown -R eagle /var/log/eagle

#Add user "eagle" to POWERAPI list
PWRAPI_CONF_FILE=/etc/sysconfig/pwrapis/pwrapis_config.ini
sed -n '/^admin=/p' $PWRAPI_CONF_FILE | grep eagle > /dev/null
if [ $? -ne 0 ]
then
    sed -i '/^admin=/s/$/,eagle/' $PWRAPI_CONF_FILE
fi

sudo systemctl start eagle.service --now