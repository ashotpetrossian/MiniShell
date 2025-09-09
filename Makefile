# Compiler and flags
CXX := g++
CXXFLAGS := -std=c++20 -Wall -Wextra -Iinclude -MMD -MP

# Directories
SRC_DIR := src
UTIL_DIR := utility
OBJ_DIR := obj
BIN_DIR := bin

# Find all .cpp files in src/ and utility/
SRC_FILES := $(wildcard $(SRC_DIR)/*.cpp) $(wildcard $(UTIL_DIR)/*.cpp)
OBJ_FILES := $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(notdir $(SRC_FILES)))
DEP_FILES := $(OBJ_FILES:.o=.d)

# Target executable
TARGET := $(BIN_DIR)/minishell

# Default rule
all: $(TARGET)

# Link objects into executable
$(TARGET): $(OBJ_FILES) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Compile src/*.cpp files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compile utility/*.cpp files
$(OBJ_DIR)/%.o: $(UTIL_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Create bin and obj directories if they don't exist
$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# Clean build files
clean:
	rm -rf $(OBJ_DIR) $(TARGET)

# Include auto-generated dependency files
-include $(DEP_FILES)

.PHONY: all clean
