
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
        ${CMAKE_CURRENT_SOURCE_DIR}/src

    PUBLIC_HEADERS
        src/LabCmd.h
        src/FFI.h

    PRIVATE_HEADERS
        src/delegate.hpp
        src/event.hpp
        src/property.h
        src/signal.hpp
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
