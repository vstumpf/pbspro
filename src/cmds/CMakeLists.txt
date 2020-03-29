message(src/cmds)

add_subdirectory(scripts)


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



add_executable(qstat)
target_link_libraries(qstat PRIVATE Commands)
target_sources(qstat PRIVATE qstat.c)


add_executable(qsub)
target_link_libraries(qsub PRIVATE Commands)
target_sources(qsub PRIVATE qsub.c)