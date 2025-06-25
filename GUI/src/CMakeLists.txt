#######################################################################
#
#   Copyright(C) 2024      Twin Oaks Computing, Inc
#   All rights reserved.   Castle Rock, CO 80108
#
#######################################################################
#
#
#######################################################################

cmake_minimum_required(VERSION 3.16)
project (Robobrawl_Clock VERSION 0.0.1 LANGUAGES CXX )

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)




#######################################################################

include( ../cmake/FindCoreDX.cmake )
include( Macros.cmake )

#######################################################################

set( Robobrawl_Clock ${CMAKE_CURRENT_SOURCE_DIR}/../ )

#######################################################################

#######################################################################
# ImGui

include(ExternalProject)
ExternalProject_Add( imgui
  PREFIX imgui-1.90.9
  URL https://github.com/ocornut/imgui/archive/refs/tags/v1.90.9.tar.gz
  CONFIGURE_COMMAND ""
  BUILD_COMMAND ""
  INSTALL_COMMAND ""
  # DOWNLOAD_EXTRACT_TIMESTAMP True
  )

set( IMGUI_TOP  ${CMAKE_CURRENT_BINARY_DIR}/imgui-1.90.9/src/imgui )

set( IMGUI_SRCS 
  ${IMGUI_TOP}/imgui.cpp
  ${IMGUI_TOP}/imgui_draw.cpp
  ${IMGUI_TOP}/imgui_tables.cpp
  ${IMGUI_TOP}/imgui_widgets.cpp
  ${IMGUI_TOP}/misc/freetype/imgui_freetype.cpp
  ${IMGUI_TOP}/imgui_demo.cpp
  )

if ( UNIX )
  set( BACKEND "GLFW" )
else()
  set( BACKEND "DX11" )
endif()

if ( BACKEND STREQUAL "GLFW" )
  set( USE_GLFW  "yes" )
  set( BACKEND_FLAGS "-DUSE_GLFW" )
elseif ( BACKEND STREQUAL "DX11" )
  set( USE_DX11  "yes" )
  set( BACKEND_FLAGS "-DUSE_DX11" )
endif( )

if ( USE_GLFW )
  if ( UNIX )
    set( GLFW_TOP          /usr )
    set( IMGUI_BE_LIBS     GL glfw rt m dl )
    set( IMGUI_BE_INCLUDES ${GLFW_TOP}/include )
  else()
    set( GLFW_TOP          ${CMAKE_CURRENT_SOURCE_DIR}/../../glfw-3.3.8.bin.WIN64 )
    set( IMGUI_BE_LIB_PATH ${GLFW_TOP}/lib-vc2015 )
    set( IMGUI_BE_LIBS     glfw3 opengl32 )
    set( IMGUI_BE_INCLUDES ${GLFW_TOP}/include )
  endif()
  set( IMGUI_BACKEND_SRCS
    ${IMGUI_TOP}/backends/imgui_impl_glfw.cpp
    ${IMGUI_TOP}/backends/imgui_impl_opengl2.cpp)
endif()

if ( USE_DX11 )
  set( IMGUI_BACKEND_SRCS
    ${IMGUI_TOP}/backends/imgui_impl_win32.cpp
    ${IMGUI_TOP}/backends/imgui_impl_dx11.cpp)
  set( IMGUI_BE_LIBS d3d11.lib d3dcompiler.lib )
endif()

#######################################################################
#  FreeType 2

ExternalProject_Add( ft2
  PREFIX freetype-2.13.2
  URL https://sourceforge.net/projects/freetype/files/freetype2/2.13.2/freetype-2.13.2.tar.xz
  
  #DOWNLOAD_EXTRACT_TIMESTAMP True
  CMAKE_ARGS -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} -DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_BINARY_DIR}/freetype-2.13.2 -DFT_DISABLE_HARFBUZZ=1 -DFT_DISABLE_BROTLI=1
  INSTALL_DIR ${CMAKE_CURRENT_BINARY_DIR}/freetype-2.13.2
  )
set( FT2_INCLUDE  ${CMAKE_CURRENT_BINARY_DIR}/freetype-2.13.2/include/freetype2 )
set( FT2_LIB_PATH ${CMAKE_CURRENT_BINARY_DIR}/freetype-2.13.2/lib )
if ( WIN32 )
  set( FT2_LIBS     freetype$<$<CONFIG:Debug>:d> )
else()
  set( FT2_LIBS     freetype png z )
endif()  

#######################################################################
# these source files come from external projects, not available until
# those projects are built:
set_source_files_properties( ${IMGUI_SRCS} ${IMGUI_BACKEND_SRCS} 
  PROPERTIES
  GENERATED TRUE )



#######################################################################
#  IDL -> c++ 
set( IDL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/idl )
set( IDL_OPTIONS -Wno-1093 -F -s )
set( IDL_GEN_DIR ${CMAKE_CURRENT_BINARY_DIR} )

set( IDL_FILE ../src/idl/robobrawl.idl )


IDL_GEN_CXX_IN_DIR( ${CMAKE_CURRENT_SOURCE_DIR}
  ${IDL_FILE}
  ${IDL_GEN_DIR}
  "${IDL_OPTIONS}"
  GEN_CXX_SRCS
  GEN_CXX_HDRS )

#######################################################################
add_executable ( Robobrawl_Clock
main.cc
gui.cc
controller.cc

robobrawlInit.cc
robobrawlStartup.cc
robobrawlMainLoop.cc
robobrawlCleanup.cc



${IMGUI_SRCS}
${IMGUI_BACKEND_SRCS}
 
${GEN_CXX_SRCS}
  )
add_dependencies( Robobrawl_Clock imgui ft2 )


target_compile_options( Robobrawl_Clock PUBLIC ${BACKEND_FLAGS} )
if ( NOT WIN32 )
  target_compile_options( Robobrawl_Clock PUBLIC -Wall )
endif()

if ( LINUX )
  set( LINUX_LIBS pthread )
endif()

if ( WIN32 )
  set_property(TARGET Robobrawl_Clock PROPERTY
    MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
  set ( WIN_LIBS ws2_32 iphlpapi )
else()
  target_compile_options( Robobrawl_Clock PUBLIC -Wall )
endif()

target_include_directories( Robobrawl_Clock
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/>
  PUBLIC $<BUILD_INTERFACE:${DDS_CXX_INC_PATH}>
  PUBLIC $<BUILD_INTERFACE:${DDS_C_INC_PATH}>
  PUBLIC $<BUILD_INTERFACE:${IMGUI_TOP}>
  PUBLIC $<BUILD_INTERFACE:${IMGUI_TOP}/backends>
  PUBLIC $<BUILD_INTERFACE:${IMGUI_BE_INCLUDES}>
  PUBLIC $<BUILD_INTERFACE:${FT2_INCLUDE}>
  #PUBLIC $<BUILD_INTERFACE:C:/Users/Ethan/StackLight/glfw-3.3.8.bin.WIN64/include>
  )

target_link_directories( Robobrawl_Clock
  PUBLIC  ${DDS_LIB_PATH}
  PUBLIC  ${FT2_LIB_PATH}
  PUBLIC  ${IMGUI_BE_LIB_PATH}
  )

if (WIN32)
  set( CMAKE_CXX_FLAGS /std:c++17 )
  set_property(TARGET Robobrawl_Clock PROPERTY
             MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")  
  target_compile_options( Robobrawl_Clock PUBLIC ${BACKEND_FLAGS} /EHsc /D_WINSOCKAPI_= /wd4820 /wd5219 /wd4514 /wd4365 /wd4244 )
  target_link_options( Robobrawl_Clock PUBLIC PUBLIC /INCREMENTAL:NO /NODEFAULTLIB:LIBCMT )
  set(WIN_LIBS ws2_32.lib iphlpapi.lib ) 
endif()

target_link_libraries( Robobrawl_Clock
  PRIVATE  ${DDS_CXX_LIBS}
  PRIVATE  ${LINUX_LIBS}
  PRIVATE  ${FT2_LIBS}
  PRIVATE  ${IMGUI_BE_LIBS}
  PRIVATE  ${WIN_LIBS}
  )

