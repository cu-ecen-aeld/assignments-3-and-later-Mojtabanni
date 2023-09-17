#!/bin/sh

if [ $# -lt 2 ]
then
	echo "writefile or writestr does not specified"
	exit 1	
else
	WRITEFILE=$1
	WRITESTR=$2
fi

WRITEDIR=`dirname $WRITEFILE`
mkdir -p $WRITEDIR
OUTPUTSTRING=$(echo $WRITESTR > $WRITEFILE)

set +e
if [ ! $? -eq 0 ]; then
	echo "failed: ${OUTPUTSTRING}"
	exit 1
fi

