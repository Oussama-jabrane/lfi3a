CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Werror -g -fsanitize=address

TARGET = lfi3a
TARGET_FIXED = lfi3a_fixed

SRC_DIR = src
SRC_FIXED_DIR = src_fixed
BUILD_DIR = build
BUILD_FIXED_DIR = build_fixed

SOURCES = $(SRC_DIR)/main.cpp $(SRC_DIR)/Lexer.cpp $(SRC_DIR)/Parser.cpp $(SRC_DIR)/Interpreter.cpp
SOURCES_FIXED = $(SRC_FIXED_DIR)/main.cpp $(SRC_FIXED_DIR)/Lexer.cpp $(SRC_FIXED_DIR)/Parser.cpp $(SRC_FIXED_DIR)/Interpreter.cpp

OBJECTS = $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SOURCES))
OBJECTS_FIXED = $(patsubst $(SRC_FIXED_DIR)/%.cpp,$(BUILD_FIXED_DIR)/%.o,$(SOURCES_FIXED))

all: $(TARGET)

fixx: $(TARGET_FIXED)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(TARGET_FIXED): $(OBJECTS_FIXED)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_FIXED_DIR)/%.o: $(SRC_FIXED_DIR)/%.cpp | $(BUILD_FIXED_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_FIXED_DIR):
	mkdir -p $(BUILD_FIXED_DIR)

clean:
	rm -rf $(BUILD_DIR) $(BUILD_FIXED_DIR)

fclean: clean
	rm -f $(TARGET) $(TARGET_FIXED)

re: fclean all fixx

.PHONY: all fixx clean fclean re
