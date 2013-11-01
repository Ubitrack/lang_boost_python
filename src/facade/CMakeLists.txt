SET(CURRENT_PACKAGE "ubitrack/facade")

SET (MODULE_SRCS "wrap_utFacade.cpp")
add_python_module("_utfacade" ${CURRENT_PACKAGE} ${MODULE_SRCS}) 


#SET (MODULE_SRCS "wrap_utMeasurement.cpp")
#add_python_module("_utmeasurement" ${CURRENT_PACKAGE} ${MODULE_SRCS}) 
