cmake_minimum_required(VERSION 3.19)
project(Project C)

set(CMAKE_C_STANDARD 11)

add_executable(Project main.c server.c server.h client.c client.h haiku.c haiku.h queue.c queue.h)
add_executable(Project_server_v1 server.c haiku.h haiku.c queue.c queue.h)
add_executable(Project_client_v1 client.c haiku.h haiku.c queue.c queue.h)

find_library(PTHREADS pthread)
find_library(CUNIT cunit)
target_link_libraries(Project LINK_PUBLIC ${PTHREAD})
target_link_libraries(Project LINK_PUBLIC ${CUNIT})