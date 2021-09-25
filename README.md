# SmallStringOptimizationStudy

Implements a class similar to std::string (some methods are missing yet) and uses small string optimization, designed for little-endian 64 bits architeture, tested in x86_64.

This string implementation has 32 bytes in stack, to fit 2 objects in 1 CPU cache-line (usually 64 bytes). It can store 31 bytes (31 single byte characters) + 1 null terminator, if the string not fits in the 31 byte space it will allocate memory in the heap.

### This project is for study purposes and are not tested sufficiently.

## Building
```
mkdir build
cd build
cmake ..
make -j
```

