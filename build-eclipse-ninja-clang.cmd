mkdir build
cd build
mkdir eclipse-ninja-clang
cd ..
cmake -G "Eclipse CDT4 - Ninja" -S "CMakeSource" -B "build/eclipse-ninja-clang" -DCMAKE_C_COMPILER="clang" -DCMAKE_CXX_COMPILER="clang++" -DCMAKE_ECLIPSE_EXECUTABLE=$ENV{HOME}/eclipse/mars/eclipse.exe -DCMAKE_ECLIPSE_VERSION=4.5 -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX=$ENV{HOME}/GameEngine