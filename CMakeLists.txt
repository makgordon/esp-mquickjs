set(srcs "esp.c" "mquickjs.c" "dtoa.c" "libm.c" "cutils.c")

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS "include")