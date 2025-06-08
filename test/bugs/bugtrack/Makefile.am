#note all .a are here twice for compilation problems
AM_LIBTOOLFLAGS= --preserve-dup-deps
LINK = $(LIBTOOL) --tag=CC $(AM_LIBTOOLFLAGS) $(LIBTOOLFLAGS) \
        --mode=link $(CCLD) $(AM_CFLAGS) $(CFLAGS) $(AM_LDFLAGS) \
       $(LDFLAGS) -o $@

LDADD = $(top_builddir)/server/libserver.la \
        $(top_builddir)/common/libcross.la \
        $(top_builddir)/random_maps/librandom_map.la \
        $(top_builddir)/test/toolkit/libtoolkit_common.a \
        @CHECK_LIBS@

# ../../logs/unit/server:
# 	(mkdir ../../logs; mkdir ../../logs/unit; mkdir ../../logs/unit/server)

$(top_builddir)/common/libcross.la:
	( cd $(top_builddir)/common ; make libcross.la )

$(top_builddir)/random_maps/librandom_map.la:
	( cd $(top_builddir)/random_maps ; make librandom_map.la )

$(top_builddir)/server/libserver.la:
	( cd $(top_builddir)/server ; make libserver.la )

$(top_builddir)/test/toolkit/libtoolkit_common.a:
	( cd $(top_builddir)/test/toolkit/; make libtoolkit_common.a)

check_1727944_SOURCES = check_1727944.cpp
check_2879249_SOURCES = check_2879249.cpp
check_883_SOURCES = check_883.cpp
check_duplicate_skills_SOURCES = check_duplicate_skills.cpp
check_free_objects_SOURCES = check_free_objects.cpp
check_weight_reduction_SOURCES = check_weight_reduction.cpp

TESTS=check_1727944 check_2879249 check_free_objects check_weight_reduction check_883 check_duplicate_skills
#check_alchemy check_apply check_attack check_ban check_build_map check_c_chat check_c_misc check_c_move check_c_new check_c_object check_commands check_c_party check_c_range check_c_wiz check_disease check_egoitem check_gods check_hiscore check_init check_login check_main check_monster check_move check_pets check_player check_plugins check_resurrection check_rune check_shop check_skills check_skill_util check_spell_attack check_spell_effect check_spell_util check_swamp check_swap check_time check_timers check_weather check_win32 comet_perf


CHECK_ROOT=../../
CHECK_FOLDER=bugs/bugtrack
CHECK_PARENT_NAME=Bugs
include $(top_srcdir)/check-fragment.am

DISTCLEANFILES=	.autorun.xml

check-local: check-html
clean-local: clean-unit
