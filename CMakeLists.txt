cmake_minimum_required(VERSION 3.31)
set(PNAME "vmem-cache-sim")
project(${PNAME} LANGUAGES C)

file(GLOB_RECURSE prog_sources CONFIGURE_DEPENDS "src/*.c" "src/*.h")
add_executable(vmem-cache-sim ${prog_sources})

find_library(MATH_LIBRARY m)
if(MATH_LIBRARY)
    target_link_libraries(${PNAME} PUBLIC ${MATH_LIBRARY})
endif()
set_target_properties(${PNAME}
    PROPERTIES
    C_STANDARD 99
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
