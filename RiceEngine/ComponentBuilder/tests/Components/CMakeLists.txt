cmake_minimum_required(VERSION 3.19)

project("compcompiler")

set(RICE_GL_NAME ${PROJECT_NAME} CACHE INTERNAL "")
set(RICE_GL_INCLUDE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/include CACHE INTERNAL "")

set(CMAKE_CXX_STANDARD 20)

MACRO(SUBDIRLIST result curdir)
  FILE(GLOB children RELATIVE ${curdir} ${curdir}/*)
  SET(dirlist "")
  FOREACH(child ${children})
    IF(IS_DIRECTORY ${curdir}/${child})
      LIST(APPEND dirlist ${child})
    ENDIF()
  ENDFOREACH()
  SET(${result} ${dirlist})
ENDMACRO()

#file search
SUBDIRLIST(TESTS ${CMAKE_CURRENT_SOURCE_DIR})

foreach(TEST ${TESTS})
    add_subdirectory(${TEST})
endforeach(TEST ${TESTS})
