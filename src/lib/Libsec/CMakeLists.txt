message(src/lib/Libsec)

add_library(sec)
target_compile_options(sec PRIVATE ${KRB5_CFLAGS})

target_include_directories(sec PRIVATE "${CMAKE_SOURCE_DIR}/src/include")

target_sources(sec PRIVATE
    cs_standard.c
)
