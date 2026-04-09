CXXFLAGS += -Wall -Wextra -std=c++14 -MMD

main: main.o filter.o equalizer.o wav_header.o
	$(CXX) $^ -o $@

clean:
	-@$(RM) *.d *.o
	@$(RM) main

run: main
	./main

-include main.d