add_library(pm STATIC
    ${PROJECT_SOURCE_DIR}/src/pm.h
    ${PROJECT_SOURCE_DIR}/src/pm.c
)

target_include_directories(pm PUBLIC
    ${PROJECT_SOURCE_DIR}/src
)

set_target_properties(pm PROPERTIES
    C_STANDARD 11
    C_STANDARD_REQUIRED ON
    C_EXTENSIONS OFF
)

function(add_pm_test name)
    add_executable("test-${name}"
        "${name}.c"
    )
    target_link_libraries("test-${name}" PRIVATE
        pm
    )
    add_test(
        NAME    "${name}"
        COMMAND "test-${name}"
    )
endfunction()
