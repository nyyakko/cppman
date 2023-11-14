add_subdirectory(source)

add_executable(${PROJECT_NAME} ${cppman_SourceFiles})

enable_static_analyzers(${PROJECT_NAME} ON)

target_include_directories(${PROJECT_NAME} 
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include" 
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include/cppman"
)

set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 23)
target_compile_options(${PROJECT_NAME} PRIVATE 
    -Werror
    -Wall
    -Wextra
    -Wshadow
    -Wnon-virtual-dtor
    -Wold-style-cast
    -Wcast-align
    -Wunused
    -Woverloaded-virtual
    -Wpedantic
    -Wconversion
    -Wsign-conversion
    -Wnull-dereference
    -Wdouble-promotion
    -Wimplicit-fallthrough
)
target_link_libraries(${PROJECT_NAME} PRIVATE ${cppman_ExternalLibraries})
