# SPDX-License-Identifier: BSD-2-Clause
# Copyright (C) 2014 - 2020 Intel Corporation.

lib_LTLIBRARIES += autodax/libautodax.la \
                   # end

autodax_libautodax_la_LIBADD = libmemkind.la

autodax_libautodax_la_SOURCES = autodax/autodax.c

clean-local: autodax-clean

autodax-clean:
	rm -f autodax/*.gcno
