message(src/lib/Libutil)

add_library(util)
target_compile_options(util PRIVATE ${KRB5_CFLAGS})

target_include_directories(util PRIVATE "${CMAKE_SOURCE_DIR}/src/include")
target_include_directories(util PRIVATE "${LIBICAL_INC}")

target_sources(util PRIVATE
    get_hostname.c
    execvnode_seq_util.c
    pbs_ical.c
    misc_utils.c
    avltree.c
    hook.c
    work_task.c
    entlim.c
    daemon_protect.c
    pbs_array_list.c
    pbs_secrets.c
    pbs_aes_encrypt.c
)

if(UNDOLR_ENABLED)
    target_include_directories(util PRIVATE "${LIBUNDOLR_INC}")
    target_sources(util PRIVATE undolr.c)
endif(UNDOLR_ENABLED)
