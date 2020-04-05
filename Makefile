# Set default values for missing environment
BUILD_TYPE ?= Debug

BUILD_DIR ?= build

# on our build environment we use cmake28, so we need to detect which cmake command to use
CMAKE := cmake

# find a build program, prefer ninja to make
MAKE_PROG := $(shell command -v ninja-build || command -v ninja || command -v make)

# set a cmake generator depending on our build program
ifeq ($(findstring ninja,$(MAKE_PROG)), ninja)
CMAKE_GENERATOR := Ninja
else
CMAKE_GENERATOR := "Unix Makefiles"
endif

MAKE_COMMAND := cd $(BUILD_DIR) && $(MAKE_PROG)

all: build

cmake-setup:
	@mkdir -p $(BUILD_DIR) && cd $(BUILD_DIR);$(CMAKE) -G $(CMAKE_GENERATOR) -DCMAKE_BUILD_TYPE=$(BUILD_TYPE) ..

build: cmake-setup
	$(MAKE_COMMAND) all

cppcheck:
	@mkdir -p $(BUILD_DIR) && cd $(BUILD_DIR);$(CMAKE) -G $(CMAKE_GENERATOR) -DCMAKE_BUILD_TYPE=$(BUILD_TYPE) -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ..
	cppcheck --enable=all --language=c++ -UASSETS_FOLDER -UNDEBUG --project=build/compile_commands.json

clean:
	rm -rf $(BUILD_DIR)/

test:
	@build/combatris/combatris_test

run:
	@build/combatris/combatris
