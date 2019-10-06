project(yycompression)

set(CMAKE_BUILD_TYPE Debug)

add_executable(
  main
  main.cc
  huffman.h
  heap.h
)
