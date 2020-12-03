#!/bin/bash
# SPDX-License-Identifier: BSD-2-Clause
# Copyright (C) 2019 - 2020 Intel Corporation.

#
# docker_run_build.sh - is called inside a Docker container;
# prepares and runs memkind build for specified pull request number
#
set -e

export UTILS_PREFIX=utils/docker

"$UTILS_PREFIX"/docker_install_libvirt.sh
