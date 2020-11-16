#!/bin/bash
# SPDX-License-Identifier: BSD-2-Clause
# Copyright (C) 2020 Intel Corporation.

set -e
set -x

QEMU_IMAGE_NAME="$1"
MEMKIND_HOST_WORKDIR="$2"
MEMKIND_MOUNT_TAG=memkind_host

if [[ ! -f "$QEMU_IMAGE_NAME" ]]; then
    echo "QEMU image "$QEMU_IMAGE_NAME" does not exist."
    echo "Usage: $0 image_file host_workdir."
    exit 1
fi

if [[ ! -d "$MEMKIND_HOST_WORKDIR" ]]; then
    echo "Memkind project root directory "$MEMKIND_HOST_WORKDIR" does not exist."
    echo "Usage: $0 image_file host_workdir."
    exit 1
fi

qemu-system-x86_64 -hda "$QEMU_IMAGE_NAME" \
        -enable-kvm \
        -cpu host \
        -vnc :5 \
        -boot c \
        -daemonize \
        -machine hmat=on \
        -netdev user,id=net0,hostfwd=tcp::10022-:22 -device virtio-net,netdev=net0 \
        -fsdev local,security_model=passthrough,id=fsdev0,path="$MEMKIND_HOST_WORKDIR" -device virtio-9p-pci,id=fs0,fsdev=fsdev0,mount_tag="$MEMKIND_MOUNT_TAG" \
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
        -numa hmat-lb,initiator=1,target=3,hierarchy=memory,data-type=access-bandwidth,bandwidth=200G
