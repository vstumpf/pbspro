message(src)

# TODO: implement KRB5_CFLAGS
SET(KRB5_CFLAGS "")
# TODO: implement KRB5_LIBS
SET(KRB5_LIBS "")
# TODO: implement DATABASE_INC
SET(DATABASE_INCLUDE "")
# TODO: implement PBS_MACH
SET(PBS_MACH "linux")
# TODO: implement LIBICAL_INC
SET(LIBICAL_INC "")
# TODO: LIBUNDOLR_INC
SET(LIBUNDOLR_INC "")


# TODO: impelment UNDOLR_ENABLED
SET(UNDOLR_ENABLED "")
# TODO: implement KRB5_ENABLED
SET(KRB5_ENABLED "")


# TODO: implement PYTHON_INCLUDE
SET(PYTHON_INCLUDE "")

find_package(Python3 REQUIRED COMPONENTS Interpreter Development)
message("Py3 Found? ${Python3_FOUND}")
message("includedirs are ${Python3_INCLUDE_DIRS}")
message("python3 path ${Python3_EXECUTABLE}")
SET(PYTHON_INCLUDE "${Python3_INCLUDE_DIRS}")

set (THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)



# TODO: noinst_LIBRARIES, wtf is that?
# TODO: lib_LTLIBRARIES, wtf is that?


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

