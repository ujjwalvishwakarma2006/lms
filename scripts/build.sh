rm -rf *
cmake.exe ..
cmake.exe --build . --config Debug
if [ $? -ne 0 ]; then
    echo "Build failed"
    exit 1
fi
echo "Build succeeded"
