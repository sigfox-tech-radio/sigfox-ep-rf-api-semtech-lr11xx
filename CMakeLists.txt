cmake_minimum_required(VERSION 3.15)
project(lr11xx_rf_api)
include(ExternalProject)

#Check if Project configure by himself or by a parent project (like sigfox-ep-lib)
if ( ${CMAKE_SOURCE_DIR} STREQUAL ${PROJECT_SOURCE_DIR} )
    if (NOT DEFINED  SIGFOX_EP_LIB_DIR)
        message(FATAL_ERROR "${PROJECT_NAME} cannot be configured without -DSIGFOX_EP_LIB_DIR=<sigfox-ep-lib directory>")
    endif()
    add_subdirectory(${SIGFOX_EP_LIB_DIR} "${CMAKE_CURRENT_BINARY_DIR}/${SIGFOX_EP_LIB_DIR}")
 else()
    set(SIGFOX_EP_LIB_DIR ${CMAKE_SOURCE_DIR})
    message("${PROJECT_NAME} is Fetched by a parent project")
endif()

set (LR11XX_RF_API_SOURCES
    src/manuf/lr11xx_rf_api.c
    src/manuf/smtc_dbpsk.c
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
        GIT_TAG           v2.4.1
    )
else ()
    list(APPEND CHIP_RF_HEADERS_LIST "$<BUILD_INTERFACE:$<TARGET_PROPERTY:lr11xx_driver,INTERFACE_INCLUDE_DIRECTORIES>>")
endif()

#Add Cmake module path
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

#Precompile module
include(precompile_lr11xx_rf_api)

add_library(${PROJECT_NAME}_obj OBJECT EXCLUDE_FROM_ALL src/manuf/lr11xx_rf_api.c  src/manuf/smtc_dbpsk.c)
if (NOT TARGET lr11xx_driver)
add_dependencies(${PROJECT_NAME}_obj lr11xx)
endif()
target_include_directories( ${PROJECT_NAME}_obj PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
    ${SIGFOX_EP_LIB_DIR}/inc
    ${CHIP_RF_HEADERS_LIST}
)
target_compile_definitions(${PROJECT_NAME}_obj PUBLIC ${DEF_FLAG_LIST})

add_library(${PROJECT_NAME} STATIC ${LR11XX_RF_API_SOURCES})
add_dependencies(${PROJECT_NAME}  precompil_${PROJECT_NAME} lr11xx)
target_include_directories(${PROJECT_NAME} PUBLIC ${PRECOMPIL_DIR}/inc  ${CHIP_RF_HEADERS_LIST})
target_compile_definitions(${PROJECT_NAME} PUBLIC ${DEF_FLAG_LIST})
target_compile_options(${PROJECT_NAME} PRIVATE -Wall)
#set_target_properties(${PROJECT_NAME} PROPERTIES PUBLIC_HEADER "${PRECOMPIL_ADDON_RFP_PUBLIC_HEADERS}")
set_target_properties(${PROJECT_NAME}
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${LIB_LOCATION}
    LIBRARY_OUTPUT_DIRECTORY ${LIB_LOCATION}
)
install(TARGETS ${PROJECT_NAME}
    ARCHIVE DESTINATION ${LIB_LOCATION}
    PUBLIC_HEADER DESTINATION ${API_LOCATION}  
)
