bin_PROGRAMS = crossfire-server

pkglib_LTLIBRARIES = libserver.la
crossfire_server_SOURCES = main.cpp
libserver_la_SOURCES = \
	account.cpp \
	account_char.cpp \
	alchemy.cpp \
	apply.cpp \
	attack.cpp \
	ban.cpp \
	build_map.cpp \
	c_chat.cpp \
	c_misc.cpp \
	c_move.cpp \
	c_new.cpp \
	c_object.cpp \
	c_party.cpp \
	c_range.cpp \
	c_wiz.cpp \
	commands.cpp \
	disease.cpp \
	hiscore.cpp \
	gods.cpp \
	init.cpp \
	knowledge.cpp \
	login.cpp \
	monster.cpp \
	move.cpp \
	ob_methods.cpp \
	ob_types.cpp \
	party.cpp \
	pets.cpp \
	player.cpp \
	plugins.cpp \
	quest.cpp \
	races.cpp \
	resurrection.cpp \
	rune.cpp \
	shop.cpp \
	skills.cpp \
	skill_util.cpp \
	spell_attack.cpp \
	spell_effect.cpp \
	spell_util.cpp \
	swap.cpp \
	time.cpp \
	timers.cpp \
	weather.cpp \
	server.cpp \
	modules/cfcitybell.cpp \
	modules/citylife.cpp \
	modules/random_house_generator.cpp \
	win32.cpp \
	$(top_srcdir)/socket/image.cpp \
	$(top_srcdir)/socket/info.cpp \
	$(top_srcdir)/socket/init.cpp \
	$(top_srcdir)/socket/item.cpp \
	$(top_srcdir)/socket/loop.cpp \
	$(top_srcdir)/socket/lowlevel.cpp \
	$(top_srcdir)/socket/metaserver.cpp \
	$(top_srcdir)/socket/request.cpp \
	$(top_srcdir)/socket/requestinfo.cpp \
	$(top_srcdir)/socket/sounds.cpp \
	$(top_srcdir)/types/altar/altar.cpp \
	$(top_srcdir)/types/armour_improver/armour_improver.cpp \
	$(top_srcdir)/types/arrow/arrow.cpp \
	$(top_srcdir)/types/blindness/blindness.cpp \
	$(top_srcdir)/types/book/book.cpp \
	$(top_srcdir)/types/button/button.cpp \
	$(top_srcdir)/types/cf_handle/cf_handle.cpp \
	$(top_srcdir)/types/check_inv/check_inv.cpp \
	$(top_srcdir)/types/clock/clock.cpp \
	$(top_srcdir)/types/container/container.cpp \
	$(top_srcdir)/types/converter/converter.cpp \
	$(top_srcdir)/types/creator/creator.cpp \
	$(top_srcdir)/types/deep_swamp/deep_swamp.cpp \
	$(top_srcdir)/types/detector/detector.cpp \
	$(top_srcdir)/types/director/director.cpp \
	$(top_srcdir)/types/dragon_focus/dragon_focus.cpp \
	$(top_srcdir)/types/duplicator/duplicator.cpp \
	$(top_srcdir)/types/exit/exit.cpp \
	$(top_srcdir)/types/food/food.cpp \
	$(top_srcdir)/types/gate/gate.cpp \
	$(top_srcdir)/types/hole/hole.cpp \
	$(top_srcdir)/types/identify_altar/identify_altar.cpp \
	$(top_srcdir)/types/lamp/lamp.cpp \
	$(top_srcdir)/types/lightable/lightable.cpp \
	$(top_srcdir)/types/lighter/lighter.cpp \
	$(top_srcdir)/types/marker/marker.cpp \
	$(top_srcdir)/types/mimic/mimic.cpp \
	$(top_srcdir)/types/mood_floor/mood_floor.cpp \
	$(top_srcdir)/types/peacemaker/peacemaker.cpp \
	$(top_srcdir)/types/pedestal/pedestal.cpp \
	$(top_srcdir)/types/player_changer/player_changer.cpp \
	$(top_srcdir)/types/player_mover/player_mover.cpp \
	$(top_srcdir)/types/poison/poison.cpp \
	$(top_srcdir)/types/poisoning/poisoning.cpp \
	$(top_srcdir)/types/potion/potion.cpp \
	$(top_srcdir)/types/power_crystal/power_crystal.cpp \
	$(top_srcdir)/types/savebed/savebed.cpp \
	$(top_srcdir)/types/scroll/scroll.cpp \
	$(top_srcdir)/types/shop_inventory/shop_inventory.cpp \
	$(top_srcdir)/types/shop_mat/shop_mat.cpp \
	$(top_srcdir)/types/sign/sign.cpp \
	$(top_srcdir)/types/skillscroll/skillscroll.cpp \
	$(top_srcdir)/types/spell_effect/spell_effect.cpp \
	$(top_srcdir)/types/spellbook/spellbook.cpp \
	$(top_srcdir)/types/spinner/spinner.cpp \
	$(top_srcdir)/types/teleporter/teleporter.cpp \
	$(top_srcdir)/types/thrown_object/thrown_object.cpp \
	$(top_srcdir)/types/transport/transport.cpp \
	$(top_srcdir)/types/trap/common_trap.cpp \
	$(top_srcdir)/types/trap/trap.cpp \
	$(top_srcdir)/types/trap/rune.cpp \
	$(top_srcdir)/types/trapdoor/trapdoor.cpp \
	$(top_srcdir)/types/treasure/treasure.cpp \
	$(top_srcdir)/types/trigger/trigger.cpp \
	$(top_srcdir)/types/trigger_altar/trigger_altar.cpp \
	$(top_srcdir)/types/trigger_button/trigger_button.cpp \
	$(top_srcdir)/types/trigger_pedestal/trigger_pedestal.cpp \
	$(top_srcdir)/types/weapon_improver/weapon_improver.cpp \
	$(top_srcdir)/types/common/common_apply.cpp \
	$(top_srcdir)/types/common/describe.cpp \
	$(top_srcdir)/types/common/projectile.cpp \
	$(top_srcdir)/types/common/light_object.cpp \
	$(top_srcdir)/types/legacy/apply.cpp \
	$(top_srcdir)/types/legacy/legacy_describe.cpp \
	$(top_srcdir)/types/legacy/process.cpp
libserver_la_LIBADD = ../random_maps/librandom_map.la

libserver_la_CPPFLAGS = -I$(top_srcdir)/include -DDATADIR=\"$(pkgdatadir)\" -DCONFDIR=\"$(pkgconfdir)\" \
	      -DLIBDIR=\"$(pkglibdir)\" -DLOCALDIR=\"$(pkgstatedir)\" \
	      -DPLUGIN_SUFFIX=\".so\"

if PCH
libserver_la_CPPFLAGS += -include-pch $(top_srcdir)/include/global.h.gch
endif

crossfire_server_LDFLAGS=$(LIBDL_LDFLAGS)

crossfire_server_LDADD = libserver.la ../common/libcross.la ../random_maps/librandom_map.la

../common/libcross.la:
	( cd ../common ; make libcross.la )

../random_maps/librandom_map.la:
	( cd ../random_maps ; make librandom_map.la )
