CXX := g++

CXXFLAGS := -std=c++17 -Wall -Wextra -Wpedantic -Iinclude -MMD -MP
LDFLAGS := -pthread

TARGET := minihttp
TEST_TARGET := build/socket_test

SRCS := $(wildcard src/*.cpp)
OBJS := $(patsubst src/%.cpp,build/%.o,$(SRCS))
DEPS := $(OBJS:.o=.d)

TEST_SRCS := $(wildcard tests/*_test.cpp)
TEST_OBJS := $(patsubst tests/%.cpp,build/%.o,$(TEST_SRCS))
TEST_DEPS := $(TEST_OBJS:.o=.d)

.PHONY: all clean run debug release test

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $@ $(LDFLAGS)

build/%.o: src/%.cpp
	mkdir -p build
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(TEST_TARGET): $(TEST_OBJS) build/socket.o
	$(CXX) $^ -o $@ $(LDFLAGS)

build/%_test.o: tests/%_test.cpp
	mkdir -p build
	$(CXX) $(CXXFLAGS) -c $< -o $@

-include $(DEPS) $(TEST_DEPS)

run: $(TARGET)
	./$(TARGET)

test: $(TEST_TARGET)
	./$(TEST_TARGET)

debug:
	$(MAKE) clean
	$(MAKE) CXXFLAGS="$(CXXFLAGS) -O0 -g -DDEBUG" all

release:
	$(MAKE) clean
	$(MAKE) CXXFLAGS="$(CXXFLAGS) -O2 -DNDEBUG" all

clean:
	rm -rf build $(TARGET)

