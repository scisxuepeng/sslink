#!/bin/bash

rm -rf res.txt

./build.sh


cd microbenchmark
rm -rf build/
mkdir build
cd build
cmake -DSSLINK=ON ../
make
echo "SSLink version: " >> ../../res.txt
for k in $( seq 1 5 )
do
  ./ff_fib >> ../../res.txt
done


cd ../
rm -rf build/
mkdir build
cd build
cmake -DSSLINK=OFF ../
make
echo "Original version: " >> ../../res.txt
for k in $( seq 1 5 )
do
  ./ff_fib >> ../../res.txt
done

cd ../../
cat res.txt
