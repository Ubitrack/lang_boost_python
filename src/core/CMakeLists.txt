SET(CURRENT_PACKAGE "ubitrack/core")

SET (MODULE_SRCS "wrap_utMath.cpp")
add_python_module("_utmath" ${MODULE_SRCS}) 


SET (MODULE_SRCS "wrap_utMeasurement.cpp")
add_python_module("_utmeasurement" ${MODULE_SRCS}) 
