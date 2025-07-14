# Compilers
CC    := gcc
CXX   := g++
# Flags
CFLAGS   := -O3 -march=native
CXXFLAGS := -O3 -march=native

# Source lists
SRCS_C   := $(wildcard *.c)
SRCS_CPP := $(wildcard *.cpp)
# Executable names (strip .c and .cpp)
BINS     := $(SRCS_C:.c=) $(SRCS_CPP:.cpp=)

.PHONY: all clean

all: $(BINS)

# Rule for C++ sources
%: %.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

# Rule for C sources
%: %.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f $(BINS)
