#Compiler options:
PROGRAM := demo
CXX := g++
PA_PREFIX := /opt/homebrew/Cellar/portaudio/19.7.0
GLFW_PREFIX := /opt/homebrew/opt/glfw
IMGUI_DIR := ImPlot/imgui-master
IMPLOT_DIR := ImPlot/implot-master

CXXFLAGS := -O2 -ggdb -std=c++23 -MMD \
	-Iinclude \
	-I$(PA_PREFIX)/include \
	-I$(GLFW_PREFIX)/include \
	-I$(IMGUI_DIR) \
	-I$(IMGUI_DIR)/backends \
	-I$(IMPLOT_DIR)

LDFLAGS := -L$(PA_PREFIX)/lib -L$(GLFW_PREFIX)/lib


EXCLUDE.cpp :=
EXCLUDE.cpp += src/synth.cpp

SOURCES.cpp := $(filter-out $(EXCLUDE.cpp), $(wildcard src/*.cpp))
OBJS.cpp := $(SOURCES.cpp:%.cpp=%.o)
DEPS.cpp := $(SOURCES.cpp:%.cpp=%.d)

IMGUI_SOURCES := \
	$(IMGUI_DIR)/imgui.cpp \
	$(IMGUI_DIR)/imgui_demo.cpp \
	$(IMGUI_DIR)/imgui_draw.cpp \
	$(IMGUI_DIR)/imgui_tables.cpp \
	$(IMGUI_DIR)/imgui_widgets.cpp \
	$(IMGUI_DIR)/backends/imgui_impl_glfw.cpp \
	$(IMGUI_DIR)/backends/imgui_impl_opengl2.cpp \
	$(IMPLOT_DIR)/implot.cpp \
	$(IMPLOT_DIR)/implot_items.cpp

IMGUI_OBJS := $(IMGUI_SOURCES:.cpp=.o)

.PHONY: all
all: $(PROGRAM)

$(PROGRAM): $(OBJS.cpp) $(IMGUI_OBJS)
	$(CXX) $(LDFLAGS) $^ -lportaudio -lglfw -framework OpenGL -o $@

-include $(DEPS.cpp)

.PHONY: clean
clean:
	rm -f src/*.o src/*.d
	rm -f $(PROGRAM)
	rm -f $(IMGUI_OBJS)
	