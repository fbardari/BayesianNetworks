all:
	g++ -std=c++17 -O3 -Wall -Wextra -pedantic -Iinclude ./src/*.cpp -o build/main.out
	./build/main.out
