pkglibexec_PROGRAMS = random_map

random_map_SOURCES = main.cpp

noinst_LIBRARIES = librandom_map.a

librandom_map_a_SOURCES = \
	decor.cpp \
	door.cpp \
	exit.cpp \
	expand2x.cpp \
	floor.cpp \
	maze_gen.cpp \
	monster.cpp \
	random_map.cpp \
	reader.cpp \
	rogue_layout.cpp \
	room_gen_onion.cpp \
	room_gen_spiral.cpp \
	snake.cpp \
	special.cpp \
	square_spiral.cpp \
	style.cpp \
	treasure.cpp \
	wall.cpp

EXTRA_DIST = \
	README \
	main.cpp

reader.cpp: reader.l
	$(LEX) -P rm -o reader.cpp $(top_srcdir)/random_maps/reader.l

AM_CPPFLAGS = -I$(top_srcdir)/include -I${top_srcdir}/include/random_maps

LEX_OUTPUT_ROOT = lex.rmap
AM_LFLAGS = -Prmap -i

random_map_LDADD = ../common/libcross.a librandom_map.a

CLEANFILES = reader.cpp
