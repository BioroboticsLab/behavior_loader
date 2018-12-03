function(install_headers)
    cmake_parse_arguments(ARGS "" "DESTINATION" "HEADERS" ${ARGN})

    foreach(file ${ARGS_HEADERS})
        get_filename_component(install_path ${file} DIRECTORY)
        install(FILES ${file} DESTINATION "${ARGS_DESTINATION}/${install_path}")
    endforeach()
endfunction()
