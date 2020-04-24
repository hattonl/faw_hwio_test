project (gpio)

include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/include")
aux_source_directory (./src DIR_SRCS)
add_library (gpio ${DIR_SRCS})
target_link_libraries (gpio pthread)


# add_executable (gpio_unit_test gpio_unit_test.c)
# target_link_libraries (gpio_unit_test gpio pthread)
