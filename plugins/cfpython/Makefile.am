plugindir=$(pkglibdir)/plugins

SUBDIRS = include

if PYTHON_PLUGIN
noinst_PROGRAMS = pyi-generate
plugin_LTLIBRARIES = cfpython.la
else
plugin_LTLIBRARIES =
endif
cfpython_la_SOURCES = cfpython.cpp \
	cfpython_archetype.cpp \
	cfpython_object.cpp \
	cfpython_map.cpp \
	cfpython_party.cpp \
	cfpython_region.cpp \
	cjson.cpp \
	../common/plugin_common.cpp

cfpython_la_CPSOURCES = cfpython.cpp \
	cfpython_archetype.cpp \
	cfpython_object.cpp \
	cfpython_party.cpp \
	cfpython_region.cpp \
	cfpython_map.cpp

cfpython_la_LIBADD = @PYTHON_LIBS@
cfpython_la_LDFLAGS = -module -avoid-version -rdynamic
AM_CPPFLAGS = -I$(srcdir)/include -I$(top_srcdir)/plugins/common/include -I$(top_srcdir)/include @PYTHON_CFLAGS@

pyi_generate_SOURCES = pyi-generate.cpp ${cfpython_la_SOURCES}
pyi_generate_INCLUDES = -I ../include
pyi_generate_CPPFLAGS = $(AM_CPPFLAGS) -O0 -g
pyi_generate_LDFLAGS = @PYTHON_LIBS@
