.PHONY: build format test clean install
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

install:
	sudo mv build/Release/src/luafts/libluafts.so /usr/local/lib/lua/5.4/libluafts.so

all: format build test

