# Find BoostNumpy Library

SET(_BOOST_NUMPY_SEARCH
  $ENV{BOOST_NUMPY_DIR}
  $ENV{PROGRAMFILES}/boost
  $ENV{PROGRAMFILES}/boost.numpy
  $ENV{PROGRAMW6432}/boost
  $ENV{PROGRAMW6432}/boost.numpy  
  $ENV{BOOST_ROOT}
  $ENV{BOOST_DIR}
  /usr
  /usr/local)
 
find_path(BOOST_NUMPY_INCLUDE_DIR boost/numpy.hpp PATHS ${_BOOST_NUMPY_SEARCH} PATH_SUFFIXES "include")
find_library(BOOST_NUMPY_LIBRARY NAMES boost_numpy HINTS ${_BOOST_NUMPY_SEARCH} PATH_SUFFIXES "lib" "lib64")
 
include(FindPackageHandleStandardArgs)
 
IF(BOOST_NUMPY_INCLUDE_DIR AND BOOST_NUMPY_LIBRARY)
  SET(BOOST_NUMPY_FOUND 1)
ENDIF(BOOST_NUMPY_INCLUDE_DIR AND BOOST_NUMPY_LIBRARY)

find_package_handle_standard_args(BoostNumPy  DEFAULT_MSG
                                  BOOST_NUMPY_LIBRARY BOOST_NUMPY_INCLUDE_DIR)
 
mark_as_advanced(BOOST_NUMPY_LIBRARY BOOST_NUMPY_INCLUDE_DIR)