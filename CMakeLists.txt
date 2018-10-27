cmake_minimum_required(VERSION 3.1.0)
project(SoundBites)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Win-specific: Ignore warnings about sprintf() in this case.
if(WIN32)
  add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

#-----------------------------------------------------------------------------
# Local CMake Modules
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

#-----------------------------------------------------------------------------
# Enable sorting projects within the solution on Visual Studio, to group
# Test and Example files together.
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

#-----------------------------------------------------------------------------
# Checking for optional and required packages

find_package(Wave REQUIRED)

#-----------------------------------------------------------------------------
# Build options.

include_directories(${WAVE_INCLUDE_DIR})

#-----------------------------------------------------------------------------
# Build the executable.

set (SOUNDBITES_SOURCES SoundBites.cpp)
set (SOUNDBITES_HEADERS
)

add_executable(SoundBites ${SOUNDBITES_SOURCES} ${SOUNDBITES_HEADERS})
target_link_libraries(SoundBites PUBLIC ${WAVE_LIBRARIES})

install(TARGETS SoundBites EXPORT ${PROJECT_NAME}
  RUNTIME DESTINATION bin COMPONENT lib
  LIBRARY DESTINATION lib${LIB_SUFFIX} COMPONENT lib
  ARCHIVE DESTINATION lib${LIB_SUFFIX} COMPONENT lib
  INCLUDES DESTINATION include
  PUBLIC_HEADER DESTINATION include
)

