.PHONY: default
default: compress decompress

compress: compress.cpp node.hpp
	g++ -Wall -Wextra -Wpedantic --std=c++17 -O3 -g $< -o $@

decompress: decompress.cpp node.hpp
	g++ -Wall -Wextra -Wpedantic --std=c++17 -O3 -g $< -o $@

clean:
	rm compress decompress
