SUBDIRS = include
plugindir=$(pkglibdir)/plugins
noinst_HEADERS =  ../common/include/plugin_common.h
if HAVE_CFNEWSPAPER
plugin_LTLIBRARIES = cfnewspaper.la
else
plugin_LTLIBRARIES =
endif

cfnewspaper_la_SOURCES = cfnewspaper.cpp \
	../common/plugin_common.cpp

cfnewspaper_la_CPSOURCES = cfnewspaper.cpp

cfnewspaper_la_LIBADD = -lsqlite3
cfnewspaper_la_LDFLAGS = -module -avoid-version -rdynamic
AM_CPPFLAGS = -I$(srcdir)/include -I$(srcdir)/../common/include -I$(top_srcdir)/include
