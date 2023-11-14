include(static_analyzers/clang_tidy)
include(static_analyzers/cppcheck)

function(enable_static_analyzers target WARNING_AS_ERRORS)
    enable_clang_tidy(${target} ${WARNING_AS_ERRORS})
    enable_cppcheck(${WARNING_AS_ERRORS} "")
endfunction()