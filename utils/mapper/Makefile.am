AM_CPPFLAGS = -I$(top_srcdir)/include

CLEANFILES = crossloop

if MAPPER
noinst_PROGRAMS = crossfire-mapper
crossfire_mapper_SOURCES = mapper.cpp ../../server/quest.cpp
crossfire_mapper_INCLUDES = -I ${top_builddir}/include
crossfire_mapper_DEPENDENCIES = ${top_builddir}/common/libcross.a
crossfire_mapper_LDADD = ${top_builddir}/common/libcross.a ${top_builddir}/socket/libsocket.a $(GD_LIBS)
$(crossfire_mapper_DEPENDENCIES) :
	for DEP in $(crossfire_mapper_DEPENDENCIES); \
	do \
	  pushd `dirname $${DEP}` >/dev/null; \
	  $(MAKE) `basename $${DEP}`; \
	  popd >/dev/null; \
	done;
endif
