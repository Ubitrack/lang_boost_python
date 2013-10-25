# try to find the pyublas library

execute_process ( COMMAND python -c "import pyublas; print(pyublas.__path__[0])" OUTPUT_VARIABLE PYUBLAS_OUT OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process ( COMMAND python -c "import numpy; print numpy.get_include()" OUTPUT_VARIABLE NUMPY_OUT OUTPUT_STRIP_TRAILING_WHITESPACE)

find_path(PYUBLAS_INCLUDE_DIR pyublas/numpy.hpp
          HINTS ${PYUBLAS_OUT}/include /usr/local/include /usr/include)

find_path(NUMPY_INCLUDE_DIR numpy/arrayobject.h
          HINTS ${NUMPY_OUT} /usr/local/include /usr/include)

MESSAGE(STATUS "PyUblas include: ${PYUBLAS_INCLUDE_DIR}")
MESSAGE(STATUS "Numpy include: ${NUMPY_INCLUDE_DIR}")
#mark_as_advanced(PYUBLAS_INCLUDE_DIR)


