#note all .a are here twice for compilation problems
AM_LIBTOOLFLAGS= --preserve-dup-deps
LINK = $(LIBTOOL) --tag=CC $(AM_LIBTOOLFLAGS) $(LIBTOOLFLAGS) \
        --mode=link $(CCLD) $(AM_CFLAGS) $(CFLAGS) $(AM_LDFLAGS) \
       $(LDFLAGS) -o $@

LDADD = $(top_builddir)/server/libserver.a \
        $(top_builddir)/common/libcross.a \
        $(top_builddir)/server/libserver.a \
        $(top_builddir)/common/libcross.a \
        $(top_builddir)/random_maps/librandom_map.a \
        $(top_builddir)/socket/libsocket.a \
        $(top_builddir)/types/libtypes.a \
        $(top_builddir)/test/toolkit/libtoolkit_common.a \
        $(top_builddir)/test/toolkit/libtoolkit_server.a \
        @CHECK_LIBS@

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

$(top_builddir)/test/toolkit/libtoolkit_server.a:
	( cd $(top_builddir)/test/toolkit/; make libtoolkit_server.a)

$(top_builddir)/test/toolkit/libtoolkit_common.a:
	( cd $(top_builddir)/test/toolkit/; make libtoolkit_common.a)

check_account_char_SOURCES = check_account_char.cpp
check_account_SOURCES = check_account.cpp
check_alchemy_SOURCES = check_alchemy.cpp
#check_apply_SOURCES = check_apply.cpp
check_attack_SOURCES = check_attack.cpp
#check_ban_SOURCES = check_ban.cpp
#check_build_map_SOURCES = check_build_map.cpp
#check_c_chat_SOURCES = check_c_chat.cpp
#check_c_misc_SOURCES = check_c_misc.cpp
#check_c_move_SOURCES = check_c_move.cpp
#check_c_new_SOURCES = check_c_new.cpp
check_c_object_SOURCES = check_c_object.cpp
#check_commands_SOURCES = check_commands.cpp
check_c_party_SOURCES = check_c_party.cpp
#check_c_range_SOURCES = check_c_range.cpp
#check_c_wiz_SOURCES = check_c_wiz.cpp
#check_disease_SOURCES = check_disease.cpp
#check_gods_SOURCES = check_gods.cpp
#check_hiscore_SOURCES = check_hiscore.cpp
#check_init_SOURCES = check_init.cpp
#check_login_SOURCES = check_login.cpp
#check_main_SOURCES = check_main.cpp
check_monster_change_SOURCES = check_monster_change.cpp
check_monster_SOURCES = check_monster.cpp
#check_move_SOURCES = check_move.cpp
#check_pets_SOURCES = check_pets.cpp
check_player_SOURCES = check_player.cpp
#check_plugins_SOURCES = check_plugins.cpp
#check_resurrection_SOURCES = check_resurrection.cpp
#check_rune_SOURCES = check_rune.cpp
check_shop_SOURCES = check_shop.cpp
#check_skills_SOURCES = check_skills.cpp
#check_skill_util_SOURCES = check_skill_util.cpp
#check_spell_attack_SOURCES = check_spell_attack.cpp
#check_spell_effect_SOURCES = check_spell_effect.cpp
#check_spell_util_SOURCES = check_spell_util.cpp
#check_swamp_SOURCES = check_swamp.cpp
check_swap_SOURCES = check_swap.cpp
#check_time_SOURCES = check_time.cpp
#check_timers_SOURCES = check_timers.cpp
#check_weather_SOURCES = check_weather.cpp
#check_win32_SOURCES = check_win32.cpp
comet_perf_SOURCES = comet_perf.cpp
perf_tests_SOURCES = perf_tests.cpp

# tests not implemented for now
#TESTS=check_apply check_ban check_build_map check_c_chat check_c_misc check_c_move check_c_new  check_commands  check_c_range check_c_wiz check_disease check_egoitem check_gods check_hiscore check_init check_login check_main check_move check_pets check_plugins check_resurrection check_rune check_skills check_skill_util check_spell_attack check_spell_effect check_spell_util check_swamp check_time check_timers check_weather check_win32
TESTS=check_account check_account_char check_alchemy check_attack check_monster check_c_object comet_perf \
	check_c_party check_shop check_swap check_monster_change check_player perf_tests

CHECK_ROOT=../../
CHECK_FOLDER=unit/server
CHECK_PARENT_NAME=Unit
include $(top_srcdir)/check-fragment.am

DISTCLEANFILES=	.autorun.xml

check-local: check-html
clean-local: clean-unit
