add_executable(table_test table_test.c)
target_link_libraries(table_test common)

add_executable(vec_test vec_test.c)
target_link_libraries(vec_test common)

add_executable(heap_test heap_test.c)
target_link_libraries(heap_test common)

#add_executable(aes_test aes_test.c)
#target_link_libraries(aes_test common)

#add_executable(sha1_test sha1_test.c)
#hexdump test
#fbuftemp test
#etc...

#find_program(CTEST_MEMORYCHECK_COMMAND valgrind)
#set(CTEST_MEMORYCHECK_COMMAND_OPTIONS "--trace-children=yes --leak-check=full")

add_test(NAME table_test COMMAND table_test)
add_test(NAME vec_test COMMAND vec_test)
add_test(NAME heap_test COMMAND heap_test)

