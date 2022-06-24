include $(top_srcdir)/check-fragment.am

CHECK_ROOT=../../
CHECK_FOLDER=unit/common
CHECK_PARENT_NAME=Unit

noinst_LIBRARIES = libstubs.a
libstubs_a_SOURCES=stubs_common.cpp
LDADD = $(top_builddir)/test/toolkit/libtoolkit_common.a $(top_builddir)/common/libcross.a libstubs.a @CHECK_LIBS@

$(top_builddir)/common/libcross.a:
	( cd $(top_builddir)/common ; make libcross.a )

$(top_builddir)/test/toolkit/libtoolkit_common.a:
	( cd $(top_builddir)/test/toolkit/; make libtoolkit_common.a)

# disabled tests, enable them when they have some real content
# check_anim check_button check_exp  check_glue check_info check_init
# check_links check_logger check_los check_map
# check_player check_porting check_recipe check_re-cmp

#check_anim_SOURCES = check_anim.cpp
check_arch_SOURCES = check_arch.cpp
check_artifact_SOURCES = check_artifact.cpp
check_asset_load_write_SOURCES = check_asset_load_write.cpp
#check_button_SOURCES = check_button.cpp
check_exp_SOURCES = check_exp.cpp
check_friend_SOURCES = check_friend.cpp
check_glue_SOURCES = check_glue.cpp
check_holy_SOURCES = check_holy.cpp
check_image_SOURCES = check_image.cpp
check_info_SOURCES = check_info.cpp
check_init_SOURCES = check_init.cpp
check_item_SOURCES = check_item.cpp
check_links_SOURCES = check_links.cpp
check_living_SOURCES = check_living.cpp
check_loader_SOURCES = check_loader.cpp
check_logger_SOURCES = check_logger.cpp
check_los_SOURCES = check_los.cpp
check_map_SOURCES = check_map.cpp
check_mapsave_SOURCES = check_mapsave.cpp
check_minheap_SOURCES = check_minheap.cpp
check_object_SOURCES = check_object.cpp
check_path_SOURCES = check_path.cpp
check_player_SOURCES = check_player.cpp
check_porting_SOURCES = check_porting.cpp
check_readable_SOURCES = check_readable.cpp
check_recipe_SOURCES = check_recipe.cpp
check_re_cmp_SOURCES = check_re-cmp.cpp
check_region_SOURCES = check_region.cpp
check_shstr_SOURCES = check_shstr.cpp
check_stringbuffer_SOURCES = check_stringbuffer.cpp
check_time_SOURCES = check_time.cpp
check_treasure_SOURCES = check_treasure.cpp
check_utils_SOURCES = check_utils.cpp
stubs_common_SOURCES = stubs_common.cpp

TESTS= check_arch check_artifact check_friend check_holy check_image check_item check_living check_loader check_mapsave check_minheap check_object check_path check_readable check_region check_shstr check_stringbuffer check_time check_treasure check_utils
TESTS+= check_asset_load_write

DISTCLEANFILES=	.autorun.xml

check-local: check-html
clean-local: clean-unit
