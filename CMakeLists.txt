cmake_minimum_required(VERSION 3.0)
project(coco)

include_directories(coco)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

enable_language(ASM)

add_library(coco STATIC
    coco/make_fcontext.S
    coco/jump_fcontext.S
    coco/context.cpp
    coco/fiber.cpp
)
target_link_libraries(coco pthread)

add_executable(test
    coco/test.cpp
)
target_link_libraries(test coco)