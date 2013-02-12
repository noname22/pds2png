#!/bin/bash
# Quick script to convert all files in a given directory 

if [ "$1" != "" ]; then
	files=`find $1 -iname "*.RED" | xargs`

	for file in $files; do
		base=${file%.RED}
		echo $base;

		./pds2png $base.RED $base.GRN $base.BLU $base.png
	done
else
	echo "usage: $0 [directory with RED/GRN/BLU files]"
fi
