if [ $# -eq 0 ]
  then
    echo 'Provide a user-defined preset'
    exit 1
fi
cmake -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=ON --preset=$1
cmake --build build
