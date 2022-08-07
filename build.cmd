mkdir build
cd build
cmake -S "../CmakeSource" -B "default" -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_C_COMPILER=clang -DCMAKE_INSTALL_PREFIX=$ENV{HOME}/GameEngine
