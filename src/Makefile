# Compiler to use
CXX = g++

# Compiler flags
CXXFLAGS = -Wall -g -std=c++11

# Targets (executables)
TARGETS = noforward forward

# Default target: build both executables
all: $(TARGETS)

# Rule to build noforward
noforward: main.cpp processor.hpp noforwarding.hpp
	$(CXX) $(CXXFLAGS) main.cpp -o noforward

# Rule to build forward
forward: main_forwarding.cpp processor_forwarding.hpp forwarding.hpp
	$(CXX) $(CXXFLAGS) main_forwarding.cpp -o forward

# Clean up
clean:
	rm -f $(TARGETS)

# Phony targets
.PHONY: all clean