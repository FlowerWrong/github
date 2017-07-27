cmake_minimum_required(VERSION 3.7)
project(github)

set(CMAKE_C_STANDARD 99)

include_directories(
        sds
)

set(SOURCE_FILES sds/sds.c sms.c)
add_executable(sms ${SOURCE_FILES})

FIND_PACKAGE(CURL)
IF(CURL_FOUND)
    INCLUDE_DIRECTORIES(${CURL_INCLUDE_DIR})
    TARGET_LINK_LIBRARIES(sms ${CURL_LIBRARY})
ELSE(CURL_FOUND)
    MESSAGE(FATAL_ERROR ”CURL library not found”)
ENDIF(CURL_FOUND)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules")

find_package(JSON-C)
include_directories(${JSON-C_INCLUDE_DIR})
target_link_libraries(sms ${JSON-C_LIBRARIES})