message(src)

# TODO: implement KRB5_CFLAGS
SET(KRB5_CFLAGS "")
# TODO: implement KRB5_LIBS
SET(KRB5_LIBS "")
# TODO: implement DATABASE_INC
SET(DATABASE_INCLUDE "")


# TODO: implement PYTHON_INCLUDE
SET(PYTHON_INCLUDE "")

find_package(Python3 REQUIRED COMPONENTS Development)
message("Py3 Found? ${Python3_FOUND}")
message("dirs are ${Python3_INCLUDE_DIRS}")
SET(PYTHON_INCLUDE "${Python3_INCLUDE_DIRS}")



# TODO: noinst_LIBRARIES, wtf is that?


# add_subdirectory(cmds)
# add_subdirectory(hooks)
# add_subdirectory(iff)
# add_subdirectory(include)
add_subdirectory(lib)
# add_subdirectory(modules)
# add_subdirectory(mom_rcp)
# add_subdirectory(resmom)
# add_subdirectory(scheduler)
# add_subdirectory(server)
# add_subdirectory(tools)
# add_subdirectory(unsupported)

