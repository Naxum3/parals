# first task  
cmake  
float  
    mkdir build && cd build  
    cmake ..  
    cd ../  
    cmake --buid build  
    cd ./build/debug  
    task1.exe  
  
double  
    mkdir build && cd build  
    cmake -DUSE_DOUBLE=ON ..  
    cd ../  
    cmake --build build  
    cd ./build/debug  
    task1.exe  
  
make  
float  
    make  
  
double  
    make USE_DOUBLE=1  
