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

$(top_builddir)/types/libtypes.a:
	( cd $(top_builddir)/types ; make libtypes.a )

# ../../logs/unit/server:
# 	(mkdir ../../logs; mkdir ../../logs/unit; mkdir ../../logs/unit/server)

$(top_builddir)/common/libcross.a:
	( cd $(top_builddir)/common ; make libcross.a )

$(top_builddir)/socket/libsocket.a:
	( cd $(top_builddir)/socket ; make libsocket.a )

$(top_builddir)/random_maps/librandom_map.a:
	( cd $(top_builddir)/random_maps ; make librandom_map.a )

$(top_builddir)/server/libserver.a:
	( cd $(top_builddir)/server ; make libserver.a )

LDADD = $(top_builddir)/server/libserver.a \
        $(top_builddir)/common/libcross.a \
        $(top_builddir)/server/libserver.a \
        $(top_builddir)/common/libcross.a \
        $(top_builddir)/random_maps/librandom_map.a \
        $(top_builddir)/socket/libsocket.a \
        $(top_builddir)/types/libtypes.a \
        $(top_builddir)/test/toolkit/libtoolkit_common.a \
        $(top_builddir)/test/toolkit/libtoolkit_server.a
