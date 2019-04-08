# Installing memkind


## Building
The memkind library has a dependency on a related fork of jemalloc.
Building and installing memkind in standard system location can be as simple as
typing the following while in the root directory of the source tree:

    ./build.sh
    make install

To install this library into **other locations**, you can use the prefix variable, e.g.:

    ./build.sh prefix=/usr/
    make install

This will install files to /usr/lib, /usr/include, /usr/share/doc/, usr/share/man.

## Build Prerequisites

You will need to install required packages on the build system:

* **autoconf**
* **automake**
* **gcc-c++**
* **libnuma-devel**
* **libtool**
* **numactl-devel**
* **unzip**

The configure scripts and gtest source code are distributed with the
source tarball included in the source RPM, and this tarball is created
with the memkind "make dist" target.  In contrast to the distributed source
tarball, the git repository does not include any generated files.
For this reason some additional steps are required when building
from a checkout of the git repo.  Those steps include running
the bash script called "autogen.sh" prior to configure.  This script
will populate a VERSION file based on "git describe", and use
autoreconf to generate a configure script.

## jemalloc
The jemalloc source was forked from jemalloc version 5.0.  This source tree
is located within the jemalloc subdirectory of the memkind source.  The jemalloc
source code has been kept close to the original form, and in particular
the build system has been lightly modified.
The developer must configure and build jemalloc prior to configuring
and building memkind.  You can do that using included shell script:

    export JE_PREFIX=jemk_
    ./build_jemalloc.sh

To configure and build jemalloc, you can follow this step-by-step instruction:

    cd jemalloc
    autoconf
    mkdir obj
    cd obj
    ../configure --enable-autogen --with-jemalloc-prefix=jemk_ --without-export \
                 --with-version=5.0.0-0-g0 --disable-fill --disable-initial-exec-tls \
                 --with-malloc-conf="narenas:256,lg_tcache_max:12"
    make
    cd ../..

**Note:**

JE_PREFIX can be set to arbitrary value, including empty one.

## memkind library

To configure, build and install memkind, you can follow this step-by-step instruction:

    export JE_PREFIX=jemk_
    ./autogen.sh
    ./configure
    make
    make install

See the output of:

    ./configure --help

for more information about either the memkind or the jemalloc
configuration options.  Some useful information about building with autotools
can also be found in the [INSTALL](INSTALL) file.

**Important Notes:**

If you are using build.sh script and later want to call 'make' command directly,
then you need to call firstly:

    export JE_PREFIX=jemk_

otherwise you will get an error like:

    undefined reference to mallocx
