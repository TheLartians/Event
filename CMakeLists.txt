cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

# ---- Project ----

project(LarsEvent 
  VERSION 2.1
  LANGUAGES CXX
)

# ---- Configuration variables ----

option(LARS_EVENT_ENABLE_TESTS "Enable tests" OFF)
option(LARS_EVENT_BUILD_EXAMPLES "Enable examples" OFF)

# ---- Include guards ----

if(TARGET LarsEvent)
  return()
endif()

if(${PROJECT_SOURCE_DIR} STREQUAL ${PROJECT_BINARY_DIR})
    message(FATAL_ERROR "In-source builds not allowed. Please make a new directory (called a build directory) and run CMake from there.")
endif()

# ---- Header target ----

FILE(GLOB headers "${CMAKE_CURRENT_SOURCE_DIR}/include/lars/*.h")
add_library(LarsEvent-headers EXCLUDE_FROM_ALL ${headers})
SET_TARGET_PROPERTIES(LarsEvent-headers PROPERTIES LINKER_LANGUAGE CXX)

# ---- Create library ----

add_library(LarsEvent INTERFACE)

target_include_directories(LarsEvent
  INTERFACE 
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

include(CMakePackageConfigHelpers)

write_basic_package_version_file(
  "${PROJECT_BINARY_DIR}/LarsEventConfigVersion.cmake"
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY AnyNewerVersion
)

# ---- Install ----

install(
  TARGETS LarsEvent
  EXPORT LarsEventTargets
  LIBRARY DESTINATION lib COMPONENT Runtime
  ARCHIVE DESTINATION lib COMPONENT Development
  RUNTIME DESTINATION bin COMPONENT Runtime
  PUBLIC_HEADER DESTINATION include COMPONENT Development
  BUNDLE DESTINATION bin COMPONENT Runtime
)

include(CMakePackageConfigHelpers)

configure_package_config_file(
  "${PROJECT_SOURCE_DIR}/cmake/LarsEventConfig.cmake.in"
  "${PROJECT_BINARY_DIR}/LarsEventConfig.cmake"
  INSTALL_DESTINATION lib/cmake/LarsEvent
)

install(
  EXPORT LarsEventTargets 
  DESTINATION lib/cmake/LarsEvent
)

install(
  FILES 
    "${PROJECT_BINARY_DIR}/LarsEventConfigVersion.cmake"
    "${PROJECT_BINARY_DIR}/LarsEventConfig.cmake"
  DESTINATION 
    lib/cmake/LarsEvent
)

install(
  DIRECTORY ${PROJECT_SOURCE_DIR}/include/
  DESTINATION include
)

# ---- Examples ----

IF(${LARS_EVENT_BUILD_EXAMPLES})
  add_subdirectory(examples)    
ENDIF()

# ---- Test ----

if(${LARS_EVENT_ENABLE_TESTS})
  ENABLE_TESTING()  
  add_subdirectory(tests)  
endif()

