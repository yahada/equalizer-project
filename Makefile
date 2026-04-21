CXXFLAGS += -Wall -Wextra -std=c++14 -MMD

BUILD_DIR := lib
SRC_DIR := src
SRCS := main.cpp filter.cpp equalizer.cpp wav_header.cpp
OBJS := $($(SRC_DIR)/SRCS:%.cpp=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

main: $(OBJS)
	mkdir -p $(BUILD_DIR)
	$(CXX) $^ -o $(BUILD_DIR)/$@

$(BUILD_DIR)/%.o: %.cpp
	mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: main
	./$(BUILD_DIR)/main

clean:
	rm -rf $(BUILD_DIR)

-include $(DEPS)