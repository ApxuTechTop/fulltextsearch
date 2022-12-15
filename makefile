.PHONY: build format test clean
build:
	cmake --preset Release
	cmake --build build/Release

format:
	git ls-files *.cpp | xargs clang-format -i -style=file --verbose
	git ls-files *.hpp | xargs clang-format -i -style=file --verbose

test:
	ctest --preset default

clean:
	cmake --build build/Release --target clean

all: format build test

