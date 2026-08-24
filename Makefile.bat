@echo off
g++ -std=c++17 -O3 -Wall -Wextra -pedantic -Iinclude src\*.cpp -o build\main.exe
build\main.exe
pause