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

add_library(CommonCommands INTERFACE)
target_sources(CommonCommands INTERFACE ${CMAKE_SOURCE_DIR}/src/lib/Libcmds/cmds_common.c)
target_link_libraries(CommonCommands INTERFACE Commands)

add_executable(pbsdsh)
target_link_libraries(pbsdsh PRIVATE Commands)
target_sources(pbsdsh PRIVATE pbsdsh.c)

add_executable(pbsnodes)
target_link_libraries(pbsnodes PRIVATE CommonCommands)
target_sources(pbsnodes PRIVATE pbsnodes.c)

add_executable(pbs_attach)
target_link_libraries(pbs_attach PRIVATE CommonCommands)
target_sources(pbs_attach PRIVATE pbs_attach.c)

add_executable(pbs_demux)
target_link_libraries(pbs_demux PRIVATE Commands)
target_sources(pbs_demux PRIVATE pbs_demux.c)


add_executable(pbs_ds_password)
target_include_directories(pbs_ds_password PRIVATE ${DATABASE_INCLUDE})
target_link_libraries(pbs_ds_password
    PRIVATE
        CommonCommands
        db
        ${DATABASE_LIBRARIES}
        ssl
        crypto
)
target_sources(pbs_ds_password PRIVATE pbs_ds_password.c)

add_executable(pbs_password)
target_link_libraries(pbs_password PRIVATE CommonCommands)
target_sources(pbs_password PRIVATE pbs_password.c)

add_executable(pbs_tmrsh)
target_link_libraries(pbs_tmrsh PRIVATE Commands)
target_sources(pbs_tmrsh PRIVATE pbs_tmrsh.c)

add_executable(pbs_ralter)
target_link_libraries(pbs_ralter PRIVATE CommonCommands)
target_sources(pbs_ralter PRIVATE pbs_ralter.c)

add_executable(pbs_rdel)
target_link_libraries(pbs_rdel PRIVATE CommonCommands)
target_sources(pbs_rdel PRIVATE pbs_rdel.c)

add_executable(pbs_rstat)
target_link_libraries(pbs_rstat PRIVATE CommonCommands)
target_sources(pbs_rstat PRIVATE pbs_rstat.c)

add_executable(pbs_rsub)
target_link_libraries(pbs_rsub PRIVATE CommonCommands)
target_sources(pbs_rsub PRIVATE pbs_rsub.c)

add_executable(pbs_migrate_users)
target_link_libraries(pbs_migrate_users PRIVATE CommonCommands)
target_sources(pbs_migrate_users PRIVATE pbs_migrate_users.c)

add_executable(pbs_release_nodes)
target_link_libraries(pbs_release_nodes PRIVATE CommonCommands)
target_sources(pbs_release_nodes PRIVATE pbs_release_nodes.c)

add_executable(qalter)
target_link_libraries(qalter PRIVATE CommonCommands)
target_sources(qalter PRIVATE qalter.c)

add_executable(qdel)
target_link_libraries(qdel PRIVATE CommonCommands)
target_sources(qdel PRIVATE qdel.c)

add_executable(qdisable)
target_link_libraries(qdisable PRIVATE CommonCommands)
target_sources(qdisable PRIVATE qdisable.c)

add_executable(qenable)
target_link_libraries(qenable PRIVATE CommonCommands)
target_sources(qenable PRIVATE qenable.c)

add_executable(qhold)
target_link_libraries(qhold PRIVATE CommonCommands)
target_sources(qhold PRIVATE qhold.c)

add_executable(qmgr)
target_link_libraries(qmgr
    PRIVATE
        CommonCommands
)
target_sources(qmgr PRIVATE qmgr.c)


add_executable(qstat)
target_link_libraries(qstat PRIVATE CommonCommands)
target_sources(qstat PRIVATE qstat.c)

add_executable(qsub)
target_link_libraries(qsub PRIVATE CommonCommands)
target_sources(qsub PRIVATE qsub.c)