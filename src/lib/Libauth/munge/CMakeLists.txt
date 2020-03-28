message(src/lib/Libauth/munge)

add_library(auth_munge)

target_include_directories(auth_munge PRIVATE "${CMAKE_SOURCE_DIR}/src/include")

target_link_libraries(auth_munge PRIVATE ${KRB5_LIBS})

target_link_options(auth_munge PRIVATE "-version-info 0:0:0")

target_sources(auth_munge PRIVATE
    munge_supp.c
)