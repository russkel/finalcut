#!/bin/bash

if [ $# -gt 0 ]
then
  eval cppcheck --force --enable=all -I../scr/include/ "$@"
else
  eval cppcheck --force --enable=all -I../src/include/ ../src/ ../examples/
fi

