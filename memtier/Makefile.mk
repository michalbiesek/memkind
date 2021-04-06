# SPDX-License-Identifier: BSD-2-Clause
# Copyright (C) 2021 Intel Corporation.

lib_LTLIBRARIES += memtier/libmemtier.la \
                   # end

memtier_libmemtier_la_SOURCES = memtier/ctl.c \
                  memtier/ctl.h \
                  memtier/memtier.c \
                  memtier/memtier_log.c \
                  memtier/memtier_log.h \
                  # end

memtier_libmemtier_la_LIBADD = libmemkind.la

clean-local: memtier-clean

memtier-clean:
	rm -f memtier/*.gcno
