# This is just a wrapper of cmake

debug:
	cmake -S . -B build -DCMAKE_BUILD_TYPE=DEBUG && cmake --build build
release:
	cmake -S . -B release-build -DCMAKE_BUILD_TYPE=RELEASE && cmake --build release-build
emscripten:
	emcmake cmake -S . -B emscripten-build -DPLATFORM=Web && cmake --build emscripten-build

all: debug release emscripten
