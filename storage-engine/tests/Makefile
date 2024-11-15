# Compiler
CXX = g++

# Directories
SRC_DIR = /workspaces/storage-engine-graph-db/storage-engine/lib
TEST_DIR = storage-engine/tests

# Source files
SRC_FILES = $(wildcard $(SRC_DIR)/*.cpp)
TEST_FILES = $(wildcard $(TEST_DIR)/*.cpp)

$(info Compiler: $(SRC_DIR))

# Output executables
OUTPUT_BIN = benchmark_tests

# Compiler flags
CXXFLAGS = -std=c++17 -I$(SRC_DIR) -I/usr/local/include -pthread 
LDFLAGS = -L/usr/local/lib -lgtest -lgtest_main -pthread

# Rules
all: $(OUTPUT_BIN)

$(OUTPUT_BIN): $(SRC_FILES) $(TEST_FILES)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

# Clean rule
clean:
	rm -f $(OUTPUT_BIN)

.PHONY: all clean