global_h_depends = \
	account_char.h \
	artifact.h \
	attack.h \
	autoconf.h \
	book.h \
	bufferreader.h \
	commands.h \
	compat.h \
	config.h \
	define.h \
	dialog.h \
	events.h \
	face.h \
	god.h \
	libproto.h \
	living.h \
	logger.h \
	map.h \
	material.h \
	microtar.h \
	newserver.h \
	ob_methods.h \
	ob_types.h \
	object.h \
	party.h \
	player.h \
	recipe.h \
	shared/newclient.h \
	skills.h \
	sockproto.h \
	spells.h \
	sstring.h \
	stringbuffer.h \
	tod.h \
	treasure.h \
	typesproto.h

noinst_HEADERS = \
	global.h \
	image.h \
	loader.h \
	metaserver2.h \
	output_file.h \
	path.h \
	plugin.h \
	random_maps/expand2x.h \
	random_maps/maze_gen.h \
	random_maps/random_map.h \
	random_maps/room_gen.h \
	random_maps/rproto.h \
	re-cmp.h \
	server.h \
	shop.h \
	shstr.h \
	sounds.h \
	sproto.h \
	svnversion.h \
	timers.h \
	$(global_h_depends)

BUILT_SOURCES = svnversion.h

CLEANFILES = svnversion.h

svnversion.h: FORCE
	OUTPUT_DATA='/* Auto-generated at build time. */'; \
	if test -n "$(GIT)" -a -d "$(top_srcdir)/.git"; \
	then VER_STR=`$(GIT) -C "$(top_srcdir)" describe --tags`; OUTPUT_DATA=`echo "$$OUTPUT_DATA"; echo "#define SVN_REV \"$$VER_STR\""`; \
	else OUTPUT_DATA=`echo "$$OUTPUT_DATA"; echo '#define SVN_REV "unknown"'`; \
	fi; \
	if test ! -e svnversion.h; then \
		echo "$$OUTPUT_DATA" > svnversion.h; \
	elif test "$$OUTPUT_DATA" != "`cat svnversion.h`"; then \
		echo "$$OUTPUT_DATA" > svnversion.h; \
	fi

FORCE:

if GLOBAL_PCH
BUILT_SOURCES += global.h.gch
CLEANFILES += global.h.gch

AM_CPPFLAGS = -I$(top_srcdir)/include -DDATADIR=\"$(pkgdatadir)\" -DCONFDIR=\"$(pkgconfdir)\" \
	      -DLIBDIR=\"$(pkglibdir)\" -DLOCALDIR=\"$(pkgstatedir)\" \
		  -x c++-header -fPIC

global.h.gch: global.h $(global_h_depends)
	$(CXX) $(AM_CPPFLAGS) $(CXXFLAGS) global.h -o global.h.gch
endif
