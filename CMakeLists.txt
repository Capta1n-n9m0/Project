cmake_minimum_required(VERSION 3.19)
project(Project C)

set(CMAKE_C_STANDARD 11)

add_executable(Project main.c server.c server.h client.c client.h haiku.c haiku.h queue.c queue.h)
add_executable(Project_V1 V1/server.c V1/client.c V1/haiku.c V1/haiku.h)

find_library(PTHREADS pthread)
find_library(CUNIT cunit)
target_link_libraries(Project LINK_PUBLIC ${PTHREAD})
target_link_libraries(Project LINK_PUBLIC ${CUNIT})
target_link_libraries(Project_V1 LINK_PUBLIC ${PTHREAD})
target_link_libraries(Project_V1 LINK_PUBLIC ${CUNIT})