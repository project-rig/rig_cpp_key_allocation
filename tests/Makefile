CFLAGS += -fprofile-arcs -ftest-coverage -Wall -Werror -I../rig_cpp_key_allocation --std=c++11
VPATH := ../rig_cpp_key_allocation

coverage : run_tests
	gcov -r constraints_graph bitset test_bitset test_constraints_graph

run_tests : tests
	valgrind --leak-check=full -q ./$<

tests : test_main.o test_bitset.o test_constraints_graph.o bitset.o constraints_graph.o
	$(CXX) $(CFLAGS) -o $@ $+ -lgtest

%.o : %.cpp
	$(CXX) $(CFLAGS) -c $< -o $@

clean :
	$(RM) *.o *.gcov *.gcno *.gcda tests
