#!/bin/bash

if [[ $# -eq 0 ]]; then
echo "Usage: check.sh [file] [ [file] ... ]"
    exit 2
fi

tmpfile=`mktemp .tmp.XXXXXXXX`

for file in $@; do
if [[ ! -e $file ]]; then
echo "File $file does not exist"
rm $tmpfile; exit 3
    fi

if [[ ! -e "$file.sorted" ]]; then
echo "File $file.sorted does not exist"
rm $tmpfile; exit 4
    fi

sort -n $file > $tmpfile

    diff "$file.sorted" $tmpfile >/dev/null
    if [[ $? -eq 0 ]]; then
echo "File $file.sorted is the sorted version of $file"
    else
echo "File $file.sorted is NOT the sorted version of $file"
rm $tmpfile; exit 5
    fi
done

if [[ ! -e "sorted.txt" ]]; then
echo "File sorted.txt does not exist"
    rm $tmpfile; exit 6
fi

sort -u -n $@ > $tmpfile

diff sorted.txt $tmpfile >/dev/null
if [[ $? -eq 0 ]]; then
echo "File sorted.txt is the merged version of the sorted files"
else
echo "File sorted.txt is NOT the merged version of the sorted files"
    rm $tmpfile; exit 7
fi

rm $tmpfile; exit 0
