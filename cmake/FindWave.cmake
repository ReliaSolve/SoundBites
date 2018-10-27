# - Find Wave
# Find the Wave includes and library
# This module defines
#  WAVE_INCLUDE_DIR, where to find the header files
#  WAVE_LIBRARIES, the libraries needed to use aqt_Embed.
#  WAVE_FOUND, If false, do not try to use Wave.

if(WAVE_INCLUDE_DIR AND WAVE_LIBRARIES)
    set(WAVE_FOUND TRUE)

else(WAVE_INCLUDE_DIR AND WAVE_LIBRARIES)
    find_path(WAVE_INCLUDE_DIR wave/file.h
      ${CMAKE_INCLUDE_PATH}
      ${CMAKE_INSTALL_PREFIX}/include
      include
      /usr/include/
      /usr/local/aqueti/include/
      /usr/local/include/
      /usr/include/
      /usr/local/include/
      /opt/include/
      $ENV{ProgramFiles}/wave/include
      $ENV{SystemDrive}/wave/include
      )

if(WIN32)
    find_library(WAVE_LIBRARIES NAMES wave
      PATHS
      ${CMAKE_INSTALL_PREFIX}/lib
      ${CMAKE_LIB_PATH}
      $ENV{ProgramFiles}/wave/lib
      $ENV{SystemDrive}/wave/lib
      )
else(WIN32)
    find_library(WAVE_LIBRARIES NAMES wave
      PATHS
      ${CMAKE_INSTALL_PREFIX}/lib
      ${CMAKE_LIB_PATH}
      /usr/local/aqueti/lib
      /usr/lib
      /usr/local/lib
      /usr/local/lib/wave
      )
endif(WIN32)

  if(WAVE_INCLUDE_DIR AND WAVE_LIBRARIES)
	set(WAVE_FOUND TRUE)
	message(STATUS "Found aqt_Embed: ${WAVE_INCLUDE_DIR}, ${WAVE_LIBRARIES}")
  else(WAVE_INCLUDE_DIR AND WAVE_LIBRARIES)
	set(WAVE_FOUND FALSE)
	if (WAVE_FIND_REQUIRED)
       message(FATAL_ERROR "aqt_Embed not found.")
    else (WAVE_FIND_REQUIRED)
       message(STATUS "aqt_Embed not found.")
    endif (WAVE_FIND_REQUIRED)
  endif(WAVE_INCLUDE_DIR AND WAVE_LIBRARIES)

  mark_as_advanced(WAVE_INCLUDE_DIR WAVE_LIBRARIES)

endif(WAVE_INCLUDE_DIR AND WAVE_LIBRARIES)

