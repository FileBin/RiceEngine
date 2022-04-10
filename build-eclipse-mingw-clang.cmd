mkdir build
cd build
mkdir eclipse-mingw-clang
cd ..
cmake -G "Eclipse CDT4 - MinGW Makefiles" -S "CMakeSource" -B "build/eclipse-mingw-clang" -DCMAKE_C_COMPILER="clang" -DCMAKE_CXX_COMPILER="clang++" -DCMAKE_ECLIPSE_EXECUTABLE=$ENV{HOME}/eclipse/mars/eclipse.exe -DCMAKE_ECLIPSE_VERSION=4.5 -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX=$ENV{HOME}/GameEngine