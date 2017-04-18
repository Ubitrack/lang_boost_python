cmake_minimum_required(VERSION 2.8)

SET(PROJECT_NAME "ubitrack_python")

PROJECT(${PROJECT_NAME})

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
SET(requiredLibs)

# Version of ${PROJECT_NAME}
SET( ${PROJECT_NAME}_MAJOR_VERSION 1 )
SET( ${PROJECT_NAME}_MINOR_VERSION 3 )
SET( ${PROJECT_NAME}_BUILD_VERSION 0 )

include(cmake/UbiTrackDetectCXXCompiler.cmake)



if(APPLE)
  set(CMAKE_MACOSX_RPATH TRUE)
# The following settings were copied from
# https://cmake.org/Wiki/CMake_RPATH_handling
# to avoid the rpath issue that appears on OS X El Capitan

  # use, i.e. don't skip the full RPATH for the build tree
  set(CMAKE_SKIP_BUILD_RPATH  FALSE)

  # when building, don't use the install RPATH already
  # (but later on when installing)
  set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE) # Changed to TRUE by A.O. 

  set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")

  # add the automatically determined parts of the RPATH
  # which point to directories outside the build tree to the install RPATH
  set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
endif()

# enable g++ C++0x extensions
#IF(CMAKE_COMPILER_IS_GNUCXX)
#	SET(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-std=gnu++0x")
#ENDIF(CMAKE_COMPILER_IS_GNUCXX)

# Needed to convert from \ to / on windows. Not needed in the FindModules since
# call to macro takes care of that there.
SET( CONVERTED_UBITRACK_PATH "" )
IF( EXISTS $ENV{UBITRACK_PATH} )
  FILE( TO_CMAKE_PATH $ENV{UBITRACK_PATH} CONVERTED_UBITRACK_PATH )
ENDIF( EXISTS $ENV{UBITRACK_PATH} )
SET(CMAKE_MODULE_PATH  ${${PROJECT_NAME}_SOURCE_DIR}/cmake ${CONVERTED_UBITRACK_PATH}/..share/UbiTrack/cmake)

MACRO(getenv_path VAR)
  SET(ENV_${VAR} $ENV{${VAR}})
  # replace won't work if var is blank
  IF(ENV_${VAR})
    FILE( TO_CMAKE_PATH ENV_${VAR} ${ENV_${VAR}} )
  ENDIF ()
ENDMACRO(getenv_path)

IF(X86_64)
  getenv_path(UBITRACKLIB_EXTERNAL64)
  IF(ENV_UBITRACKLIB_EXTERNAL64)
    SET(EXTERNAL_LIBRARIES_DIR "${ENV_UBITRACKLIB_EXTERNAL64}")
  ENDIF(ENV_UBITRACKLIB_EXTERNAL64)
ELSE()
  getenv_path(UBITRACKLIB_EXTERNAL32)
  IF(ENV_UBITRACKLIB_EXTERNAL32)
    SET(EXTERNAL_LIBRARIES_DIR "${ENV_UBITRACKLIB_EXTERNAL32}")
  ENDIF(ENV_UBITRACKLIB_EXTERNAL32)
ENDIF(X86_64)

MESSAGE(STATUS "Ubitrack Externals Dir: ${EXTERNAL_LIBRARIES_DIR}")

# UbiTrack  
FIND_PACKAGE(Ubitrack REQUIRED)

IF(UBITRACK_FOUND)
  SET(HAVE_UBITRACK 1)
  INCLUDE_DIRECTORIES( ${UBITRACK_INCLUDE_DIR} )
  # currently needed because of relative include :(
  INCLUDE_DIRECTORIES( "${UBITRACK_INCLUDE_DIR}/utFacade" )
  SET(requiredLibs ${requiredLibs} ${UBITRACK_LIBRARIES} )
ENDIF(UBITRACK_FOUND)

IF(WIN32)
  set(${PROJECT_NAME}_COMPILE_FLAGS "${${PROJECT_NAME}_COMPILE_FLAGS} /EHsc /c /W3 /GR /wd4355 /wd4996 /wd4251 /wd4275 /wd4819 /wd4290")
  set(${PROJECT_NAME}_LINK_FLAGS "/SUBSYSTEM:CONSOLE")
  set(${PROJECT_NAME}_DEFINES "WIN32" "_MBCS" "_WIN32_WINNT=0x501")	
ENDIF(WIN32) 



if(APPLE)
  execute_process(COMMAND ${CMAKE_CXX_COMPILER} -v
    ERROR_VARIABLE _clangcxx_dumpedversion
    ERROR_STRIP_TRAILING_WHITESPACE
    )
  string(REGEX REPLACE ".*clang version ([0-9]\\.[0-9]+).*" "\\1" _clangcxx_version ${_clangcxx_dumpedversion})
  # Apple Clang 4.2 no longer reports clang version but LLVM version
  # Moreover, this is Apple versioning, not LLVM upstream
  # If this is the case, the previous regex will not do anything.
  # Check to see if we have "Apple LLVM version" in the output,
  # and if so extract the original LLVM version which should appear as
  # "based on LLVM X.Ysvn"
  if(APPLE AND "${_clangcxx_version}" MATCHES ".*Apple LLVM version.*")
    string(REGEX REPLACE ".*based on LLVM ([0-9]\\.[0-9]+)svn.*" "\\1" _clangcxx_version ${_clangcxx_version})
    set(APPLE_CLANG_VERSION ${_clangcxx_version})
    message(STATUS "Apple Clang version: ${_clangcxx_version}")
  endif()
endif(APPLE)


# Find Boost library. Required to compile.
IF(WIN32 AND DEFINED EXTERNAL_LIBRARIES_DIR)
	set(BOOST_ROOT_DIR "${EXTERNAL_LIBRARIES_DIR}/boost")
	INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR} )
ENDIF(WIN32 AND DEFINED EXTERNAL_LIBRARIES_DIR)


#Boost bindings
IF(NOT WIN32)
  MESSAGE(STATUS "set boost::ublas alignment to 16")
  add_definitions("-DBOOST_UBLAS_BOUNDED_ARRAY_ALIGN=__attribute__ ((aligned (16)))")
ENDIF(NOT WIN32)
ADD_DEFINITIONS(-DBOOST_SPIRIT_USE_OLD_NAMESPACE)

IF(MSVC)
  # force dynamic linking of boost libs on windows ..
  ADD_DEFINITIONS(-DBOOST_ALL_DYN_LINK)
ENDIF(MSVC)

SET(HAVE_BOOST 0)
if(MSVC)
  # force dynamic linking of boost libs on windows ..
  add_definitions(-DBOOST_ALL_DYN_LINK)
endif(MSVC)
set(Boost_USE_STATIC_LIBS   OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package( Boost 1.49 COMPONENTS thread date_time system filesystem regex chrono locale serialization program_options python numpy REQUIRED)
if(Boost_FOUND)
  add_definitions("-DBOOST_FILESYSTEM_VERSION=3")
  add_definitions(-DHAVE_BOOST)
  link_directories(${Boost_LIBRARY_DIRS})
  SET(requiredLibs ${requiredLibs} ${Boost_LIBRARIES} )
  SET(HAVE_BOOST 1)
endif(Boost_FOUND)

if (Boost_VERSION VERSION_LESS "1.63.0")
find_package(BoostNumpy REQUIRED)
IF(BOOST_NUMPY_FOUND)
  add_definitions(-DHAVE_BOOST_NUMPY)
  SET(HAVE_BOOST_NUMPY 1)
  INCLUDE_DIRECTORIES( ${BOOST_NUMPY_INCLUDE_DIR} )
  SET(requiredLibs ${requiredLibs} ${BOOST_NUMPY_LIBRARY} )
ENDIF(BOOST_NUMPY_FOUND)
endif (Boost_VERSION VERSION_LESS "1.63.0")

# Find Lapack library. Required to compile.
IF(WIN32 AND DEFINED EXTERNAL_LIBRARIES_DIR)
  set(LAPACK_ROOT_DIR "${EXTERNAL_LIBRARIES_DIR}/lapack")
  set(CLAPACK_DIR "${EXTERNAL_LIBRARIES_DIR}/clapack")
ENDIF(WIN32 AND DEFINED EXTERNAL_LIBRARIES_DIR)

SET(HAVE_LAPACK 0)
IF(WIN32)
  FIND_PACKAGE(CLAPACK)
  IF(CLAPACK_FOUND)
    SET(LAPACK_FOUND ${CLAPACK_FOUND})
    add_definitions(-DHAVE_CLAPACK)
    include_directories(${CLAPACK_INCLUDE_DIR})
    SET(LAPACK_LIBRARIES ${CLAPACK_LIBRARIES})
  ENDIF(CLAPACK_FOUND)

  IF(NOT LAPACK_FOUND AND DEFINED EXTERNAL_LIBRARIES_DIR)
    SET(LAPACK_LIB_DIR "${EXTERNAL_LIBRARIES_DIR}/lapack/lib")
    # for now just manually define the libraries ..
    SET(LAPACK_LIBRARIES "${LAPACK_LIB_DIR}/atlas.lib" 
               "${LAPACK_LIB_DIR}/cblas.lib" 
               "${LAPACK_LIB_DIR}/f77blas.lib"
               "${LAPACK_LIB_DIR}/g2c.lib"
               "${LAPACK_LIB_DIR}/gcc.lib"
               "${LAPACK_LIB_DIR}/lapack.lib"
               )
    SET(LAPACK_FOUND 1)
  ENDIF(NOT LAPACK_FOUND AND DEFINED EXTERNAL_LIBRARIES_DIR)
ELSE()
  FIND_PACKAGE(LAPACK REQUIRED)
ENDIF(WIN32)
IF(LAPACK_FOUND)
  ADD_DEFINITIONS(-DHAVE_LAPACK)
  SET(HAVE_LAPACK 1)
  INCLUDE_DIRECTORIES( ${LAPACK_INCLUDE_DIR} )
  SET(requiredLibs ${requiredLibs} ${LAPACK_LIBRARIES} )
ENDIF(LAPACK_FOUND)

#OpenCV 
IF(WIN32 AND DEFINED EXTERNAL_LIBRARIES_DIR)
  SET(OPENCV_ROOT_DIR "${EXTERNAL_LIBRARIES_DIR}/opencv")
ENDIF(WIN32 AND DEFINED EXTERNAL_LIBRARIES_DIR)

SET(HAVE_OPENCV 0)
FIND_PACKAGE(OpenCV COMPONENTS calib3d core features2d flann highgui imgcodecs imgproc ml objdetect video videoio)
IF(OPENCV_FOUND)
  MESSAGE(STATUS "Found: opencv, includes: ${OPENCV_INCLUDE_DIR}, libraries: ${OPENCV_LIBRARIES}")
  ADD_DEFINITIONS(-DHAVE_OPENCV)
  SET(HAVE_OPENCV 1)
  INCLUDE_DIRECTORIES( ${OPENCV_INCLUDE_DIR} )
  SET(requiredLibs ${requiredLibs} ${OPENCV_LIBRARIES} )
ENDIF(OPENCV_FOUND)

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


find_package(Log4cpp REQUIRED)
IF(LOG4CPP_FOUND)
  add_definitions(-DHAVE_LOG4CPP)
  SET(HAVE_LOG4CPP 1)
  INCLUDE_DIRECTORIES( ${LOG4CPP_INCLUDE_DIRS} )
  SET(requiredLibs ${requiredLibs} ${LOG4CPP_LIBRARIES} )
ENDIF(LOG4CPP_FOUND)

find_package(TinyXML REQUIRED)
IF(TINYXML_FOUND)
  ADD_DEFINITIONS(-DTIXML_USE_STL)
  ADD_DEFINITIONS(-DHAVE_TINYXML)
  SET(HAVE_TINYXML 1)
  INCLUDE_DIRECTORIES( ${TINYXML_INCLUDE_DIR} )
  SET(requiredLibs ${requiredLibs} ${TINYXML_LIBRARIES} )
ENDIF(TINYXML_FOUND)

# TBB
# setup defaults for windows binary distributions
IF(WIN32)
  IF(MSVC71)
    SET(TBB_COMPILER "vc7.1")
  ENDIF(MSVC71)
  IF(MSVC80)
    SET(TBB_COMPILER "vc8")
  ENDIF(MSVC80)
  IF(MSVC90)
    SET(TBB_COMPILER "vc9")
  ENDIF(MSVC90)
  IF(MSVC10)
    sET(TBB_COMPILER "vc10")
  ENDIF(MSVC10)
  IF(X86_64)
    SET(TBB_ARCHITECTURE "intel64")
  ELSE()
    SET(TBB_ARCHITECTURE "ia32")
  ENDIF(X86_64)
ENDIF(WIN32)

IF(WIN32 AND DEFINED EXTERNAL_LIBRARIES_DIR)
  SET(TBB_INSTALL_DIR "${EXTERNAL_LIBRARIES_DIR}/tbb")
ENDIF(WIN32 AND DEFINED EXTERNAL_LIBRARIES_DIR)

SET(HAVE_TBB 0)
FIND_PACKAGE(TBB)
IF(TBB_FOUND)
  ADD_DEFINITIONS(-DHAVE_TBB)
  SET(HAVE_TBB 1)
  INCLUDE_DIRECTORIES( ${TBB_INCLUDE_DIR} )
  SET(requiredLibs ${requiredLibs} ${TBB_LIBRARIES} )
ENDIF(TBB_FOUND)

INCLUDE(cmake/PythonModuleBuilder.cmake)
INCLUDE(cmake/UbiTrackDetectPython.cmake)

INCLUDE_DIRECTORIES(${PYTHON_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${PYTHON_NUMPY_INCLUDE_DIR})
SET(requiredLibs ${requiredLibs} ${PYTHON_LIBRARY})

IF( COMMAND cmake_policy )
  IF( POLICY CMP0003 )
    CMAKE_POLICY(SET CMP0003 OLD)
  ENDIF( POLICY CMP0003 )
ENDIF( COMMAND cmake_policy )

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
