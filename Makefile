#Compiler options:
PROGRAM := demo
CXX := g++
PA_PREFIX := /opt/homebrew/Cellar/portaudio/19.7.0

CXXFLAGS := -O2 -ggdb -Wall -Wextra -std=c++23 -MMD \
	-Iinclude \
	-I$(PA_PREFIX)/include \

LDFLAGS := -L$(PA_PREFIX)/lib 

SOURCES.cpp := $(wildcard src/*.cpp)
OBJS.cpp := $(SOURCES.cpp:%.cpp=%.o)
DEPS.cpp := $(SOURCES.cpp:%.cpp=%.d)

.PHONY: all
all: $(PROGRAM)

$(PROGRAM): $(OBJS.cpp) $(IMGUI_OBJS)
	$(CXX) $(LDFLAGS) $^ -lportaudio -o $@

-include $(DEPS.cpp)

.PHONY: clean
clean:
	rm -f src/*.o src/*.d
	rm -f $(PROGRAM)
	