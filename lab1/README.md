# first task
cmake
float
    mkdir build && cd build
    cmake ..
    make
    ./task1

double
    mkdir build && cd build
    cmake -DUSE_DOUBLE=ON ..
    make
    ./task1

make
float
    make

double
    make USE_DOUBLE=1
