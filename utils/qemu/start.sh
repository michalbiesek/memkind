#!/bin/bash
# SPDX-License-Identifier: BSD-2-Clause
# Copyright (C) 2016 - 2020 Intel Corporation.

set -e
set -x

qemu-system-x86_64 -hda qemu-image/ubuntu-20.04.img -enable-kvm -cpu host -vnc :5 \
    -machine hmat=on \
    -m 2G \
    -object memory-backend-ram,size=512M,id=m0 \
    -object memory-backend-ram,size=512M,id=m1 \
    -object memory-backend-ram,size=512M,id=m2 \
    -object memory-backend-ram,size=512M,id=m3 \
    -smp 2 \
    -numa node,nodeid=0,memdev=m0,initiator=0 \
    -numa node,nodeid=1,memdev=m1,initiator=1 \
    -numa node,nodeid=2,memdev=m2,initiator=0 \
    -numa node,nodeid=3,memdev=m3,initiator=1 \
    -numa cpu,node-id=0,socket-id=0 \
    -numa cpu,node-id=1,socket-id=1 \
    -numa hmat-lb,initiator=0,target=0,hierarchy=memory,data-type=access-latency,latency=5 \
    -numa hmat-lb,initiator=0,target=0,hierarchy=memory,data-type=access-bandwidth,bandwidth=20G \
    -numa hmat-lb,initiator=0,target=1,hierarchy=memory,data-type=access-latency,latency=10 \
    -numa hmat-lb,initiator=0,target=1,hierarchy=memory,data-type=access-bandwidth,bandwidth=10G \
    -numa hmat-lb,initiator=0,target=2,hierarchy=memory,data-type=access-latency,latency=10 \
    -numa hmat-lb,initiator=0,target=2,hierarchy=memory,data-type=access-bandwidth,bandwidth=200G \
    -numa hmat-lb,initiator=0,target=3,hierarchy=memory,data-type=access-latency,latency=10 \
    -numa hmat-lb,initiator=0,target=3,hierarchy=memory,data-type=access-bandwidth,bandwidth=180G \
    -numa hmat-lb,initiator=1,target=0,hierarchy=memory,data-type=access-latency,latency=10 \
    -numa hmat-lb,initiator=1,target=0,hierarchy=memory,data-type=access-bandwidth,bandwidth=10G \
    -numa hmat-lb,initiator=1,target=1,hierarchy=memory,data-type=access-latency,latency=5 \
    -numa hmat-lb,initiator=1,target=1,hierarchy=memory,data-type=access-bandwidth,bandwidth=20G \
    -numa hmat-lb,initiator=1,target=2,hierarchy=memory,data-type=access-latency,latency=10 \
    -numa hmat-lb,initiator=1,target=2,hierarchy=memory,data-type=access-bandwidth,bandwidth=180G \
    -numa hmat-lb,initiator=1,target=3,hierarchy=memory,data-type=access-latency,latency=10 \
    -numa hmat-lb,initiator=1,target=3,hierarchy=memory,data-type=access-bandwidth,bandwidth=200G &
