#!/bin/sh
if [ $# -ne 2 ]
then
	echo "Usage: ${0} [kiss install path] [spec]"
	exit 0
fi 

cd ${1}
ABSOLUTE_KISS=`pwd`
cd -
echo "KISS=${ABSOLUTE_KISS}" > kiss.pri
qmake -r -spec ${2}
make
make install
