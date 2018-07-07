
include_guard()

cmake_minimum_required (VERSION 3.11)
project (LabCmd)

set(LABCMD_ROOT ${CMAKE_CURRENT_SOURCE_DIR})

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../LabCMake/scripts
                      ${CMAKE_CURRENT_SOURCE_DIR}/../LabCMake/modules)

include(Utilities)

if (WIN32)
    set(WEBBY_PLATFORM_HEADER webby_win32.h)
else()
    set(WEBBY_PLATFORM_HEADER webby_unix.h)
endif()    

lab_library(LabCmd
    TYPE SHARED

    ALIAS LabCmd::Core

    LIBRARIES

    INCLUDE_DIRS
        ${CMAKE_CURRENT_SOURCE_DIR}/include

    PUBLIC_HEADERS
        include/LabCmd/LabCmd.h
        include/LabCmd/FFI.h
        include/LabCmd/delegate.hpp
        include/LabCmd/event.hpp
        include/LabCmd/signal.hpp

    PRIVATE_HEADERS
        src/property.h
        src/json/json-forwards.h
        src/json/json.h
        src/oscpkt/oscpkt.hh
        src/oscpkt/udp.hh
        src/webby/webby.h
        src/webby/${WEBBY_PLATFORM_HEADER}

    CPPFILES
        src/webby/webby.c
        src/FFI.cpp
        src/jsoncpp.cpp
)
