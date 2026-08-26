@echo off
g++ -std=c++17 -O2 -Wall -Wextra -pedantic -Iinclude src\*.cpp -o build\main.exe
build\main.exe
pause