# cpp_app

# install tools:
sudo apt-get install -y git cmake build-essential autoconf libtool pkg-config qt5-default

# choose a directory to hold locally installed packaged
export MY_INSTALL_DIR=$HOME/.local
mkdir -p $MY_INSTALL_DIR
export PATH="$PATH:$MY_INSTALL_DIR/bin"

# install new cmake version (later than available on ubuntu 18.04 is required)
wget -q -O cmake-linux.sh https://github.com/Kitware/CMake/releases/download/v3.18.4/cmake-3.18.4-Linux-x86_64.tar.gz
sh cmake-linux.sh -- --skip-license --prefix=$MY_INSTALL_DIR
rm cmake-linux.sh

# clone repository with submodules:
git clone --recurse-submodules https://github.com/irenicus30/cpp_app.git

# build and install gRPC and Protocol Buffers from submodule
cd cpp_app/grpc
mkdir -p cmake/build
cd cmake/build
cmake -DgRPC_INSTALL=ON -DgRPC_BUILD_TESTS=OFF -DCMAKE_INSTALL_PREFIX=$MY_INSTALL_DIR ../..
make
sudo make install
cd ../../..

# create file wiyh sqlite database 'example.db'
python3 initial_database_migration.py example.db

# build cpp_app
mkdir -p cmake/build
cmake ../..
make

# run app with created database as command line argument
./server ../../example.bd


