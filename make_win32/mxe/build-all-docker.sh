#!/bin/sh

./configure-cross-compile-win.sh &&
( cd ../.. && make ) &&
./build-cre.sh
