#!/bin/sh

./configure-cross-compile-win.sh &&
( cd ../.. && make CFLAGS="${CFLAGS} -DCF_MXE_CROSS_COMPILE=1" CPPFLAGS="${CPPFLAGS} -DCF_MXE_CROSS_COMPILE=1" ) &&
./build-cre.sh
