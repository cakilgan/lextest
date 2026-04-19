set(_LX_DIR ${CMAKE_CURRENT_LIST_DIR}/..)

function(lx_add_test target_name)
    add_executable(${target_name}
        ${ARGN}
        ${_LX_DIR}/entrypoint.cpp
    )

    target_link_libraries(${target_name} PRIVATE lextest)
endfunction()