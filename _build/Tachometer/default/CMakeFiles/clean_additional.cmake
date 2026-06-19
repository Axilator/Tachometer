# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "")
  file(REMOVE_RECURSE
  "C:\\Users\\Bengt Palmkvist\\Documents\\Project\\Tachometer\\out\\Tachometer\\default.cmf"
  "C:\\Users\\Bengt Palmkvist\\Documents\\Project\\Tachometer\\out\\Tachometer\\default.hex"
  "C:\\Users\\Bengt Palmkvist\\Documents\\Project\\Tachometer\\out\\Tachometer\\default.hxl"
  "C:\\Users\\Bengt Palmkvist\\Documents\\Project\\Tachometer\\out\\Tachometer\\default.mum"
  "C:\\Users\\Bengt Palmkvist\\Documents\\Project\\Tachometer\\out\\Tachometer\\default.o"
  "C:\\Users\\Bengt Palmkvist\\Documents\\Project\\Tachometer\\out\\Tachometer\\default.sdb"
  "C:\\Users\\Bengt Palmkvist\\Documents\\Project\\Tachometer\\out\\Tachometer\\default.sym"
  )
endif()
