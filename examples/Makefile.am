# Relative path from here to uninstalled library
INCLUDE_VG_DIR = $(top_builddir)/include
LIB_VG_DIR = $(top_builddir)/src
LIB_VG = ${LIB_VG_DIR}/libOpenVG.la

# Variables which are common to all example programs
# (Combine variables passed from configure script with
# the local path to uninstalled library)
EXAMPLE_SRCS = test.h test.c
EXAMPLE_CF = @CONFIG_CFLAGS@ -I${INCLUDE_VG_DIR} -I${prefix}/include
EXAMPLE_LA = @CONFIG_LDADD@ ${LIB_VG}
EXAMPLE_LF = @CONFIG_LDFLAGS@ -L${prefix}/lib

EXTRA_DIST = *.jpg *.png

noinst_PROGRAMS =

if BUILD_VGU
noinst_PROGRAMS += test_vgu
endif

if BUILD_DASH
noinst_PROGRAMS += test_dash
endif

if BUILD_LINEAR
noinst_PROGRAMS += test_linear
endif

if BUILD_RADIAL
noinst_PROGRAMS += test_radial
endif

if BUILD_INTERPOLATE
noinst_PROGRAMS += test_interpolate
endif

if BUILD_TIGER
noinst_PROGRAMS += test_tiger
endif

if BUILD_IMAGE
noinst_PROGRAMS += test_image
endif

if BUILD_PATTERN
noinst_PROGRAMS += test_pattern
endif

if BUILD_BLEND
noinst_PROGRAMS += test_blend
endif

test_vgu_SOURCES =\
	${EXAMPLE_SRCS} test_vgu.c

test_dash_SOURCES =\
	${EXAMPLE_SRCS} test_dash.c

test_linear_SOURCES =\
	${EXAMPLE_SRCS} test_linear.c

test_radial_SOURCES =\
	${EXAMPLE_SRCS} test_radial.c

test_interpolate_SOURCES =\
	${EXAMPLE_SRCS} test_interpolate.c

test_tiger_SOURCES =\
	${EXAMPLE_SRCS} test_tiger.c test_tiger_paths.c

test_image_SOURCES =\
	${EXAMPLE_SRCS} test_image.c

test_pattern_SOURCES =\
	${EXAMPLE_SRCS} test_pattern.c

test_blend_SOURCES =\
	${EXAMPLE_SRCS} test_blend.c


test_vgu_CFLAGS = ${EXAMPLE_CF}
test_vgu_LDADD = ${EXAMPLE_LA}
test_vgu_LDFLAGS = ${EXAMPLE_LF}

test_dash_CFLAGS = ${EXAMPLE_CF}
test_dash_LDADD = ${EXAMPLE_LA}
test_dash_LDFLAGS = ${EXAMPLE_LF}

test_linear_CFLAGS = ${EXAMPLE_CF}
test_linear_LDADD = ${EXAMPLE_LA}
test_linear_LDFLAGS = ${EXAMPLE_LF}

test_radial_CFLAGS = ${EXAMPLE_CF}
test_radial_LDADD = ${EXAMPLE_LA}
test_radial_LDFLAGS = ${EXAMPLE_LF}

test_interpolate_CFLAGS = ${EXAMPLE_CF}
test_interpolate_LDADD = ${EXAMPLE_LA}
test_interpolate_LDFLAGS = ${EXAMPLE_LF}

test_tiger_CFLAGS = ${EXAMPLE_CF}
test_tiger_LDADD = ${EXAMPLE_LA}
test_tiger_LDFLAGS = ${EXAMPLE_LF}

test_image_CFLAGS = ${EXAMPLE_CF}
test_image_LDADD = ${EXAMPLE_LA} -ljpeg
test_image_LDFLAGS = ${EXAMPLE_LF}

test_pattern_CFLAGS = ${EXAMPLE_CF}
test_pattern_LDADD = ${EXAMPLE_LA} -ljpeg
test_pattern_LDFLAGS = ${EXAMPLE_LF}

test_blend_CFLAGS = ${EXAMPLE_CF}
test_blend_LDADD = ${EXAMPLE_LA}
test_blend_LDFLAGS = ${EXAMPLE_LF}
