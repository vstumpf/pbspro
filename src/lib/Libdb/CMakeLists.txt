message(src/lib/Libdb)
add_library(db)

target_compile_options(db PRIVATE ${KRB5_CFLAGS})

target_include_directories(db PRIVATE "${CMAKE_SOURCE_DIR}/src/include")
target_include_directories(db PRIVATE "${database_inc}")

target_sources(db PRIVATE
    db_postgres.h
    db_postgres_impl.c
    db_postgres_common.c
    db_postgres_attr.c
    db_postgres_job.c
    db_postgres_resv.c
    db_postgres_svr.c
    db_postgres_que.c
    db_postgres_node.c
    db_postgres_sched.c
)
