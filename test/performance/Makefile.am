noinst_PROGRAMS = perf_mapload perf_mapsave test_mapsave
perf_mapload_SOURCES = perf_mapload.cpp
perf_mapsave_SOURCES = perf_mapsave.cpp
test_mapsave_SOURCES = test_mapsave.cpp

AM_CPPFLAGS = -I$(top_srcdir)/include -DDATADIR=\"$(pkgdatadir)\" -DCONFDIR=\"$(pkgconfdir)\" \
	      -DLIBDIR=\"$(pkglibdir)\" -DLOCALDIR=\"$(pkgstatedir)\" \
	      -DPLUGIN_SUFFIX=\".so\"

SERVER_LDADD = ../../server/libserver.a ../../common/libcross.a ../../random_maps/librandom_map.a \
		../../socket/libsocket.a ../../types/libtypes.a

perf_mapload_LDADD = $(SERVER_LDADD)

perf_mapsave_LDADD = $(SERVER_LDADD)

test_mapsave_LDADD = $(SERVER_LDADD)
