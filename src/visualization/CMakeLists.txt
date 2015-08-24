SET(CURRENT_PACKAGE "ubitrack/visualization")

SET (MODULE_SRCS_UTVISUALIZATION "wrap_utVisualization.cpp"
		"BackgroundImage.cpp")
				 
add_python_module("_utvisualization" ${CURRENT_PACKAGE} "MODULE_SRCS_UTVISUALIZATION")


if(HAVE_UBITRACK_RENDERAPI)
	MESSAGE("Include support for utRenderAPI")
	SET (MODULE_SRCS_UTRENDERAPI "wrap_utRenderAPI.cpp")
	add_python_module("_utrenderapi" ${CURRENT_PACKAGE} "MODULE_SRCS_UTRENDERAPI")
endif(HAVE_UBITRACK_RENDERAPI)