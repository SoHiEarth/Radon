cmake -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=ON --preset=$1
cmake --build build
