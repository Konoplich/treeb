#!/bin/bash
F=interface.c
I=./test_public/public/test_protected/protected/test_private/private/test_knowhow/knowhow
gcc -c ${F} -I${I}
