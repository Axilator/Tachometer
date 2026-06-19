include("${CMAKE_CURRENT_LIST_DIR}/rule.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/file.cmake")

set(Tachometer_default_library_list )

# Handle files with suffix (s|as|asm|AS|ASM|As|aS|Asm), for group default-XC8
if(Tachometer_default_default_XC8_FILE_TYPE_assemble)
add_library(Tachometer_default_default_XC8_assemble OBJECT ${Tachometer_default_default_XC8_FILE_TYPE_assemble})
    Tachometer_default_default_XC8_assemble_rule(Tachometer_default_default_XC8_assemble)
    list(APPEND Tachometer_default_library_list "$<TARGET_OBJECTS:Tachometer_default_default_XC8_assemble>")

endif()

# Handle files with suffix S, for group default-XC8
if(Tachometer_default_default_XC8_FILE_TYPE_assemblePreprocess)
add_library(Tachometer_default_default_XC8_assemblePreprocess OBJECT ${Tachometer_default_default_XC8_FILE_TYPE_assemblePreprocess})
    Tachometer_default_default_XC8_assemblePreprocess_rule(Tachometer_default_default_XC8_assemblePreprocess)
    list(APPEND Tachometer_default_library_list "$<TARGET_OBJECTS:Tachometer_default_default_XC8_assemblePreprocess>")

endif()

# Handle files with suffix [cC], for group default-XC8
if(Tachometer_default_default_XC8_FILE_TYPE_compile)
add_library(Tachometer_default_default_XC8_compile OBJECT ${Tachometer_default_default_XC8_FILE_TYPE_compile})
    Tachometer_default_default_XC8_compile_rule(Tachometer_default_default_XC8_compile)
    list(APPEND Tachometer_default_library_list "$<TARGET_OBJECTS:Tachometer_default_default_XC8_compile>")

endif()

# Handle files with suffix elf, for group default-XC8
if(Tachometer_default_default_XC8_FILE_TYPE_objcopy_avr)
add_library(Tachometer_default_default_XC8_objcopy_avr OBJECT ${Tachometer_default_default_XC8_FILE_TYPE_objcopy_avr})
    Tachometer_default_default_XC8_objcopy_avr_rule(Tachometer_default_default_XC8_objcopy_avr)
    list(APPEND Tachometer_default_library_list "$<TARGET_OBJECTS:Tachometer_default_default_XC8_objcopy_avr>")

endif()


# Main target for this project
add_executable(Tachometer_default_image_opd1MajW ${Tachometer_default_library_list})

set_target_properties(Tachometer_default_image_opd1MajW PROPERTIES
    OUTPUT_NAME "default"
    SUFFIX ".elf"
    ADDITIONAL_CLEAN_FILES "${output_extensions}"
    RUNTIME_OUTPUT_DIRECTORY "${Tachometer_default_output_dir}")
target_link_libraries(Tachometer_default_image_opd1MajW PRIVATE ${Tachometer_default_default_XC8_FILE_TYPE_link})

#Add objcopy steps
Tachometer_default_objcopy_avr_rule(Tachometer_default_image_opd1MajW)
# Add the link options from the rule file.
Tachometer_default_link_rule( Tachometer_default_image_opd1MajW)


