#!/bin/bash

cd build
sudo make install 
sudo systemctl start eagle.service --now