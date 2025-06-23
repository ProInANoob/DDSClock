########################################
# Select COREDX Distribution

if ( NOT DEFINED ENV{COREDX_TOP} )
   message( FATAL_ERROR "Required variable not set: COREDX_TOP" )
endif()
if ( NOT DEFINED ENV{COREDX_TARGET} )
   message( FATAL_ERROR "Required variable not set: COREDX_TARGET" )
endif()
if ( NOT DEFINED ENV{COREDX_HOST} )
   message( FATAL_ERROR "Required variable not set: COREDX_HOST" )
endif()
FILE( TO_CMAKE_PATH   $ENV{COREDX_TOP}    COREDX_TOP    )
FILE( TO_CMAKE_PATH   $ENV{COREDX_TARGET} COREDX_TARGET )
FILE( TO_CMAKE_PATH   $ENV{COREDX_HOST}   COREDX_HOST   )

set( DDS_C_INC_PATH   ${COREDX_TOP}/target/include )
set( DDS_CC_INC_PATH  ${COREDX_TOP}/target/include )
set( DDS_CXX_INC_PATH ${COREDX_TOP}/target/include/dds_cxx )

set( DDS_LIB_PATH  ${COREDX_TOP}/target/${COREDX_TARGET}/lib )

########################################
# C Libraries

if ( UNIX )
  set( DDS_C_LIBS
    libdds_qos_provider_log.a
    libdds_xml_log.a
    libdds_cf_log.a
    )
else()
  set( DDS_C_LIBS
    dds_qos_provider_log
    dds_xml_log
    dds_cf_log
    )
  
  # under windows, select the 'debug' lib versions if required
  set( TMP_DDS_LIBS ${DDS_C_LIBS} )
  set( DDS_C_LIBS )
  foreach( LIB ${TMP_DDS_LIBS} )
    if ( CMAKE_BUILD_TYPE STREQUAL "Debug" )
      set( DDS_C_LIBS ${DDS_C_LIBS} ${LIB}_d_static )
    else()
      set( DDS_C_LIBS ${DDS_C_LIBS} ${LIB}_static )
    endif()
  endforeach()
endif()

########################################
# C++ Libraries

if ( UNIX )
  set( DDS_CC_LIBS
    libdds_cpp_log.a
    libdds_qos_provider_log.a
    libdds_cf_log.a
    )
else()
  set( DDS_CC_LIBS
    dds_cpp_log
    dds_qos_provider_log
    dds_cf_log
    )
  # under windows, select the 'debug' lib versions if required
  set( TMP_DDS_LIBS ${DDS_CC_LIBS} )
  set( DDS_CC_LIBS )
  foreach( LIB ${TMP_DDS_LIBS} )
    if ( CMAKE_BUILD_TYPE STREQUAL "Debug" )
      set( DDS_CC_LIBS ${DDS_CC_LIBS} ${LIB}_d_static )
    else()
      set( DDS_CC_LIBS ${DDS_CC_LIBS} ${LIB}_static )
    endif()
  endforeach()
endif()

########################################
# C++11 Libraries

if ( UNIX )
  set( DDS_CXX_LIBS
    libdds_cxx_log.a
    libdds_qos_provider_log.a
    libdds_dyntype_log.a
    libdds_xml_log.a
    libdds_cf_log.a
    )
else()
  set( DDS_CXX_LIBS
    dds_cxx_log
    dds_qos_provider_log
    dds_dyntype_log
    dds_xml_log
    dds_cf_log
    )
  # under windows, select the 'debug' lib versions if required
  set( TMP_DDS_LIBS ${DDS_CXX_LIBS} )
  set( DDS_CXX_LIBS )
  foreach( LIB ${TMP_DDS_LIBS} )
    if ( CMAKE_BUILD_TYPE STREQUAL "Debug" )
      set( DDS_CXX_LIBS ${DDS_CXX_LIBS} ${LIB}_d_static )
    else()
      set( DDS_CXX_LIBS ${DDS_CXX_LIBS} ${LIB}_static )
    endif()
  endforeach()
endif()


########################################
# IDL compiler

if (WIN32)
  set(EXT ".exe")
endif()
set( COREDX_DDL_EXE   "${COREDX_TOP}/host/${COREDX_HOST}/bin/coredx_ddl${EXT}" )
add_executable( coredx_ddl IMPORTED )
set_property( TARGET coredx_ddl
  PROPERTY IMPORTED_LOCATION ${COREDX_DDL_EXE} )


