message(src/scheduler)

add_executable(pbs_sched)
add_executable(pbsfs)

add_library(pbs_schedlib)
set_target_properties(pbs_schedlib
    PROPERTIES
        OUTPUT_NAME "libpbs_sched"
)

add_library(pbs_sched_common INTERFACE)

target_include_directories(pbs_sched_common
    INTERFACE
        "${CMAKE_SOURCE_DIR}/src/include"
        ${ZLIB_INCLUDE_DIRS}
        ${PYTHON_INCLUDE}
)

target_link_libraries(pbs_sched_common
    INTERFACE
        pbs_schedlib
        util
        log
        net
        sec
        pbs
        tpp
        Threads::Threads
        ${PYTHON_LIBRARIES}
        ${ZLIB_LIBRARIES}
        ${LIBICAL_LIBRARIES}
        ${KRB5_LIBS}
)

target_compile_options(pbs_sched_common
    INTERFACE
        ${KRB5_CFLAGS}
        ${PYTHON_LDFLAGS}
        ${DATABASE_LDFLAGS}
)

target_compile_options(pbs_schedlib
    PRIVATE
        ${KRB5_CFLAGS}
)

target_link_libraries(pbs_schedlib
    PRIVATE
        Threads::Threads
        m
)

target_include_directories(pbs_schedlib
    PRIVATE
        "${CMAKE_SOURCE_DIR}/src/include"
        ${ZLIB_INCLUDE_DIRS}
        ${PYTHON_INCLUDE}
)

target_sources(pbs_schedlib
    PRIVATE
        "${CMAKE_SOURCE_DIR}/src/lib/Libpython/shared_python_utils.c"
        buckets.c
        buckets.h
        check.c
        check.h
        config.h
        constant.h
        data_types.h
        dedtime.c
        dedtime.h
        fairshare.c
        fairshare.h
        fifo.c
        fifo.h
        get_4byte.c
        globals.c
        globals.h
        job_info.c
        job_info.h
        limits.c
        misc.c
        misc.h
        multi_threading.c
        multi_threading.h
        node_info.c
        node_info.h
        node_partition.c
        node_partition.h
        parse.c
        parse.h
        pbs_bitmap.c
        pbs_bitmap.h
        prev_job_info.c
        prev_job_info.h
        prime.c
        prime.h
        queue.c
        queue.h
        queue_info.c
        queue_info.h
        range.c
        range.h
        resource.h
        resource_resv.c
        resource_resv.h
        resv_info.c
        resv_info.h
        resource.c
        server_info.c
        server_info.h
        simulate.c
        simulate.h
        sort.c
        sort.h
        state_count.c
        state_count.h
        site_code.c
        site_code.h
        site_data.h
)

target_link_libraries(pbs_sched
    PRIVATE
        pbs_sched_common
        ${LIBUNDOLR_LIBRARIES}
)

target_sources(pbs_sched
    PRIVATE
        pbs_sched.c
)

target_link_libraries(pbsfs
    PRIVATE
        pbs_sched_common
)

target_sources(pbsfs
    PRIVATE
        pbsfs.c
)
