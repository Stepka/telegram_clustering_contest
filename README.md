
---

## Run
*You need STL, Boost and C++17 support to compile.*

#### Using CMake

_Windows_

```bash
mkdir build
cd build
cmake .. -T llvm -A x64  -DBoost_NAMESPACE="libboost" -DBoost_COMPILER="-vc141"
```
Then open solution in the Microsoft Visual Studio

_Linux_

Just run cmake
```bash
mkdir build
cd build
cmake ..
make
```