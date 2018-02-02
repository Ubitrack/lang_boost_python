SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

SET(PROJECT_NAME "ubitrack_python")
PROJECT(${PROJECT_NAME})

cmake_minimum_required(VERSION 3.4.0)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(KEEP_RPATHS)

include(GNUInstallDirs)
include(cmake/UbitrackConfig)


# Add a cache variable PREFER_STATIC_LIBRARIES to have the choice of generating a project
# linking against static libraries if they exist. Default is NO since most people will not use this.
IF( NOT DEFINED PREFER_STATIC_LIBRARIES )
  SET( PREFER_STATIC_LIBRARIES "NO" CACHE BOOL "Decides if CMake should prefer static libraries to dynamic libraries when both exist." )
  MARK_AS_ADVANCED(PREFER_STATIC_LIBRARIES)
ENDIF( NOT DEFINED PREFER_STATIC_LIBRARIES )

IF( PREFER_STATIC_LIBRARIES )
  SET( CMAKE_FIND_LIBRARY_SUFFIXES .a;${CMAKE_FIND_LIBRARY_SUFFIXES} )
ENDIF( PREFER_STATIC_LIBRARIES )

SET( ${PROJECT_NAME}_COMPILE_FLAGS "-D${PROJECT_NAME}_EXPORTS" )
SET( ${PROJECT_NAME}_SOURCE_DIR ${${PROJECT_NAME}_SOURCE_DIR} )

SET(optionalLibs)
SET(requiredLibs ${CONAN_LIBS})

# Version of ${PROJECT_NAME}
SET( ${PROJECT_NAME}_MAJOR_VERSION 1 )
SET( ${PROJECT_NAME}_MINOR_VERSION 3 )
SET( ${PROJECT_NAME}_BUILD_VERSION 0 )

# OpenGL
SET(HAVE_OPENGL 0)
find_package(OpenGL)
IF(OPENGL_FOUND)
  add_definitions(-DHAVE_OPENGL)
  SET(HAVE_OPENGL 1)
  MESSAGE(STATUS "Found OpenGL: ${OPENGL_INCLUDE_DIR} - ${OPENGL_LIBRARIES}")
  INCLUDE_DIRECTORIES(${OPENGL_INCLUDE_DIR})
  SET(requiredLibs ${requiredLibs} ${OPENGL_LIBRARIES})
ENDIF(OPENGL_FOUND)

INCLUDE(cmake/PythonModuleBuilder.cmake)

INCLUDE_DIRECTORIES(${PYTHON_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${PYTHON_NUMPY_INCLUDE_DIR})
SET(requiredLibs ${requiredLibs} ${PYTHON_LIBRARY})

SET(MODULE_INSTALL_PATH ${PYTHON_PACKAGE_DIR})
SET( MODULE_LIBS ${requiredLibs} ${optionalLibs})



if (PYTHON_EXECUTABLE)
    set(SETUP_PY_IN "${CMAKE_CURRENT_SOURCE_DIR}/setup.py.in")
    set(SETUP_PY    "${CMAKE_CURRENT_BINARY_DIR}/setup.py")
    set(DEPS        "${CMAKE_CURRENT_SOURCE_DIR}/lib/ubitrack/__init__.py")
    set(OUTPUT      "${CMAKE_CURRENT_BINARY_DIR}/build")

    configure_file(${SETUP_PY_IN} ${SETUP_PY})

	add_custom_command(
	  OUTPUT ${OUTPUT}/timestamp
	  COMMAND ${PYTHON_EXECUTABLE} setup.py build
	  COMMAND ${CMAKE_COMMAND} -E touch ${OUTPUT}/timestamp
	  DEPENDS ${DEPS}
	)
	add_custom_target(pybuild ALL DEPENDS ${OUTPUT}/timestamp )

    install(CODE "execute_process(COMMAND ${PYTHON_EXECUTABLE} ${SETUP_PY} install)")
endif(PYTHON_EXECUTABLE)

INCLUDE_DIRECTORIES("${CMAKE_SOURCE_DIR}/include")

add_subdirectory(src)

enable_testing()
add_test( nosetests "${CMAKE_COMMAND}" -P ${CMAKE_BINARY_DIR}/testrunner.cmake )
