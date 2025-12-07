# Testsuite target.
TARGET := testsuite

# Source directory.
SOURCE_DIR := ../source

# Source files - update this list as new source files are added to the system.
SOURCE_FILES := $(SOURCE_DIR)/arch/test/hw_platform.cpp \
                $(SOURCE_DIR)/driver/adc/atmega328p.cpp \
                $(SOURCE_DIR)/driver/eeprom/atmega328p.cpp \
                $(SOURCE_DIR)/driver/gpio/atmega328p.cpp \
                $(SOURCE_DIR)/driver/serial/atmega328p.cpp \
                $(SOURCE_DIR)/driver/tempsensor/tmp36.cpp \
                $(SOURCE_DIR)/driver/timer/atmega328p.cpp \
                $(SOURCE_DIR)/driver/watchdog/atmega328p.cpp \
                $(SOURCE_DIR)/logic/logic.cpp \
                $(SOURCE_DIR)/utils/utils.cpp \

# Test files - update this list as new test files are added to the system.
TEST_FILES := testsuite.cpp \
              driver/adc/atmega328p_test.cpp \
              driver/tempsensor/tmp36_test.cpp \
              logic/logic_test.cpp \

# All files.
ALL_FILES := $(SOURCE_FILES) $(TEST_FILES)

# Main include directory.
INC_DIR := ../include

# Gtest directory.
GTEST_DIR := /usr/include

# C++ compiler.
CXX_COMPILER = g++

# C++ compiler flags.
COMPILER_FLAGS = -std=c++17 -Werror -Wall -I$(INC_DIR) -I$(GTEST_DIR) -DTESTSUITE

# Linked libraries.
LINK_LIBS = -lgtest -lgmock -lgtest_main -lpthread

# Build and run the test suite as default:
default: build run

# Build the test suite.
build:
	@$(CXX_COMPILER) $(ALL_FILES) -o $(TARGET) $(COMPILER_FLAGS) $(LINK_LIBS)

# Run the test suite.
run:
	@./$(TARGET)

# Clean the test suite.
clean:
	@rm -f $(TARGET)
