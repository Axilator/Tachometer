# The following variables contains the files used by the different stages of the build process.
set(Tachometer_default_default_XC8_FILE_TYPE_assemble)
set_source_files_properties(${Tachometer_default_default_XC8_FILE_TYPE_assemble} PROPERTIES LANGUAGE ASM)

# For assembly files, add "." to the include path for each file so that .include with a relative path works
foreach(source_file ${Tachometer_default_default_XC8_FILE_TYPE_assemble})
        set_source_files_properties(${source_file} PROPERTIES INCLUDE_DIRECTORIES "$<PATH:NORMAL_PATH,$<PATH:REMOVE_FILENAME,${source_file}>>")
endforeach()

set(Tachometer_default_default_XC8_FILE_TYPE_assemblePreprocess)
set_source_files_properties(${Tachometer_default_default_XC8_FILE_TYPE_assemblePreprocess} PROPERTIES LANGUAGE ASM)

# For assembly files, add "." to the include path for each file so that .include with a relative path works
foreach(source_file ${Tachometer_default_default_XC8_FILE_TYPE_assemblePreprocess})
        set_source_files_properties(${source_file} PROPERTIES INCLUDE_DIRECTORIES "$<PATH:NORMAL_PATH,$<PATH:REMOVE_FILENAME,${source_file}>>")
endforeach()

set(Tachometer_default_default_XC8_FILE_TYPE_compile
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../Debug.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../Eeprom.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../Handler.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../IO.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../Init.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../Main.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../Monitor.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../SerialPort.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../SysTimer.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../TachoCalc.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../Timer.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../Version.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../main2.c")
set_source_files_properties(${Tachometer_default_default_XC8_FILE_TYPE_compile} PROPERTIES LANGUAGE C)
set(Tachometer_default_default_XC8_FILE_TYPE_link)
set(Tachometer_default_default_XC8_FILE_TYPE_objcopy_avr)
set(Tachometer_default_image_name "default.elf")
set(Tachometer_default_image_base_name "default")

# The output directory of the final image.
set(Tachometer_default_output_dir "${CMAKE_CURRENT_SOURCE_DIR}/../../../out/Tachometer")

# The full path to the final image.
set(Tachometer_default_full_path_to_image ${Tachometer_default_output_dir}/${Tachometer_default_image_name})

# Potential output file extensions
set(output_extensions
    .hex
    .hxl
    .mum
    .o
    .sdb
    .sym
    .cmf)
list(TRANSFORM output_extensions PREPEND "${Tachometer_default_output_dir}/${Tachometer_default_image_base_name}")
