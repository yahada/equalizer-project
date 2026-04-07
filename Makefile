CXXFLAGS  += -Wall -Wextra -std=c++14 -MMD

main:
	$(CXX) *.cpp -o $@

clean:
	-@$(RM) main.d main.o
	@$(RM) main

-include main.d