
cmake_minimum_required (VERSION 3.4)
project (LabCmd)
set(CMAKE_BUILD_TYPE Release)

set(LABCMD_ROOT ${CMAKE_CURRENT_SOURCE_DIR})
set(LOCAL_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../local)

# override cmake install prefix if it's not set,
# per: http://public.kitware.com/pipermail/cmake/2010-December/041135.html
IF(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    SET(CMAKE_INSTALL_PREFIX ${LOCAL_ROOT} CACHE PATH "Install set to local" FORCE)
ENDIF(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/defaults
                      ${CMAKE_SOURCE_DIR}/cmake/modules
                      ${CMAKE_SOURCE_DIR}/cmake/macros)

include(CXXDefaults)
include(Utilities)
include(Packages)

add_library(LabCmd SHARED "")

if (MSVC_IDE)
    # hack to get around the "Debug" and "Release" directories cmake tries to add on Windows
    #set_target_properties(LabRender PROPERTIES PREFIX "../")
    set_target_properties(LabCmd PROPERTIES IMPORT_PREFIX "../")
endif()

target_include_directories(LabCmd PRIVATE "${LABCMD_ROOT}/include")
target_include_directories(LabCmd PRIVATE "${LOCAL_ROOT}/include")
target_compile_definitions(LabCmd PRIVATE BUILDING_LABCMD=1)

IF(WIN32)
target_compile_definitions(LabCmd PUBLIC PLATFORM_WINDOWS=1)
elseif(APPLE)
target_compile_definitions(LabCmd PUBLIC PLATFORM_MACOS=1)
else()
target_compile_definitions(LabCmd PUBLIC PLATFORM_LINUX=1)
endif()

set_target_properties(LabCmd
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
#set_target_properties( targets LabCmd
#    PROPERTIES
#    ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/lib"
#    LIBRARY_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/lib"
#    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/bin"
#)

install (TARGETS LabCmd
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin)

source_file(src/LabCmd.h)
source_file(src/jsoncpp.cpp)
source_file(src/FFI.h)
source_file(src/FFI.cpp)
source_file(src/json/json-forwards.h)
source_file(src/json/json.h)
source_file(src/oscpkt/oscpkt.hh)
source_file(src/oscpkt/udp.hh)
source_file(src/webby/webby.c)
source_file(src/webby/webby.h)
source_file(src/delegate.hpp)
source_file(src/event.hpp)
source_file(src/signal.hpp)

if (WIN32)
    source_file(src/webby/webby_win32.h)
else()
    source_file(src/webby/webby_unix.h)
endif()

install(
    FILES
        src/LabCmd.h
        src/FFI.h
        src/delegate.hpp
        src/event.hpp
        src/signal.hpp
        src/property.h

    DESTINATION
        include/LabCmd)
