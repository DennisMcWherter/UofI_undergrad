#!/bin/bash

# Generate some possible solutions
echo "a" > test1.test
echo "b" >> test1.test
echo "c" >> test1.test
echo "d" >> test1.test
echo "e" >> test1.test
echo "f" >> test1.test
echo "g" >> test1.test

# Not sure, but seems EWS machines are limited to < 500 threads per process
# unless this is just a limitation of valgrind.
# So 498 threads + 1 main thread = 499 threads < 500
touch check_against
for((c = 1 ; c < 499 ; c = c+82))
do
	valgrind --leak-check=full -q --log-file=test1 ./parmake -j$c testfile1 > test1.out
	valgrind --leak-check=full -q --log-file=test2 ./parmake -j$c testfile2 > test2.out
  if diff check_against test1 && diff test1.test test1.out ; then
    echo "Success: Test1 $c threads"
  else
    echo "Error: Test1 with $c threads"
  fi
  if diff check_against test2; then
    echo "Success: Test2 $c threads"
  else
    echo "Error: Test2 with $c threads"
  fi
done
rm -rf test1
rm -rf test2
rm -rf test1.test
rm -rf test2.test
rm -rf check_against
rm -rf test1.out
rm -rf test2.out
