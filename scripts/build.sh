#!/bin/sh
if [ $# -ne 1 ]
then
	echo "Usage: ${0} [kiss install path]"
	exit 0
fi 

cd ${1}
echo "KISS=${1}" > kiss.pri
qmake -r
make
make install
