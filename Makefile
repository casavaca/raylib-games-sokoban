# This is just a wrapper of cmake

debug:
	CC=clang CXX=clang++ cmake -S . -B build -DCMAKE_BUILD_TYPE=DEBUG && cmake --build build
gcc:
	CC=gcc CXX=g++ cmake -S . -B gcc-build -DCMAKE_BUILD_TYPE=RELEASE && cmake --build gcc-build
coverage:
	CC=clang CXX=clang++ cmake -S . -B coverage-build -DCMAKE_BUILD_TYPE=COVERAGE && cmake --build coverage-build
emscripten:
	emcmake cmake -S . -B emscripten-build -DPLATFORM=Web && cmake --build emscripten-build

all: debug gcc coverage emscripten
