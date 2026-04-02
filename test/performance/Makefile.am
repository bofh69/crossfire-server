noinst_PROGRAMS = perf_mapload perf_mapsave test_mapsave
perf_mapload_SOURCES = perf_mapload.cpp
perf_mapsave_SOURCES = perf_mapsave.cpp
test_mapsave_SOURCES = test_mapsave.cpp

AM_CPPFLAGS = -I$(top_srcdir)/include -DDATADIR=\"$(pkgdatadir)\" -DCONFDIR=\"$(pkgconfdir)\" \
	      -DLIBDIR=\"$(pkglibdir)\" -DLOCALDIR=\"$(pkgstatedir)\" \
	      -DPLUGIN_SUFFIX=\".so\"

AM_LIBTOOLFLAGS= --preserve-dup-deps
LINK = $(LIBTOOL) --tag=CXX $(AM_LIBTOOLFLAGS) $(LIBTOOLFLAGS) \
        --mode=link $(CCLD) $(AM_CFLAGS) $(CFLAGS) $(AM_LDFLAGS) \
       $(LDFLAGS) -o $@

# ../../logs/unit/server:
# 	(mkdir ../../logs; mkdir ../../logs/unit; mkdir ../../logs/unit/server)

$(top_builddir)/common/libcross.la:
	( cd $(top_builddir)/common ; make libcross.la )

$(top_builddir)/random_maps/librandom_map.la:
	( cd $(top_builddir)/random_maps ; make librandom_map.la )

$(top_builddir)/server/libserver.la:
	( cd $(top_builddir)/server ; make libserver.la )

LDADD = $(top_builddir)/server/libserver.la \
        $(top_builddir)/common/libcross.la \
        $(top_builddir)/random_maps/librandom_map.la \
        $(top_builddir)/test/toolkit/libtoolkit_common.a \
        $(top_builddir)/test/toolkit/libtoolkit_server.a
