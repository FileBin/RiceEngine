mkdir build
cd build
cmake -G "Eclipse CDT4 - MinGW Makefiles" -S "../CmakeSource" -DCMAKE_ECLIPSE_EXECUTABLE=$ENV{HOME}/eclipse/mars/eclipse.exe -DCMAKE_ECLIPSE_VERSION=4.5 -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX=$ENV{HOME}/GameEngine