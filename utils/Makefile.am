dist_noinst_SCRIPTS = mktable.script player_dl.html crossloop.web metaserver
dist_bin_SCRIPTS = cfdb_convert crossloop player_dl.pl

noinst_PROGRAMS = gridarta-types-convert itemdump
gridarta_types_convert_SOURCES = gridarta-types-convert.cpp
itemdump_SOURCES = itemdump.cpp

itemdump_LDADD = $(top_builddir)/server/libserver.la \
			$(top_builddir)/common/libcross.la \
			$(top_builddir)/random_maps/librandom_map.la \
			$(top_builddir)/server/libserver.la

EXTRA_DIST =
AM_CPPFLAGS = -I$(top_srcdir)/include

CLEANFILES = crossloop

SUBDIRS = mapper
