CXX := g++

IMGUI_DIR := imgui
SRC_DIR := src
BUILD_DIR := lib

CXXFLAGS += -Wall -Wextra -std=c++17 -MMD
CPPFLAGS += -I. -I$(SRC_DIR) -I$(IMGUI_DIR) -I$(IMGUI_DIR)/backends
LDLIBS += -lglfw -lGL

SRCS := \
	$(SRC_DIR)/main.cpp \
	$(SRC_DIR)/filter.cpp \
	$(SRC_DIR)/equalizer.cpp \
	$(SRC_DIR)/wav_header.cpp \
	$(SRC_DIR)/cli.cpp \
	$(SRC_DIR)/equalizer_ui.cpp \
	$(IMGUI_DIR)/imgui.cpp \
	$(IMGUI_DIR)/imgui_draw.cpp \
	$(IMGUI_DIR)/imgui_widgets.cpp \
	$(IMGUI_DIR)/imgui_tables.cpp \
	$(IMGUI_DIR)/backends/imgui_impl_glfw.cpp \
	$(IMGUI_DIR)/backends/imgui_impl_opengl3.cpp

OBJS := $(SRCS:%.cpp=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

main: $(OBJS)
	mkdir -p $(BUILD_DIR)
	$(CXX) $^ -o $(BUILD_DIR)/$@ $(LDLIBS)

$(BUILD_DIR)/%.o: %.cpp
	mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

run: main
	./$(BUILD_DIR)/main

clean:
	rm -rf $(BUILD_DIR)

-include $(DEPS)