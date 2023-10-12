#!/bin/bash
set -x #echo on

bake rebuild -r --cfg release tools
sudo cp /home/johan/bake/x64-Linux/release/bin/ouster_capture1 /usr/local/bin/
sudo cp /home/johan/bake/x64-Linux/release/bin/ouster_replay1 /usr/local/bin/
sudo cp /home/johan/bake/x64-Linux/release/bin/ouster_snapshot /usr/local/bin/
sudo cp /home/johan/bake/x64-Linux/release/bin/ouster_ether /usr/local/bin/
sudo cp /home/johan/bake/x64-Linux/release/bin/ouster_meta /usr/local/bin/
sudo cp /home/johan/bake/x64-Linux/release/bin/ouster_monitor /usr/local/bin/