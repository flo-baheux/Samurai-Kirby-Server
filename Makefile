CXX := g++
CXXFLAGS := -std=c++20 -Wall -Wextra -pedantic
DEBUGFLAGS := -g

ifeq ($(OS),Windows_NT)
CXXFLAGS += -static
LIBS := -lws2_32
endif

SRC_DIR := ./src
BUILD_DIR := ./build
INCLUDE_DIR := $(shell find $(SRC_DIR) -type d)
INCLUDE_FLAG := $(addprefix -I,$(INCLUDE_DIR)) -I./include

SRCS := $(shell find $(SRC_DIR) -type f -name '*.cpp')
OBJS := $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SRCS))

OUTPUT := server

all: $(OUTPUT)

$(OUTPUT):  $(OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LIBS)

# Rule to compile source files into object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDE_FLAG) -c $< -o $@ 

# Create object directory if it doesn't exist
$(BUILD_DIR):
	mkdir -p $(dir $(OBJS))

## TESTS SECTION
TEST_DIR := ./tests
TEST_BUILD_DIR := ./tests/build
TEST_SRCS := $(shell find $(TEST_DIR) -type f -name '*.cpp')
TEST_OBJS := $(patsubst $(TEST_DIR)/%.cpp,$(TEST_BUILD_DIR)/%.o,$(TEST_SRCS))
TEST_OUTPUT := test

$(TEST_OUTPUT): $(TEST_OBJS) $(filter-out $(BUILD_DIR)/main.o,$(OBJS))
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LIBS)

$(TEST_BUILD_DIR)/%.o: $(TEST_DIR)/%.cpp | $(TEST_BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDE_FLAG) -I$(TEST_DIR) -c $< -o $@

$(TEST_BUILD_DIR):
	mkdir -p $(dir $(TEST_OBJS))
## SECTION END

# Clean rule
clean:
	rm -rf $(BUILD_DIR) $(OUTPUT) $(TEST_BUILD_DIR) $(TEST_OUTPUT)

re: clean all

debug: clean
	$(MAKE) CXXFLAGS="$(CXXFLAGS) -g" all

# Phony targets
.PHONY: all clean re debug