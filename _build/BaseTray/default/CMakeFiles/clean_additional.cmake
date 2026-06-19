# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "")
  file(REMOVE_RECURSE
  "C:\\Users\\Bengt Palmkvist\\Documents\\Project\\Tachometer\\out\\BaseTray\\default.cmf"
  "C:\\Users\\Bengt Palmkvist\\Documents\\Project\\Tachometer\\out\\BaseTray\\default.hex"
  "C:\\Users\\Bengt Palmkvist\\Documents\\Project\\Tachometer\\out\\BaseTray\\default.hxl"
  "C:\\Users\\Bengt Palmkvist\\Documents\\Project\\Tachometer\\out\\BaseTray\\default.mum"
  "C:\\Users\\Bengt Palmkvist\\Documents\\Project\\Tachometer\\out\\BaseTray\\default.o"
  "C:\\Users\\Bengt Palmkvist\\Documents\\Project\\Tachometer\\out\\BaseTray\\default.sdb"
  "C:\\Users\\Bengt Palmkvist\\Documents\\Project\\Tachometer\\out\\BaseTray\\default.sym"
  )
endif()
