.PHONY: build format test clean
build:
	cmake --preset Release
	cmake --build build/Release

format:
	clang-format -i src/libmath/math.cpp
	clang-format -i src/sum-cli/main.cpp
	clang-format -i tests/math.test.cpp
	clang-format -i include/libmath/math.hpp

test:
	./build/Release/bin/math_test

clean:
	cmake --build build/Release --target clean

all: format build test

