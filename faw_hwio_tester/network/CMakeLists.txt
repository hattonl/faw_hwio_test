project (network)

include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/include")

include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/../menu/include")
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../menu)

#link_libraries(menu)

aux_source_directory (./src DIR_SRCS)
add_library (network ${DIR_SRCS})
target_link_libraries (network pthread menu)


#add_executable (network_server_test network_server_test.c)
#target_link_libraries (network_server_test network pthread)
