# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/ethan/Robobrawl/DDSClock/GUI/build/imgui-1.90.9/src/imgui"
  "/home/ethan/Robobrawl/DDSClock/GUI/build/imgui-1.90.9/src/imgui-build"
  "/home/ethan/Robobrawl/DDSClock/GUI/build/imgui-1.90.9"
  "/home/ethan/Robobrawl/DDSClock/GUI/build/imgui-1.90.9/tmp"
  "/home/ethan/Robobrawl/DDSClock/GUI/build/imgui-1.90.9/src/imgui-stamp"
  "/home/ethan/Robobrawl/DDSClock/GUI/build/imgui-1.90.9/src"
  "/home/ethan/Robobrawl/DDSClock/GUI/build/imgui-1.90.9/src/imgui-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/ethan/Robobrawl/DDSClock/GUI/build/imgui-1.90.9/src/imgui-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/ethan/Robobrawl/DDSClock/GUI/build/imgui-1.90.9/src/imgui-stamp${cfgdir}") # cfgdir has leading slash
endif()
