BUILD_DIR := build
PERF_BUILD_DIR := build-release
CLI := $(BUILD_DIR)/poker_cli
PERF := $(PERF_BUILD_DIR)/poker_perf
ARGS ?= As Kh
PERF_ARGS ?= 200000

.PHONY: help configure build run test perf clean rebuild

help:
	@echo "Available targets:"
	@echo "  make configure            Configure the CMake build"
	@echo "  make build                Build the project"
	@echo "  make run ARGS=\"As Kh\"    Build and run the CLI"
	@echo "  make test                 Build and run the test suite"
	@echo "  make perf PERF_ARGS=200000 Build release benchmark and run it"
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

perf:
	cmake -S . -B $(PERF_BUILD_DIR) -DCMAKE_BUILD_TYPE=Release
	cmake --build $(PERF_BUILD_DIR)
	./$(PERF) $(PERF_ARGS)

clean:
	rm -rf $(BUILD_DIR)
	rm -rf $(PERF_BUILD_DIR)

rebuild: clean build
