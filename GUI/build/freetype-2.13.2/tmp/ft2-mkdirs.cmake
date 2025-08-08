# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/ethan/RobobrawlClock/DDSClock/GUI/build/freetype-2.13.2/src/ft2"
  "/home/ethan/RobobrawlClock/DDSClock/GUI/build/freetype-2.13.2/src/ft2-build"
  "/home/ethan/RobobrawlClock/DDSClock/GUI/build/freetype-2.13.2"
  "/home/ethan/RobobrawlClock/DDSClock/GUI/build/freetype-2.13.2/tmp"
  "/home/ethan/RobobrawlClock/DDSClock/GUI/build/freetype-2.13.2/src/ft2-stamp"
  "/home/ethan/RobobrawlClock/DDSClock/GUI/build/freetype-2.13.2/src"
  "/home/ethan/RobobrawlClock/DDSClock/GUI/build/freetype-2.13.2/src/ft2-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/ethan/RobobrawlClock/DDSClock/GUI/build/freetype-2.13.2/src/ft2-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/ethan/RobobrawlClock/DDSClock/GUI/build/freetype-2.13.2/src/ft2-stamp${cfgdir}") # cfgdir has leading slash
endif()
