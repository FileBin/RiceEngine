#!/bin/sh

mkdir build
cd build
mkdir eclipse-clang-linux
cd ..

cmake -G "Eclipse CDT4 - Unix Makefiles" -S "CMakeSource" -B "build/eclipse-clang-linux" -DCMAKE_C_COMPILER="clang" -DCMAKE_CXX_COMPILER="clang++" -DCMAKE_ECLIPSE_EXECUTABLE=~/eclipse/eclipse.exe -DCMAKE_ECLIPSE_VERSION=4.5 -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX=$ENV{HOME}/GameEngine
