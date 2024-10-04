# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -std=c++11 -Wall -Wextra -pedantic

# Include paths
INCLUDES = -I/usr/include

# Libraries
LIBS = -ljpeg -lpng

# Source files
SOURCES = silverbullet.cc image_tool.cc main.cc image8bit.cc

# Object files
OBJECTS = $(SOURCES:.cc=.o)

# Executable name
EXECUTABLE = silverbullet.out

# Default target
all: $(EXECUTABLE)

# Rule to link the program
$(EXECUTABLE): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(EXECUTABLE) $(LIBS)

# Rule to compile source files to object files
%.o: %.cc
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Clean target
clean:
	rm -f $(OBJECTS) $(EXECUTABLE)

# Phony targets
.PHONY: all clean

# Dependencies
silverbullet.o: silverbullet.cc silverbullet.h itool.h image_tool.h
image_tool.o: image_tool.cc image_tool.h itool.h
image8bit.o: image8bit.cc image_tool.h
main.o: main.cc silverbullet.h
