set(_LX_DIR ${CMAKE_CURRENT_LIST_DIR}/..)
function(lx_add_tests)
    foreach(source IN LISTS ARGN)
        get_filename_component(target_name ${source} NAME_WE)

        add_executable(${target_name}
            ${source}
            ${_LX_DIR}/entrypoint.cpp
        )
        target_link_libraries(${target_name} PRIVATE lextest)
    endforeach()
endfunction()
