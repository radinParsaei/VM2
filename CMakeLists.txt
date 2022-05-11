cmake_minimum_required(VERSION 3.5)

project(VM LANGUAGES CXX C)

option(USE_GMP_LIB "" ON)
option(USE_DOUBLE "" OFF)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/CMake/Modules)

include_directories(.)
include_directories(Value)

add_definitions(-DSTATIC_BUILD)

if (${CMAKE_SYSTEM_NAME} MATCHES Darwin)
    add_link_options(-dead_strip -dead_strip_dylibs -Os)
endif ()
if (${CMAKE_SYSTEM_NAME} MATCHES Linux)
    add_link_options(-Wl,--gc-sections)
    add_compile_options(-fdata-sections -ffunction-sections)
endif ()

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-Ofast")

add_custom_command (
    OUTPUT static_call.cpp
    DEPENDS build_static_call_list.sh
    COMMAND bash -c "bash build_static_call_list.sh \"${BUNDLE_LIBRARIES}\">static_call.cpp"
)

SET(libsources "static_call.cpp")

foreach(X IN LISTS BUNDLE_LIBRARIES)
    if(EXISTS "libraries/${X}/bundle_CMakeLists.txt")
        include("libraries/${X}/bundle_CMakeLists.txt")
    endif()
endforeach()

message(STATUS ${libsources})

include_directories(libraries)

if (USE_DOUBLE)
    add_executable(VM ${libsources} main.cpp VM.cpp)
    add_executable(assemble assembler.cpp)
    add_executable(disassemble disassembler.cpp)
    add_definitions(-DUSE_DOUBLE)
else()
    if (USE_GMP_LIB)
        find_package(GMPXX REQUIRED)
        find_package(GMP REQUIRED)
        INCLUDE_DIRECTORIES("${GMPXX_INCLUDE_DIR}")
        link_libraries(${GMPXX_LIBRARIES})
        link_libraries(${GMP_LIBRARIES})
        add_executable(VM ${libsources} main.cpp VM.cpp)
        add_executable(assemble assembler.cpp)
        add_executable(disassemble disassembler.cpp)
    else()
        include_directories(BigNumber/src/BigNumber)
        add_executable(VM ${libsources} BigNumber/src/BigNumber/number.c BigNumber/src/BigNumber/BigNumber.cpp main.cpp VM.cpp)
        add_executable(assemble BigNumber/src/BigNumber/number.c BigNumber/src/BigNumber/BigNumber.cpp assembler.cpp)
        add_executable(disassemble BigNumber/src/BigNumber/number.c BigNumber/src/BigNumber/BigNumber.cpp disassembler.cpp)
    endif()
endif()
if (${CMAKE_SYSTEM_NAME} MATCHES Linux)
	target_link_libraries(VM dl)
endif()
