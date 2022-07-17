rm -rf build
mkdir build && cd build
export CC=/usr/bin/clang
export CXX=/usr/bin/clang++
cmake ..
#cmake --build . -v
cmake --build .
