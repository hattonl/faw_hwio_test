project (uart)

include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/include")

include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/../menu/include")
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../menu)

aux_source_directory (./src DIR_SRCS)
add_library (uart ${DIR_SRCS})
target_link_libraries (uart pthread menu)


#add_executable (uart_unit_test uart_unit_test.c)
#target_link_libraries (uart_unit_test uart pthread)