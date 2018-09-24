macro(VSSourceGroups ROOT_PATH)
    file(
        GLOB_RECURSE sources 
        LIST_DIRECTORIES false
        "${ROOT_PATH}/*.c*"
        "${ROOT_PATH}/*.h*"
        "${ROOT_PATH}/*.ui*"
    )

    foreach(source IN ITEMS ${sources})
        get_filename_component(path "${source}" PATH)
        file(RELATIVE_PATH rel_path "${ROOT_PATH}" "${path}")
        string(REPLACE "/" "\\" group_path "${rel_path}")
        source_group("${group_path}" FILES "${source}")
    endforeach()
endmacro()