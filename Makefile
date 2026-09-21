.PHONY: all build test docs clean install

BUILD_DIR ?= build
CMAKE_GENERATOR ?= $(shell which ninja >/dev/null 2>&1 && echo "Ninja" || echo "Unix Makefiles")

all: build

build:
	cmake -B $(BUILD_DIR) -G "$(CMAKE_GENERATOR)"
	cmake --build $(BUILD_DIR)

test: build
	ctest --test-dir $(BUILD_DIR) --output-on-failure

docs:
	cmake -B $(BUILD_DIR) -G "$(CMAKE_GENERATOR)"
	cmake --build $(BUILD_DIR) --target docs

install: build
	cmake --install $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR)
