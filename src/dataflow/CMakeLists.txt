SET(CURRENT_PACKAGE "ubitrack/dataflow")

SET (MODULE_SRCS "wrap_utDataflow.cpp")
add_python_module("_utdataflow" ${CURRENT_PACKAGE} ${MODULE_SRCS}) 


#SET (MODULE_SRCS "wrap_utMeasurement.cpp")
#add_python_module("_utmeasurement" ${CURRENT_PACKAGE} ${MODULE_SRCS}) 
