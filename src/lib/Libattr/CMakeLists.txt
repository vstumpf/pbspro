message(src/lib/Libattr)


add_library(attr)
target_compile_options(attr PRIVATE ${KRB5_CFLAGS})

target_include_directories(attr PRIVATE "${CMAKE_SOURCE_DIR}/src/include")

target_sources(attr PRIVATE
    attr_atomic.c
    attr_fn_acl.c
    attr_fn_arst.c
    attr_fn_b.c
    attr_fn_c.c
    attr_fn_entlim.c
    attr_fn_f.c
    attr_fn_hold.c
    attr_fn_intr.c
    attr_fn_l.c
    attr_fn_ll.c
    attr_fn_resc.c
    attr_fn_size.c
    attr_fn_str.c
    attr_fn_time.c
    attr_fn_unkn.c
    attr_func.c
    attr_node_func.c
    attr_resc_func.c
    job_attr_def.c
    Long_.c
    LTostr.c
    node_attr_def.c
    queue_attr_def.c
    resc_def_all.c
    resc_map.c
    resv_attr_def.c
    sched_attr_def.c
    strToL.c
    strTouL.c
    svr_attr_def.c
    uLTostr.c
)

set(PBS_ATTR_PARSER "${CMAKE_SOURCE_DIR}/buildutils/attr_parser.py")
set(PBS_LIBATTR "${CMAKE_SOURCE_DIR}/src/lib/Libattr")

add_custom_command(
    OUTPUT job_attr_def.c
    COMMAND
        ${Python3_EXECUTABLE} ${PBS_ATTR_PARSER}
            -m ${PBS_LIBATTR}/master_job_attr_def.xml
            -e /dev/null
            -s job_attr_def.c
            -a job
    DEPENDS
        ${PBS_LIBATTR}/master_job_attr_def.xml
        ${PBS_ATTR_PARSER}
)

add_custom_command(
    OUTPUT svr_attr_def.c
    COMMAND
        ${Python3_EXECUTABLE} ${PBS_ATTR_PARSER}
        -m ${PBS_LIBATTR}/master_svr_attr_def.xml
        -e /dev/null
        -s svr_attr_def.c
        -a server
    DEPENDS
        ${PBS_LIBATTR}/master_svr_attr_def.xml
        ${PBS_ATTR_PARSER}
)

add_custom_command(
    OUTPUT queue_attr_def.c
    COMMAND
        ${Python3_EXECUTABLE} ${PBS_ATTR_PARSER}
            -m ${PBS_LIBATTR}/master_queue_attr_def.xml
            -e /dev/null
            -s queue_attr_def.c
            -a queue
    DEPENDS
        ${PBS_LIBATTR}/master_queue_attr_def.xml
        ${PBS_ATTR_PARSER}
)

add_custom_command(
    OUTPUT node_attr_def.c
    COMMAND
        ${Python3_EXECUTABLE} ${PBS_ATTR_PARSER}
            -m ${PBS_LIBATTR}/master_node_attr_def.xml
            -e /dev/null
            -s node_attr_def.c
            -a node
    DEPENDS
        ${PBS_LIBATTR}/master_node_attr_def.xml
        ${PBS_ATTR_PARSER}
)

add_custom_command(
    OUTPUT sched_attr_def.c
    COMMAND
        ${Python3_EXECUTABLE} ${PBS_ATTR_PARSER}
            -m ${PBS_LIBATTR}/master_sched_attr_def.xml
            -e /dev/null
            -s sched_attr_def.c
            -a sched
    DEPENDS
        ${PBS_LIBATTR}/master_sched_attr_def.xml
        ${PBS_ATTR_PARSER}
)

add_custom_command(
    OUTPUT resv_attr_def.c
    COMMAND
        ${Python3_EXECUTABLE} ${PBS_ATTR_PARSER}
            -m ${PBS_LIBATTR}/master_resv_attr_def.xml
            -e /dev/null
            -s resv_attr_def.c
            -a resv
    DEPENDS
        ${PBS_LIBATTR}/master_resv_attr_def.xml
        ${PBS_ATTR_PARSER}
)

add_custom_command(
    OUTPUT resc_def_all.c
    COMMAND
        ${Python3_EXECUTABLE} ${PBS_ATTR_PARSER}
            -m ${PBS_LIBATTR}/master_resc_def_all.xml
            -e /dev/null
            -s resc_def_all.c
            -a resc
    DEPENDS
        ${PBS_LIBATTR}/master_resc_def_all.xml
        ${PBS_ATTR_PARSER}
)


# EXTRA_DIST = \
# 	master_job_attr_def.xml \
# 	master_node_attr_def.xml \
# 	master_queue_attr_def.xml \
# 	master_resc_def_all.xml \
# 	master_resv_attr_def.xml \
# 	master_sched_attr_def.xml \
# 	master_svr_attr_def.xml

# CLEANFILES = \
# 	job_attr_def.c \
# 	node_attr_def.c \
# 	queue_attr_def.c \
# 	resc_def_all.c \
# 	resv_attr_def.c \
# 	sched_attr_def.c \
# 	svr_attr_def.c
