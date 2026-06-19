include("${CMAKE_CURRENT_LIST_DIR}/rule.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/file.cmake")

set(BaseTray_default_library_list )

# Handle files with suffix (s|as|asm|AS|ASM|As|aS|Asm), for group default-XC8
if(BaseTray_default_default_XC8_FILE_TYPE_assemble)
add_library(BaseTray_default_default_XC8_assemble OBJECT ${BaseTray_default_default_XC8_FILE_TYPE_assemble})
    BaseTray_default_default_XC8_assemble_rule(BaseTray_default_default_XC8_assemble)
    list(APPEND BaseTray_default_library_list "$<TARGET_OBJECTS:BaseTray_default_default_XC8_assemble>")

endif()

# Handle files with suffix S, for group default-XC8
if(BaseTray_default_default_XC8_FILE_TYPE_assemblePreprocess)
add_library(BaseTray_default_default_XC8_assemblePreprocess OBJECT ${BaseTray_default_default_XC8_FILE_TYPE_assemblePreprocess})
    BaseTray_default_default_XC8_assemblePreprocess_rule(BaseTray_default_default_XC8_assemblePreprocess)
    list(APPEND BaseTray_default_library_list "$<TARGET_OBJECTS:BaseTray_default_default_XC8_assemblePreprocess>")

endif()

# Handle files with suffix [cC], for group default-XC8
if(BaseTray_default_default_XC8_FILE_TYPE_compile)
add_library(BaseTray_default_default_XC8_compile OBJECT ${BaseTray_default_default_XC8_FILE_TYPE_compile})
    BaseTray_default_default_XC8_compile_rule(BaseTray_default_default_XC8_compile)
    list(APPEND BaseTray_default_library_list "$<TARGET_OBJECTS:BaseTray_default_default_XC8_compile>")

endif()

# Handle files with suffix elf, for group default-XC8
if(BaseTray_default_default_XC8_FILE_TYPE_objcopy_avr)
add_library(BaseTray_default_default_XC8_objcopy_avr OBJECT ${BaseTray_default_default_XC8_FILE_TYPE_objcopy_avr})
    BaseTray_default_default_XC8_objcopy_avr_rule(BaseTray_default_default_XC8_objcopy_avr)
    list(APPEND BaseTray_default_library_list "$<TARGET_OBJECTS:BaseTray_default_default_XC8_objcopy_avr>")

endif()


# Main target for this project
add_executable(BaseTray_default_image_EZAOwJq4 ${BaseTray_default_library_list})

set_target_properties(BaseTray_default_image_EZAOwJq4 PROPERTIES
    OUTPUT_NAME "default"
    SUFFIX ".elf"
    ADDITIONAL_CLEAN_FILES "${output_extensions}"
    RUNTIME_OUTPUT_DIRECTORY "${BaseTray_default_output_dir}")
target_link_libraries(BaseTray_default_image_EZAOwJq4 PRIVATE ${BaseTray_default_default_XC8_FILE_TYPE_link})

#Add objcopy steps
BaseTray_default_objcopy_avr_rule(BaseTray_default_image_EZAOwJq4)
# Add the link options from the rule file.
BaseTray_default_link_rule( BaseTray_default_image_EZAOwJq4)


