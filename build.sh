#!/bin/bash
rm ./ok
gcc -g main.c -o ok -L. -ltreeb -Wl,-rpath,. -I.
