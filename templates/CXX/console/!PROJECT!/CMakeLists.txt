add_subdirectory(source)

add_executable(${PROJECT_NAME} ${!PROJECT!_SourceFiles})

enable_static_analyzers(${PROJECT_NAME} ON)

target_include_directories(${PROJECT_NAME} 
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include" 
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include/!PROJECT!"
)

set_target_properties(${PROJECT_NAME} PROPERTIES !LANGUAGE!_STANDARD !STANDARD!)
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
target_link_libraries(${PROJECT_NAME} PRIVATE ${!PROJECT!_ExternalLibraries})
