WORKSPACE := $(abspath $(CURDIR)/..)
include $(WORKSPACE)/Settings.mk

CMAKE_FLAGS := -DCFG=$(cfg) -DSDK=$(sdk)
# BUILDS_DIR here becomes CMAKE_BINARY_DIR in CMake.

.PHONY: all
all: check_tools cmake_build

.PHONY: check_tools
check_tools:
	components install

.PHONY: cmake_build
cmake_build:
	mkdir -p $(BUILDS_DIR)
	rm -fv $(BUILDS_DIR)/CMakeCache.txt
	cd $(BUILDS_DIR) && cmake $(CMAKE_FLAGS) $(CURDIR)
	$(MAKE) -C $(BUILDS_DIR) -j $(jobs) install

.PHONY: clean
clean:
	rm -rf $(BUILDS_DIR)
