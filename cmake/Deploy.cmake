if(WIN32)
    function(deploy_shared_lib target directory lib)
        set(options TO_BUILD)
        cmake_parse_arguments(DEPLOY_SHARED_LIB "${options}" "" "" ${ARGN})

        message("Searching for ${lib}${CMAKE_SHARED_LIBRARY_SUFFIX}")
        execute_process(
            COMMAND where.exe "${lib}${CMAKE_SHARED_LIBRARY_SUFFIX}"
            OUTPUT_VARIABLE _matches
            OUTPUT_STRIP_TRAILING_WHITESPACE
            RESULT_VARIABLE _res
        )
        if(NOT "${_res}" STREQUAL "0")
            message(FATAL_ERROR "Failed to find ${lib}${CMAKE_SHARED_LIBRARY_SUFFIX}")
        endif()
        string(REGEX REPLACE "\n" ";" _matches "${_matches}")
        list(GET _matches 0 _lib)
        string(REGEX REPLACE "\\\\" "/" _lib "${_lib}")

        if(DEPLOY_SHARED_LIB_TO_BUILD)
            # Deploy into build tree
            add_custom_command(TARGET ${target} POST_BUILD
                COMMAND "${CMAKE_COMMAND}" -E
                    copy_if_different "${_lib}" \"$<TARGET_FILE_DIR:${target}>\"
                COMMENT "Copying ${_lib} to \"$<TARGET_FILE_DIR:${target}>\""
            )
        endif()

        # Deploy into install tree
        install(CODE
        "
        if(\"${directory}\" STREQUAL \"\")
            set(_dest \"\${CMAKE_INSTALL_PREFIX}\")
        else()
            set(_dest \"\${CMAKE_INSTALL_PREFIX}/${directory}\")
        endif()
        message(\"Copying ${_lib} to \${_dest}\")
        execute_process(
            COMMAND \"${CMAKE_COMMAND}\" -E
                copy \"${_lib}\" \"\${_dest}/\"
        )
        "
    )
    endfunction()

    include(${CMAKE_CURRENT_LIST_DIR}/Windeployqt.cmake)

    function(deploy_qt5 target directory)
        windeployqt(${target} "${directory}")
    endfunction()

    function(deploy_cuda target directory)
        set(multiValueArgs COMPONENTS)
        cmake_parse_arguments(DEPLOY_CUDA "" "" "${multiValueArgs}" ${ARGN})

        if(${CMAKE_SIZEOF_VOID_P} EQUAL 8)
            set(CUDA_SUFFIX "64_${CUDA_VERSION_MAJOR}${CUDA_VERSION_MINOR}")
        else()
            set(CUDA_SUFFIX "32_${CUDA_VERSION_MAJOR}${CUDA_VERSION_MINOR}")
        endif()

        foreach(component ${DEPLOY_CUDA_COMPONENTS})
            deploy_shared_lib(${target} "${directory}" "${component}${CUDA_SUFFIX}")
        endforeach()
    endfunction()

    function(deploy_cudnn target directory version)
        if(${CMAKE_SIZEOF_VOID_P} EQUAL 8)
            set(CUDNN_SUFFIX "64_${version}")
        else()
            set(CUDNN_SUFFIX "32_${version}")
        endif()

        deploy_shared_lib(${target} "${directory}" "cudnn${CUDNN_SUFFIX}")
    endfunction()

    function(deploy_opencv target directory)
        set(multiValueArgs COMPONENTS)
        cmake_parse_arguments(DEPLOY_OPENCV "" "" "${multiValueArgs}" ${ARGN})

        set(CV_SUFFIX "${OpenCV_VERSION_MAJOR}${OpenCV_VERSION_MINOR}${OpenCV_VERSION_PATCH}")

        foreach(component ${DEPLOY_OPENCV_COMPONENTS})
            deploy_shared_lib(${target} "${directory}" "opencv_${component}${CV_SUFFIX}")
        endforeach()
    endfunction()
endif()