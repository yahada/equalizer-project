CXX := g++
Q := @
IMGUI_DIR := imgui
SRC_DIR := src
BUILD_DIR := lib
TESTS_DIR := tests

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
	$(Q)mkdir -p $(BUILD_DIR)
	$(Q)$(CXX) $^ -o $(BUILD_DIR)/$@ $(LDLIBS)

$(BUILD_DIR)/%.o: %.cpp
	$(Q)printf "[CXX] %s\n" "$<"
	$(Q)mkdir -p $(dir $@)
	$(Q)$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

TEST_CORE_SRCS := \
	$(SRC_DIR)/equalizer.cpp \
	$(SRC_DIR)/wav_header.cpp \
	$(SRC_DIR)/filter.cpp

SRCS_TEST := $(TESTS_DIR)/test-functions.cpp

TEST_CORE_OBJS := $(addprefix $(BUILD_DIR)/, $(TEST_CORE_SRCS:.cpp=.test.o))
TEST_OBJS := $(addprefix $(BUILD_DIR)/, $(SRCS_TEST:.cpp=.test.o))
OBJS_TEST := $(TEST_CORE_OBJS) $(TEST_OBJS)

test: $(OBJS_TEST)
	$(Q)mkdir -p $(BUILD_DIR)
	$(Q)$(CXX) $^ -o $(BUILD_DIR)/test_runner
	$(Q)printf "\n[Running tests]\n"
	$(Q)./$(BUILD_DIR)/test_runner

$(BUILD_DIR)/%.test.o: %.cpp
	$(Q)printf "[TEST] %s\n" "$<"
	$(Q)mkdir -p $(dir $@)
	$(Q)$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

run: main
	$(Q)./$(BUILD_DIR)/main

clean:
	$(Q)rm -rf $(BUILD_DIR)
	$(Q)printf "[CLEAN]\n"

-include $(DEPS)