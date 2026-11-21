
CXX := g++

CXXFLAGS := -std=c++17 -Wall -Wextra -Wpedantic -Iinclude -MMD -MP
LDFLAGS := -pthread

TARGET := minihttp

SRCS := $(wildcard src/*.cpp)
OBJS := $(patsubst src/%.cpp,build/%.o,$(SRCS))
DEPS := $(OBJS:.o=.d)

.PHONY: all clean run debug release

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $@ $(LDFLAGS)

build/%.o: src/%.cpp
	mkdir -p build
	$(CXX) $(CXXFLAGS) -c $< -o $@

-include $(DEPS)

run: $(TARGET)
	./$(TARGET)

debug:
	$(MAKE) clean
	$(MAKE) CXXFLAGS="$(CXXFLAGS) -O0 -g -DDEBUG" all

release:
	$(MAKE) clean
	$(MAKE) CXXFLAGS="$(CXXFLAGS) -O2 -DNDEBUG" all

clean:
	rm -rf build $(TARGET)

