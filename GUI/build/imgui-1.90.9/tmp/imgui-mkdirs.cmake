# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

# If CMAKE_DISABLE_SOURCE_CHANGES is set to true and the source directory is an
# existing directory in our source tree, calling file(MAKE_DIRECTORY) on it
# would cause a fatal error, even though it would be a no-op.
if(NOT EXISTS "C:/Users/Ethan/Documents/GitHub/DDSClock/GUI/build/imgui-1.90.9/src/imgui")
  file(MAKE_DIRECTORY "C:/Users/Ethan/Documents/GitHub/DDSClock/GUI/build/imgui-1.90.9/src/imgui")
endif()
file(MAKE_DIRECTORY
  "C:/Users/Ethan/Documents/GitHub/DDSClock/GUI/build/imgui-1.90.9/src/imgui-build"
  "C:/Users/Ethan/Documents/GitHub/DDSClock/GUI/build/imgui-1.90.9"
  "C:/Users/Ethan/Documents/GitHub/DDSClock/GUI/build/imgui-1.90.9/tmp"
  "C:/Users/Ethan/Documents/GitHub/DDSClock/GUI/build/imgui-1.90.9/src/imgui-stamp"
  "C:/Users/Ethan/Documents/GitHub/DDSClock/GUI/build/imgui-1.90.9/src"
  "C:/Users/Ethan/Documents/GitHub/DDSClock/GUI/build/imgui-1.90.9/src/imgui-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "C:/Users/Ethan/Documents/GitHub/DDSClock/GUI/build/imgui-1.90.9/src/imgui-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "C:/Users/Ethan/Documents/GitHub/DDSClock/GUI/build/imgui-1.90.9/src/imgui-stamp${cfgdir}") # cfgdir has leading slash
endif()
