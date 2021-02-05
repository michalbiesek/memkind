#!/bin/bash
# SPDX-License-Identifier: BSD-2-Clause
# Copyright 2021, Intel Corporation
#

# file-exceptions.sh - filter out files not checked for copyright and license
set -e

grep -v -E -e 'jemalloc'