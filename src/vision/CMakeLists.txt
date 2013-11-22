SET(CURRENT_PACKAGE "ubitrack/vision")

SET (MODULE_SRCS "wrap_utVision.cpp" )
add_python_module("_utvision" ${CURRENT_PACKAGE} ${MODULE_SRCS}) 
