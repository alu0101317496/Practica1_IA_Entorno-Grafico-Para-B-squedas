@echo off

pushd exec
clang ../source/*.cc -o main.exe -fms-compatibility -l user32.lib -l gdi32.lib 
popd