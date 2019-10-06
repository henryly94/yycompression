add_library(
  heap
  heap.cc
)

add_library(
  huffman
  huffman.cc
)
target_link_libraries(
  huffman
  heap
)

add_executable(
  encoder_main
  encoder_main.cc
)
target_link_libraries(
  encoder_main
  huffman
)

add_executable(
  decoder_main
  decoder_main.cc
)
target_link_libraries(
  decoder_main
  huffman
)

install(
  TARGETS encoder_main decoder_main
  DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/.."
)
