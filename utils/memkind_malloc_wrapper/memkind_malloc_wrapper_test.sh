#!/bin/bash
# SPDX-License-Identifier: BSD-2-Clause
# Copyright (C) 2020 Intel Corporation.

WRAPPER_LIB=libmemkind_malloc_wrapper.so
WRAPPER_LIB_DBG=libmemkind_malloc_wrapper_dbg.so

#Test MEMKIND_DEFAULT kind
MEMKIND_DAX_KMEM_ON=0 LD_LIBRARY_PATH="$(pwd)/.libs" LD_PRELOAD=$WRAPPER_LIB
#Test PMEM kind
PMEM_KIND_PATH=/tmp LD_LIBRARY_PATH="$(pwd)/.libs" LD_PRELOAD=$WRAPPER_LIB
#Test MEMKIND_DAX_KMEM kind
MEMKIND_DAX_KMEM_ON=1 MEMKIND_DAX_KMEM_NODES=0 LD_LIBRARY_PATH="$(pwd)/.libs" LD_PRELOAD=$WRAPPER_LIB ls

MEMKIND_DAX_KMEM_ON=0 LD_LIBRARY_PATH="$(pwd)/.libs" LD_PRELOAD=$WRAPPER_LIB_DBG ls
#Test PMEM kind
PMEM_KIND_PATH=/tmp LD_LIBRARY_PATH="$(pwd)/.libs" LD_PRELOAD=$WRAPPER_LIB_DBG  ls
#Test MEMKIND_DAX_KMEM kind
MEMKIND_DAX_KMEM_ON=1 MEMKIND_DAX_KMEM_NODES=0 LD_LIBRARY_PATH="$(pwd)/.libs" LD_PRELOAD=$WRAPPER_LIB_DBG ls
