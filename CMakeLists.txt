cmake_minimum_required(VERSION 3.16)
project(socket LANGUAGES CXX)

if("${PROJECT_SOURCE_DIR}" STREQUAL "${PROJECT_BINARY_DIR}")
    message(SEND_ERROR "In-source builds are not allowed")
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

# Add all .cpp, - files under src/ directory
file(GLOB SOURCES_CPP "src/*.hpp" "src/*.cpp")


set(SOURCES ${SOURCES_CPP} )
message(STATUS "SRC directory source code files: \n\t-- HEADERS: ${SOURCES_H}${SOURCES_HPP}\n\t-- C SOURCES: ${SOURCES_C}\n\t-- CPP SOURCE:${SOURCES_CPP}\n\t-- ALL:${SOURCES}")
# add the executable target
add_executable(socket)

target_sources(socket PUBLIC ${SOURCES})
target_include_directories(socket PUBLIC ext)
target_include_directories(socket PUBLIC src)

# add_executable(CMakeSFMLProject src/main.cpp)x
target_compile_features(socket PRIVATE cxx_std_17)

# if(WIN32)
#     add_custom_command(
#         TARGET socket
#         COMMENT "Copy OpenAL DLL"
#         PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${SFML_SOURCE_DIR}/extlibs/bin/$<IF:$<EQUAL:${CMAKE_SIZEOF_VOID_P},8>,x64,x86>/openal32.dll $<TARGET_FILE_DIR:socket>
#         VERBATIM)
# endif()

install(TARGETS socket)
