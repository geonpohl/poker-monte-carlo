BUILD_DIR := build
CLI := $(BUILD_DIR)/poker_cli
ARGS ?= As Kh

.PHONY: help configure build run test clean rebuild

help:
	@echo "Available targets:"
	@echo "  make configure            Configure the CMake build"
	@echo "  make build                Build the project"
	@echo "  make run ARGS=\"As Kh\"    Build and run the CLI"
	@echo "  make test                 Build and run the test suite"
	@echo "  make clean                Remove the build directory"
	@echo "  make rebuild              Clean, configure, and build again"

configure:
	cmake -S . -B $(BUILD_DIR)

build: configure
	cmake --build $(BUILD_DIR)

run: build
	./$(CLI) $(ARGS)

test: build
	ctest --test-dir $(BUILD_DIR) --output-on-failure

clean:
	rm -rf $(BUILD_DIR)

rebuild: clean build
