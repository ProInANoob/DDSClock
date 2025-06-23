#######################################################################
#
#   Copyright(C) 2024      Twin Oaks Computing, Inc
#   All rights reserved.   Castle Rock, CO 80108
#
######################################################
#
macro(target_public_headers TARGET)
  set_target_properties( ${TARGET} PROPERTIES PUBLIC_HEADER "${ARGN}")
endmacro()

######################################################
#
macro(IDL_GEN_CXX_IN_DIR working_dir idlfile destdir ddl_options gen_srcs_out gen_hdrs_out )
  get_filename_component( IDL_FILE      ${idlfile}  NAME )
  get_filename_component( IDL_FILE_BASE ${IDL_FILE} NAME_WE )
  
  set( _SRCS  ${IDL_FILE_BASE}.cc ${IDL_FILE_BASE}TypeSupport.cc )
  set( _HDRS  ${IDL_FILE_BASE}.hh ${IDL_FILE_BASE}TypeSupport.hh )

  # message( STATUS "IDL: ${idlfile} :: ${IDL_PATH}/${IDL_FILE} -> ${_HDRS} + ${_SRCS}" )

  add_custom_command(
    OUTPUT ${destdir}/${_SRCS} ${destdir}/${_HDRS}
    WORKING_DIRECTORY ${working_dir}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${destdir}/${IDL_PATH}
    COMMAND coredx_ddl ${COREDX_DDL_FLAGS} -l cpp11 -d ${destdir}/${IDL_PATH} ${ddl_options} -f ${idlfile}
    DEPENDS ${idlfile}
    DEPENDS coredx_ddl )

  set( ${gen_srcs_out} ${${gen_srcs_out}} ${_SRCS} )
  set( ${gen_hdrs_out} ${${gen_hdrs_out}} ${_HDRS} )
  
endmacro( )
