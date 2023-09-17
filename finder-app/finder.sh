#!/bin/sh



if [ $# -lt 2 ]
then
	echo "filesdir or searchstr does not specified"
	exit 1	
else
	FILESDIR=$1
	SEARCHSTR=$2
fi

if [ ! -d "$FILESDIR" ]
then
	echo "filesdir does not represent a directory on the filesystem"
	exit 1
fi

X=$(find "$FILESDIR" -type f | wc -l)
Y=$(grep -r "$SEARCHSTR" $FILESDIR | wc -l)
echo "The number of files are $X and the number of matching lines are $Y"