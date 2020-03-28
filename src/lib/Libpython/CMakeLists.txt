message(src/lib/Libpython)


add_library(pbspython)

target_compile_options(pbspython PRIVATE ${KRB5_CFLAGS})

target_include_directories(pbspython PRIVATE "${CMAKE_SOURCE_DIR}/src/include")
target_include_directories(pbspython PRIVATE "${PYTHON_INCLUDE}")

target_sources(pbspython PRIVATE
    shared_python_utils.c
    common_python_utils.c
    pbs_ifl_wrap.c
    pbs_python_external.c
    pbs_python_svr_external.c
    module_pbs_v1.c
    pbs_python_svr_internal.c
    pbs_python_svr_size_type.c
    pbs_python_import_types.c
)

add_library(pbspython_svr)

target_compile_options(pbspython_svr PRIVATE ${KRB5_CFLAGS})
target_compile_definitions(pbspython_svr PRIVATE "LIBPYTHONSVR")

target_include_directories(pbspython_svr PRIVATE "${CMAKE_SOURCE_DIR}/src/include")
target_include_directories(pbspython_svr PRIVATE "${PYTHON_INCLUDE}")

target_sources(pbspython_svr PRIVATE
    shared_python_utils.c
    common_python_utils.c
    pbs_ifl_wrap.c
    pbs_python_external.c
    pbs_python_svr_external.c
    module_pbs_v1.c
    pbs_python_svr_internal.c
    pbs_python_svr_size_type.c
    pbs_python_import_types.c
)