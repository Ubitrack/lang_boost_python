if(NOT COMMAND find_host_package)
  macro(find_host_package)
    find_package(${ARGN})
  endmacro()
endif()
if(NOT COMMAND find_host_program)
  macro(find_host_program)
    find_program(${ARGN})
  endmacro()
endif()

macro(add_python_module full_modname MODULE_SRCS)

ADD_LIBRARY(${full_modname} MODULE ${MODULE_SRCS})

set_property(TARGET ${full_modname} PROPERTY PREFIX "")

#if(WIN32)
#    set_property(TARGET ${full_modname} PROPERTY SUFFIX ".pyd")
#endif()

# make the name of debug libraries end in _d.
SET_TARGET_PROPERTIES( ${full_modname} PROPERTIES DEBUG_POSTFIX "_d" )

IF(UNIX) 
  SET_TARGET_PROPERTIES( ${full_modname} PROPERTIES OUTPUT_NAME ${full_modname} )
ENDIF(UNIX)

TARGET_LINK_LIBRARIES( ${full_modname} ${MODULE_LIBS})

SET( ${full_modname}_COMPILE_FLAGS ${${PROJECT_NAME}_COMPILE_FLAGS} )
SET( ${full_modname}_LINK_FLAGS ${${PROJECT_NAME}_LINK_FLAGS} )
SET( ${full_modname}_COMPILE_FLAGS_DEBUG ${${PROJECT_NAME}_COMPILE_FLAGS_DEBUG} )
SET( ${full_modname}_LINK_FLAGS_DEBUG ${${PROJECT_NAME}_LINK_FLAGS_DEBUG} )

IF(MSVC)
  # Create variables
  SET( ${full_modname}_LINK_FLAGS_DEBUG "" )
  
  SET( PROJECT_MSVC_VERSION 6 )
  SET( TEMP_MSVC_VERSION 1299 )
  WHILE( ${MSVC_VERSION} GREATER ${TEMP_MSVC_VERSION} )
    MATH( EXPR PROJECT_MSVC_VERSION "${PROJECT_MSVC_VERSION} + 1" )
    MATH( EXPR TEMP_MSVC_VERSION "${TEMP_MSVC_VERSION} + 100" )
  ENDWHILE( ${MSVC_VERSION} GREATER ${TEMP_MSVC_VERSION} )
  
  # Treat wchar_t as built in type for all visual studio versions.
  # This is default for every version above 7 ( so far ) but we still set it for all.
  SET( ${full_modname}_COMPILE_FLAGS "${${full_modname}_COMPILE_FLAGS} /Zc:wchar_t")
  
  IF( ${MSVC_VERSION} GREATER 1399 )
    # Remove compiler warnings about deprecation for visual studio versions 8 and above.
    SET( ${full_modname}_COMPILE_FLAGS "${${full_modname}_COMPILE_FLAGS} -D_CRT_SECURE_NO_DEPRECATE" )
  ENDIF( ${MSVC_VERSION} GREATER 1399 )

  IF( ${MSVC_VERSION} GREATER 1499 )
    # Build using several threads for visual studio versions 9 and above.
    SET( ${full_modname}_COMPILE_FLAGS "${${full_modname}_COMPILE_FLAGS} /MP" )
    # Remove link against default library for debug builds.
    SET( ${full_modname}_LINK_FLAGS_DEBUG "${${full_modname}_LINK_FLAGS_DEBUG} /NODEFAULTLIB:msvcrt" )
  ENDIF( ${MSVC_VERSION} GREATER 1499 )

  SET_TARGET_PROPERTIES( ${full_modname} PROPERTIES OUTPUT_NAME ${full_modname}_vc${PROJECT_MSVC_VERSION} )

  # Seems like ${full_modname} needs to have incremental linking shut off for vc8. Since this is
  # not the most important feature it is shut off for all visual studio versions.
  SET_TARGET_PROPERTIES( ${full_modname} PROPERTIES LINK_FLAGS_DEBUG "${${full_modname}_LINK_FLAGS_DEBUG} /INCREMENTAL:NO" )

ENDIF(MSVC)

IF( NOT APPLE )
  SET( ${full_modname}_FULL_VERSION ${${PROJECT_NAME}_MAJOR_VERSION}.${${PROJECT_NAME}_MINOR_VERSION}.${${PROJECT_NAME}_BUILD_VERSION} ) 
  SET_TARGET_PROPERTIES( ${full_modname} PROPERTIES VERSION ${${full_modname}_FULL_VERSION} )
ENDIF( NOT APPLE )

# set compile flags for ${full_modname} project
SET_TARGET_PROPERTIES( ${full_modname} PROPERTIES COMPILE_FLAGS "${${full_modname}_COMPILE_FLAGS}" )

endmacro()
