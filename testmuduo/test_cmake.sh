[ -d build ] && rm -rf build
mkdir -p build && cd build
cmake .. && make
cd ../bin
./muduo_chat_server &   