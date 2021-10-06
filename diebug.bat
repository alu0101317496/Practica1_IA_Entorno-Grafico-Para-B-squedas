@echo off

pushd debug
clang -g ../source/main.cc -l user32.lib -l gdi32.lib
popd