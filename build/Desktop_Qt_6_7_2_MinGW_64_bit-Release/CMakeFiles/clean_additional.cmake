# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Release")
  file(REMOVE_RECURSE
  "CMakeFiles\\ChattingApp_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\ChattingApp_autogen.dir\\ParseCache.txt"
  "ChattingApp_autogen"
  )
endif()
