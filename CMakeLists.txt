################################################################################
#
# Copyright (c) 2024, UnaBiz SAS
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
#  1 Redistributions of source code must retain the above copyright notice,
#    this list of conditions and the following disclaimer.
#  2 Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
#  3 Neither the name of UnaBiz SAS nor the names of its contributors may be
#    used to endorse or promote products derived from this software without
#    specific prior written permission.
#
# NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE GRANTED BY
# THIS LICENSE. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
# CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
# PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
# CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
# EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
# PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
# BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
# IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
#
################################################################################

cmake_minimum_required(VERSION 3.15)
project(lr11xx_rf_api)
include(ExternalProject)

#Check if Project configure by himself or by a parent project (like sigfox-ep-lib)
if ( ${CMAKE_SOURCE_DIR} STREQUAL ${PROJECT_SOURCE_DIR} )
    if (NOT DEFINED  SIGFOX_EP_LIB_DIR)
        message(FATAL_ERROR "${PROJECT_NAME} cannot be configured without -DSIGFOX_EP_LIB_DIR=<sigfox-ep-lib directory>")
    endif()
    add_subdirectory(${SIGFOX_EP_LIB_DIR} "${CMAKE_CURRENT_BINARY_DIR}/sigfox-ep-lib")
 else()
    if (NOT DEFINED SIGFOX_EP_LIB_DIR)
        set(SIGFOX_EP_LIB_DIR ${CMAKE_SOURCE_DIR})
        message("${PROJECT_NAME} is Fetched by a parent project")
    endif()
endif()

set (LR11XX_RF_API_SOURCES
    src/manuf/lr11xx_rf_api.c
    src/manuf/smtc_dbpsk.c
    src/board/lr11xx_hal.c
    src/board/lr11xx_hw_api.c
)

set (LR11XX_RF_API_HEADERS
    inc/manuf/lr11xx_rf_api.h
    inc/manuf/smtc_dbpsk.h
    inc/board/lr11xx_hw_api.h
)

set( CHIP_RF_HEADERS_LIST "")

if (NOT TARGET lr11xx_driver)
    list(APPEND CHIP_RF_HEADERS_LIST  ${CMAKE_CURRENT_BINARY_DIR}/drivers-src/lr11xx/src/)
    ExternalProject_Add(lr11xx
        PREFIX drivers-build
        CONFIGURE_COMMAND ""
        BUILD_COMMAND ""
        INSTALL_COMMAND ""
        UPDATE_COMMAND 
            COMMAND bash -c "git -C ${CMAKE_CURRENT_BINARY_DIR}/drivers-src/lr11xx/ reset --hard"  
        SOURCE_DIR drivers-src/lr11xx
        GIT_PROGRESS TRUE
        GIT_SHALLOW TRUE
        GIT_REPOSITORY    https://github.com/Lora-net/SWDR001.git
        GIT_TAG           v3.0.0
    )
else ()
    list(APPEND CHIP_RF_HEADERS_LIST "$<BUILD_INTERFACE:$<TARGET_PROPERTY:lr11xx_driver,INTERFACE_INCLUDE_DIRECTORIES>>")
endif()

#Add Cmake module path
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

#Precompile module
find_program(UNIFDEF unifdef)
if(NOT UNIFDEF)
    message(STATUS "unifdef - not found (precompilation target will not be available)")   
else()
    include(precompile_lr11xx_rf_api)
endif()

#Target to create object
add_library(${PROJECT_NAME}_obj OBJECT EXCLUDE_FROM_ALL ${LR11XX_RF_API_SOURCES})
if (NOT TARGET lr11xx_driver)
    add_dependencies(${PROJECT_NAME}_obj lr11xx)
endif()
target_include_directories( ${PROJECT_NAME}_obj PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
    ${CHIP_RF_HEADERS_LIST}
    $<BUILD_INTERFACE:$<TARGET_PROPERTY:sigfox_ep_lib_obj,INTERFACE_INCLUDE_DIRECTORIES>>
)

#Target to create library
add_library(${PROJECT_NAME} STATIC $<TARGET_OBJECTS:${PROJECT_NAME}_obj>)
if (NOT TARGET lr11xx_driver)
    add_dependencies(${PROJECT_NAME} lr11xx)
endif()
target_include_directories(${PROJECT_NAME} PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
    ${CHIP_RF_HEADERS_LIST}
    $<BUILD_INTERFACE:$<TARGET_PROPERTY:sigfox_ep_lib_obj,INTERFACE_INCLUDE_DIRECTORIES>>
)
set_target_properties(${PROJECT_NAME}
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${LIB_LOCATION}
    LIBRARY_OUTPUT_DIRECTORY ${LIB_LOCATION}
)
install(TARGETS ${PROJECT_NAME}
    ARCHIVE DESTINATION ${LIB_LOCATION}
    PUBLIC_HEADER DESTINATION ${API_LOCATION}  
)
