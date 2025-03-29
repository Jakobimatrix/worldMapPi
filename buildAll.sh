#!/bin/sh -e

echo "updating\n"
git submodule update --init --recursive

echo "building and installing WiringPi\n"

cd WiringPiLibrary/WiringPi
./build

echo "======================================================\n"
gpio readall
echo "======================================================\n"

cd ../../programs

[ -d build ] || mkdir build
cd build
cmake --build .
sudo make install

