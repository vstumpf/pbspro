message(src/cmds)

add_subdirectory(scripts)


# Define the Commands interface library, all commands will inherit properties
# from the Commands interface library.

add_library(Commands INTERFACE)
target_include_directories(Commands INTERFACE "${CMAKE_SOURCE_DIR}/src/include")
target_compile_options(Commands INTERFACE "${KRB5_CFLAGS}")
target_link_libraries(Commands
    INTERFACE
        ${CMAKE_DL_LIBS}
        pbs
        net
        sec
        util
        Threads::Threads
        ${KRB5_LIBS}
)

target_sources(Commands INTERFACE ${CMAKE_SOURCE_DIR}/src/lib/Libcmds/cmds_common.c)

add_executable(pbsdsh)
target_link_libraries(pbsdsh PRIVATE Commands)
target_sources(pbsdsh PRIVATE pbsdsh.c)

add_executable(pbsnodes)
target_link_libraries(pbsnodes PRIVATE Commands)
target_sources(pbsnodes PRIVATE pbsnodes.c)

add_executable(pbs_attach)
target_link_libraries(pbs_attach PRIVATE Commands)
target_sources(pbs_attach PRIVATE pbs_attach.c)

add_executable(pbs_demux)
target_link_libraries(pbs_demux PRIVATE Commands)
target_sources(pbs_demux PRIVATE pbs_demux.c)

add_executable(qstat)
target_link_libraries(qstat PRIVATE Commands)
target_sources(qstat PRIVATE qstat.c)


add_executable(qsub)
target_link_libraries(qsub PRIVATE Commands)
target_sources(qsub PRIVATE qsub.c)