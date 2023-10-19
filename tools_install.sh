#!/bin/bash
set -x #echo on

# ouster_ether REQUIRES
# sudo apt install libpcap-dev

# ouster_view REQUIRES
# sudo apt-get install build-essential libgl1-mesa-dev
# sudo apt install libx11-dev


bake rebuild -r --cfg release tools
sudo cp $HOME/bake/x64-Linux/release/bin/ouster_capture1 /usr/local/bin/
sudo cp $HOME/bake/x64-Linux/release/bin/ouster_replay1 /usr/local/bin/
sudo cp $HOME/bake/x64-Linux/release/bin/ouster_snapshot /usr/local/bin/
sudo cp $HOME/bake/x64-Linux/release/bin/ouster_meta /usr/local/bin/
sudo cp $HOME/bake/x64-Linux/release/bin/ouster_monitor /usr/local/bin/
sudo cp $HOME/bake/x64-Linux/release/bin/ouster_view /usr/local/bin/