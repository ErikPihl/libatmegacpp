

## Install Google Test

```bash
sudo apt -y update
sudo apt -y install libgtest-dev libgmock-dev cmake
```

```bash
cd /usr/src/gtest
sudo cmake .
sudo make
sudo mv lib/*.a /usr/lib
```

```makefile
# Target.
TARGET := testsuite

# Source directory.
SOURCE_DIR := ../source

# Source files.
SOURCE_FILES := test.cpp \
                $(SOURCE_DIR)/driver/adc/atmega328p.cpp\
                $(SOURCE_DIR)/driver/eeprom/atmega328p.cpp\
                $(SOURCE_DIR)/driver/gpio/atmega328p.cpp\
                $(SOURCE_DIR)/driver/serial/atmega328p.cpp\
                $(SOURCE_DIR)/driver/tempsensor/tmp36.cpp\
                $(SOURCE_DIR)/driver/timer/atmega328p.cpp\
                $(SOURCE_DIR)/driver/watchdog/atmega328p.cpp\
                $(SOURCE_DIR)/logic/logic.cpp\
                $(SOURCE_DIR)/utils/utils.cpp\

# Include directory.
INCLUDE_DIR := ../include/

# C++ compiler.
CXX_COMPILER = g++

# C++ compiler flags.
COMPILER_FLAGS = -std=c++17 -Wall -I/usr/include -I$(INCLUDE_DIR)

# Linked libraries.
LINK_LIBS = -lgtest -lgtest_main -lpthread

# Build and run the test suite as default:
default: build run

# Build the test suite.
build: 
    @$(CXX_COMPILER) $(SOURCE_FILES) -o $(TARGET) $(COMPILER_FLAGS) $(LINK_LIBS)

# Run the test suite.
run:
    @./$(TARGET)

# Clean the test suite.
clean:
    @rm -f $(TARGET)
```