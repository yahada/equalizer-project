IMGUI_DIR := ../../imgui-project/imgui
SRC_DIR := src

CXXFLAGS += -Wall -Wextra -std=c++17
CPPFLAGS += -I. -I$(SRC_DIR) -I$(IMGUI_DIR) -I$(IMGUI_DIR)/backends
LDLIBS += -lglfw -lGL

SOURCES := \
	$(SRC_DIR)/main.cpp \
	equalizer_ui.cpp \
	$(SRC_DIR)/filter.cpp \
	$(SRC_DIR)/equalizer.cpp \
	$(SRC_DIR)/wav_header.cpp \
	$(IMGUI_DIR)/imgui.cpp \
	$(IMGUI_DIR)/imgui_draw.cpp \
	$(IMGUI_DIR)/imgui_widgets.cpp \
	$(IMGUI_DIR)/imgui_tables.cpp \
	$(IMGUI_DIR)/backends/imgui_impl_glfw.cpp \
	$(IMGUI_DIR)/backends/imgui_impl_opengl3.cpp

main: $(SOURCES)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $^ -o $@ $(LDLIBS)

run: main
	./main

clean:
	@$(RM) main
