# This is just a wrapper of cmake

debug:
	cmake -S . -B debug-build -DCMAKE_BUILD_TYPE=DEBUG && make -C debug-build
release:
	cmake -S . -B release-build -DCMAKE_BUILD_TYPE=RELEASE && make -C release-build
emscripten:
	emcmake cmake -S . -B emscripten-build -DPLATFORM=Web && make -C emscripten-build

all: debug release emscripten
