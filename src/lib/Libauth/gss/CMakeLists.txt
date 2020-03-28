message(src/lib/Libauth/gss)

if(KRB5_ENABLED)
    add_library(auth_gss)
    target_compile_options(auth_gss PRIVATE ${KRB5_CFLAGS})

    target_include_directories(auth_gss PRIVATE "${CMAKE_SOURCE_DIR}/src/include")

    target_link_libraries(auth_gss PRIVATE ${KRB5_LIBS})
    target_link_libraries(auth_gss PRIVATE Threads::Threads)
    target_link_options(auth_gss PRIVATE "-version-info 0:0:0")

    target_sources(auth_gss PRIVATE
        pbs_gss.c
    )

endif(KRB5_ENABLED)