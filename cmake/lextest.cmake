function(lx_add_tests)
    foreach(source IN LISTS ARGN)
        get_filename_component(target_name ${source} NAME_WE)

        add_executable(${target_name}
            ${source}
            ${LEXTEST_SOURCE_DIR}/entrypoint.cpp
        )
        target_link_libraries(${target_name} PRIVATE lextest)
    endforeach()
endfunction()
