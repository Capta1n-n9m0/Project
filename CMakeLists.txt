cmake_minimum_required(VERSION 3.19)
project(Project C)

set(CMAKE_C_STANDARD 11)

add_executable(Project main.c server.c server.h client.c client.h haiku.c haiku.h queue.c queue.h)
add_executable(Project_server server.c haiku.h haiku.c queue.c queue.h)
add_executable(Project_client client.c haiku.h haiku.c queue.c queue.h)
add_executable(Project_tests tests.c server.c server.h client.c client.h haiku.c haiku.h queue.c queue.h)

find_library(PTHREADS pthread)
find_library(CUNIT cunit)
target_link_libraries(Project LINK_PUBLIC ${PTHREAD})
target_link_libraries(Project_server LINK_PUBLIC ${PTHREAD})
target_link_libraries(Project_client LINK_PUBLIC ${PTHREAD})
target_link_libraries(Project_tests LINK_PUBLIC ${PTHREAD})
target_link_libraries(Project_tests LINK_PUBLIC ${CUNIT})
target_compile_definitions(Project_client PUBLIC STANDALONE)
target_compile_definitions(Project_server PUBLIC STANDALONE)