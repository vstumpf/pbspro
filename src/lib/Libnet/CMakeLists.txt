message(src/lib/Libnet)
add_library(net)

target_compile_options(net PRIVATE ${KRB5_CFLAGS})

target_include_directories(net PRIVATE "${CMAKE_SOURCE_DIR}/src/include")

target_sources(net PRIVATE
    get_hostaddr.c
    net_client.c
    net_server.c
    net_set_clse.c
    port_forwarding.c
    hnls.c
)
