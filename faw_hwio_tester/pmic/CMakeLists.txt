project (pmic)

include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/include")
aux_source_directory (./src DIR_SRCS)
add_library (pmic ${DIR_SRCS})
target_link_libraries (pmic pthread)


add_executable (pmic_unit_test pmic_unit_test.c)
target_link_libraries (pmic_unit_test pmic pthread)
