SOURCE_DIR = ../rig_cpp_key_allocation
CFLAGS += -fprofile-arcs -ftest-coverage -Wall -Werror -I$(SOURCE_DIR)
VPATH := $(SOURCE_DIR)

coverage : run_tests
	gcov -r constraints_graph test_constraints_graph

run_tests : tests
	valgrind --leak-check=full -q ./$<

tests : test_main.o test_constraints_graph.o constraints_graph.o
	$(CXX) $(CFLAGS) -o $@ $+ -lgtest

%.o : $(SOURCE_DIR)/%.cpp
	# We build the main source in C++98 for compatibility with older Python
	# versions on Windows
	$(CXX) $(CFLAGS) --std=c++98 -c $< -o $@

%.o : %.cpp
	$(CXX) $(CFLAGS) --std=c++11 -c $< -o $@

clean :
	$(RM) *.o *.gcov *.gcno *.gcda tests
