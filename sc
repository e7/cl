#! /bin/bash

if [ -z $1 ]; then
    echo "usage: $0 directory"
elif [ -d $1 ]; then
#find "$1" -regex ".*\(h\|c\|cc\|cxx\|cpp\|py\)" | xargs cat | grep -v ^$ | wc -l
    find "$1" -iregex ".*\.\(h\|c\|cc\|cxx\|cpp\|py\)" | xargs cat | grep -v ^$ | wc -l
else
    echo "$1 is NOT a directory."
    exit -1
fi

exit 0
