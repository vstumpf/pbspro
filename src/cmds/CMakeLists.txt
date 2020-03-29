message(src/cmds)

add_subdirectory(scripts)


add_executable(qstat)

set(COMMON_CFLAGS "${KRB5_CFLAGS}")

target_include_directories(qstat PRIVATE "${CMAKE_SOURCE_DIR}/src/include")
target_compile_options(qstat PRIVATE ${COMMON_CFLAGS})

target_link_libraries(qstat
    dl
    pbs
    net
    sec
    util
    Threads::Threads
    # socket
    ${KRB5_LIBS}
)

target_sources(qstat PRIVATE
    ${CMAKE_SOURCE_DIR}/src/lib/Libcmds/cmds_common.c
    qstat.c

)
