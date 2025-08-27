#!/bin/bash
if [[ "$1" == "-h" ]]; then
	echo ""
	echo "Usage : make.api.h"
	exit -1
fi
echo "#pragma once"
for f in $(cat source.list | sed 's/.c$//')
do
	echo "//file:$f.c"
	cheader.api include/$(basename $f .o).h
done
