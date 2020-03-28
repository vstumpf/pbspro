message(src/lib/Libtpp)

add_library(tpp)
target_compile_options(tpp PRIVATE ${KRB5_CFLAGS})

target_include_directories(tpp PRIVATE "${CMAKE_SOURCE_DIR}/src/include")

target_sources(tpp PRIVATE
    tpp_internal.h
    tpp_client.c
    tpp_em.c
    tpp_platform.c
    tpp_router.c
    tpp_transport.c
    tpp_util.c
)
