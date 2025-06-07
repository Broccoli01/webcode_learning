
[ -d build ] && rm -rf build
mkdir -p build && cd build
cmake .. && make
cd ../bin
./MySQL_Demo &   