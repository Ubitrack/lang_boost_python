SET(CURRENT_PACKAGE "ubitrack/dataflow")

SET (MODULE_SRCS "wrap_utDataflow.cpp")
add_python_module("_utdataflow" ${CURRENT_PACKAGE} ${MODULE_SRCS}) 


SET (MODULE_SRCS "wrap_utGraph.cpp")
add_python_module("_utgraph" ${CURRENT_PACKAGE} ${MODULE_SRCS}) 
