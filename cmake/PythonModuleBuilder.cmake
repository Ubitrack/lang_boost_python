
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


macro(add_python_module full_modname the_package MODULE_SRC_NAME)

MESSAGE(STATUS "python module ${full_modname} with sources: ${${MODULE_SRC_NAME}}")
ADD_LIBRARY(${full_modname} MODULE ${${MODULE_SRC_NAME}})

set_property(TARGET ${full_modname} PROPERTY PREFIX "")

if(WIN32)
    set_property(TARGET ${full_modname} PROPERTY SUFFIX ".pyd")
endif()

# make the name of debug libraries end in _d.
SET_TARGET_PROPERTIES( ${full_modname} PROPERTIES DEBUG_POSTFIX "_d" )

IF(UNIX) 
  SET_TARGET_PROPERTIES( ${full_modname} PROPERTIES OUTPUT_NAME ${full_modname} )
ENDIF(UNIX)

TARGET_LINK_LIBRARIES( ${full_modname} ${MODULE_LIBS})


# Apply Target Properties
if(MSVC)
  if(CMAKE_CROSSCOMPILING)
    set_target_properties(${full_modname} PROPERTIES LINK_FLAGS "/NODEFAULTLIB:secchk")
  endif()
  set_target_properties(${full_modname} PROPERTIES LINK_FLAGS "/NODEFAULTLIB:libc /DEBUG")
endif()

foreach(_flag ${UBITRACK_COMPILE_FLAGS})
  set_target_properties(${full_modname} PROPERTIES COMPILE_FLAGS "${_flag}")
endforeach()
foreach(_flag ${UBITRACK_LINK_FLAGS})
  set_target_properties(${full_modname} PROPERTIES LINK_FLAGS "${_flag}")
endforeach()
foreach(_flag ${UBITRACK_LINK_FLAGS_DEBUG})
  set_target_properties(${full_modname} PROPERTIES LINK_FLAGS_DEBUG "${_flag}")
endforeach()
foreach(_symb ${UBITRACK_DEFINES})
  set_target_properties(${full_modname} PROPERTIES DEFINE_SYMBOL ${_symb})
endforeach()

# set compiler Definitions
set_target_properties(${full_modname} PROPERTIES COMPILE_DEFINITIONS "${UBITRACK_COMPILE_DEFINITIONS}")

# set fPIC
set_property(TARGET ${full_modname} PROPERTY POSITION_INDEPENDENT_CODE ON)

# IF(MSVC)
#   # Create variables
#   SET( ${full_modname}_LINK_FLAGS_DEBUG "" )
  
#   SET( PROJECT_MSVC_VERSION 6 )
#   SET( TEMP_MSVC_VERSION 1299 )
#   WHILE( ${MSVC_VERSION} GREATER ${TEMP_MSVC_VERSION} )
#     MATH( EXPR PROJECT_MSVC_VERSION "${PROJECT_MSVC_VERSION} + 1" )
#     MATH( EXPR TEMP_MSVC_VERSION "${TEMP_MSVC_VERSION} + 100" )
#   ENDWHILE( ${MSVC_VERSION} GREATER ${TEMP_MSVC_VERSION} )
  
#   # Treat wchar_t as built in type for all visual studio versions.
#   # This is default for every version above 7 ( so far ) but we still set it for all.
#   SET( ${full_modname}_COMPILE_FLAGS "${${full_modname}_COMPILE_FLAGS} /Zc:wchar_t")
  
#   IF( ${MSVC_VERSION} GREATER 1399 )
#     # Remove compiler warnings about deprecation for visual studio versions 8 and above.
#     SET( ${full_modname}_COMPILE_FLAGS "${${full_modname}_COMPILE_FLAGS} -D_CRT_SECURE_NO_DEPRECATE" )
#   ENDIF( ${MSVC_VERSION} GREATER 1399 )

#   IF( ${MSVC_VERSION} GREATER 1499 )
#     # Build using several threads for visual studio versions 9 and above.
#     SET( ${full_modname}_COMPILE_FLAGS "${${full_modname}_COMPILE_FLAGS} /MP" )
#     # Remove link against default library for debug builds.
#     SET( ${full_modname}_LINK_FLAGS_DEBUG "${${full_modname}_LINK_FLAGS_DEBUG} /NODEFAULTLIB:msvcrt" )
#   ENDIF( ${MSVC_VERSION} GREATER 1499 )

#   #SET_TARGET_PROPERTIES( ${full_modname} PROPERTIES OUTPUT_NAME ${full_modname}_vc${PROJECT_MSVC_VERSION} )

#   # Seems like ${full_modname} needs to have incremental linking shut off for vc8. Since this is
#   # not the most important feature it is shut off for all visual studio versions.
#   SET_TARGET_PROPERTIES( ${full_modname} PROPERTIES LINK_FLAGS_DEBUG "${${full_modname}_LINK_FLAGS_DEBUG} /INCREMENTAL:NO" )

# ENDIF(MSVC)

# set compile flags for ${full_modname} project
# SET_TARGET_PROPERTIES( ${full_modname} PROPERTIES COMPILE_FLAGS "${${full_modname}_COMPILE_FLAGS}" )

add_custom_command(
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/timestamp.${full_modname}
  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${full_modname}> ${CMAKE_BINARY_DIR}/build/modules/${the_package}/$<TARGET_FILE_NAME:${full_modname}>
  COMMAND ${CMAKE_COMMAND} -E touch ${CMAKE_CURRENT_BINARY_DIR}/timestamp.${full_modname}
  DEPENDS ${DEPS}
  VERBATIM
)
add_custom_target(copy${full_modname} ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/timestamp.${full_modname} )
ADD_DEPENDENCIES(copy${full_modname} ${full_modname})
ADD_DEPENDENCIES(pybuild copy${full_modname})

endmacro()
