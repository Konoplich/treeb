#!/bin/bash
F=public.c
I=./test_protected/protected/test_private/private/test_knowhow/knowhow
gcc -c ${F} -I${I}
