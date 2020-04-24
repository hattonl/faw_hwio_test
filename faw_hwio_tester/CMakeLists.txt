project (server)

###################### sub dir #######################
add_subdirectory (${CMAKE_CURRENT_SOURCE_DIR}/menu)
add_subdirectory (${CMAKE_CURRENT_SOURCE_DIR}/network)
add_subdirectory (${CMAKE_CURRENT_SOURCE_DIR}/camera)
add_subdirectory (${CMAKE_CURRENT_SOURCE_DIR}/can)
add_subdirectory (${CMAKE_CURRENT_SOURCE_DIR}/gpio)
add_subdirectory (${CMAKE_CURRENT_SOURCE_DIR}/pmic)
add_subdirectory (${CMAKE_CURRENT_SOURCE_DIR}/uart)

###################### include dir #######################
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/menu/include)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/network/include)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/camera/include)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/can/include)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/gpio/include)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/pmic/include)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/uart/include)

###################### lib #######################
#set (LIB_LIST pthread network)
#set (LIB_LIST pthread camera)
#set (LIB_LIST pthread can)
#set (LIB_LIST pthread gpio)
set (LIB_LIST pthread network camera can gpio pmic menu uart)

###################### main #######################
add_executable (faw_hwio_test faw_hwio_test.c)
target_link_libraries (faw_hwio_test ${LIB_LIST})
