message(src/lib/Liblog)
add_library(log)

target_compile_options(log PRIVATE ${KRB5_CFLAGS})

target_include_directories(log PRIVATE "${CMAKE_SOURCE_DIR}/src/include")

target_sources(log PRIVATE
    chk_file_sec.c
    log_event.c
    pbs_log.c
    pbs_messages.c
    setup_env.c
)