
include_guard()

cmake_minimum_required (VERSION 3.11)
project (LabCmd)

set(LABCMD_ROOT ${CMAKE_CURRENT_SOURCE_DIR})

if (WIN32)
    set(WEBBY_PLATFORM_HEADER webby_win32.h)
else()
    set(WEBBY_PLATFORM_HEADER webby_unix.h)
endif()    

set(LABCMD_PUBLIC_HEADERS
    include/LabCmd/LabCmd.h
    include/LabCmd/FFI.h
    include/LabCmd/delegate.hpp
    include/LabCmd/event.hpp
    include/LabCmd/signal.hpp
    include/LabCmd/Queue.h)

set(LABCMD_PRIVATE_HEADERS
    src/property.h
    src/json/json-forwards.h
    src/json/json.h
    src/oscpkt/oscpkt.hh
    src/oscpkt/udp.hh
    src/webby/webby.h
    src/webby/${WEBBY_PLATFORM_HEADER})

set(LABCMD_SRC
    src/webby/webby.c
    src/FFI.cpp
    src/jsoncpp.cpp)

add_library(LabCmd STATIC
    ${LABCMD_PUBLIC_HEADERS} ${LABCMD_PRIVATE_HEADERS} ${LABCMD_SRC})
target_include_directories(LabCmd PUBLIC ${LABCMD_ROOT}/include)
set_target_properties(LabCmd PROPERTIES
    FOLDER "LabCmd"
    POSITION_INDEPENDENT_CODE ON
    PUBLIC_HEADER "${LABCMD_PUBLIC_HEADERS}"
    OUTPUT_NAME "LabCmd"
    OUTPUT_NAME_DEBUG "LabCmd_d"
    INTERFACE_INCLUDE_DIRECTORIES "${LABCMD_ROOT}/include")

add_library(Lab::Cmd ALIAS LabCmd)
configure_file(LabCmdConfig.cmake.in "${PROJECT_BINARY_DIR}/LabCmdConfig.cmake" @ONLY)

install(FILES
    "${PROJECT_BINARY_DIR}/LabCmdConfig.cmake"
    DESTINATION "${CMAKE_INSTALL_PREFIX}/cmake")

