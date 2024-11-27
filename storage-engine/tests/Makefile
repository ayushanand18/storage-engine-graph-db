# Set the compiler
CXX = g++
CXXFLAGS = -std=c++17 -I../lib  # Include the lib directory for header files
LDFLAGS = -L../lib  # Link libraries if needed (not necessary for this case)

# All source files needed for the project (add all .cpp files in lib/)
SOURCES = \
    lib/core/memtable.cpp \
    lib/core/graph_node.cpp \
    lib/core/compaction_manager.cpp \
    lib/core/merge_log.cpp \
    lib/core/object_cache.cpp \
    lib/core/sstable.cpp \
    lib/core/utils.cpp \
    lib/core/uuid_generator.cpp \
    lib/index/node_data_index.cpp \
    lib/index/node_id_index.cpp \
    lib/persistence/flushing_manager.cpp \
    lib/persistence/durability_manager.cpp \
    lib/concurrency/thread_pool.cpp \
    lib/concurrency/lock_manager.cpp \
    tests/storage_engine_build_example.cpp  # Your testing file

# The object files generated from the source files
OBJECTS = $(SOURCES:.cpp=.o)

# The final output executable
# TARGET = storage_engine_build_example
$(info Path: $(OBJECTS) $(CXX) $(LDFLAGS) -o $(TARGET) $(OBJECTS))
# Rule to build the executable
$(TARGET): $(OBJECTS)
	$(CXX) $(LDFLAGS) -o $(TARGET) $(OBJECTS)

# Rule to compile .cpp files into .o files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean rule to remove generated files
clean:
	rm -f $(OBJECTS) $(TARGET)

# # Compiler
# CXX = g++

# # Directories
# SRC_DIR = /workspaces/storage-engine-graph-db/storage-engine/lib
# TEST_DIR = storage-engine/tests

# # Source files
# SRC_FILES = $(wildcard $(SRC_DIR)/*.cpp)
# TEST_FILES = $(wildcard $(TEST_DIR)/*.cpp)

# $(info Compiler: $(SRC_DIR))

# # Output executables
# OUTPUT_BIN = benchmark_tests

# # Compiler flags
# CXXFLAGS = -std=c++17 -I$(SRC_DIR) -I/usr/local/include -pthread 
# LDFLAGS = -L/usr/local/lib  -pthread # -lgtest -lgtest_main

# # Rules
# all: $(OUTPUT_BIN)

# $(OUTPUT_BIN): $(SRC_FILES) $(TEST_FILES)
# 	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

# # Clean rule
# clean:
# 	rm -f $(OUTPUT_BIN)

# .PHONY: all clean