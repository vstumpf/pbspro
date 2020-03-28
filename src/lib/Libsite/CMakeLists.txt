message(src/lib/Libsite)

add_library(site)
target_compile_options(site PRIVATE ${KRB5_CFLAGS})

target_include_directories(site PRIVATE "${CMAKE_SOURCE_DIR}/src/include")
target_include_directories(site PRIVATE "${CMAKE_SOURCE_DIR}/src/resmom/${PBS_MACH}")

target_sources(site PRIVATE
    site_alt_rte.c
    site_mom_chu.c
    site_mom_jst.c
    site_allow_u.c
    site_check_u.c
    site_map_usr.c
    site_mom_ckp.c
)
