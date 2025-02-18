# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles/appblockchain_autogen.dir/AutogenUsed.txt"
  "CMakeFiles/appblockchain_autogen.dir/ParseCache.txt"
  "appblockchain_autogen"
  )
endif()
