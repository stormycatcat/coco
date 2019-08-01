cmake_minimum_required(VERSION 3.0)
project(coco)

include_directories(coco)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

enable_language(ASM)

add_library(coco_base STATIC
    coco/make_fcontext.S
    coco/jump_fcontext.S
    coco/event/dispatcher.cpp
    coco/scheduler.cpp
)
target_link_libraries(coco_base pthread)

add_library(coco_hook STATIC
    coco/hook/fdhook.cpp
    coco/hook/utility.cpp
    coco/hook/init.cpp
    coco/hook/sys/socket.cpp
    coco/hook/sys/accept.cpp
    coco/hook/sys/connect.cpp
    coco/hook/sys/read.cpp
    coco/hook/sys/write.cpp
    coco/hook/sys/recv.cpp
    coco/hook/sys/send.cpp
)
target_link_libraries(coco_hook dl coco_base)

add_executable(test
    coco/test.cpp
)
target_link_libraries(test coco_base coco_hook)
