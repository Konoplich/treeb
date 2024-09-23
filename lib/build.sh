#!/bin/bash
F=knowhow.c
I=./test_interface/interface/test_public/public/test_protected/protected/test_private/private/test_knowhow/knowhow
rm ./libtreeb.so
gcc -g -shared -o libtreeb.so -fPIC ${F} -I${I}
cp libtreeb.so ..
cp libtreeb.h ..
