add_library(cfpython MODULE
    ../common/plugin_common.cpp
    cfpython.cpp
    cfpython_archetype.cpp
    cfpython_map.cpp
    cfpython_object.cpp
    cfpython_party.cpp
    cfpython_region.cpp
    cjson.cpp
)
set_target_properties(cfpython PROPERTIES PREFIX "")
include_directories(../common/include include ${Python_INCLUDE_DIRS})
target_precompile_headers(cfpython PRIVATE ../../include/global.h)
target_link_libraries(cfpython PUBLIC ${Python_LIBRARIES})
install(TARGETS cfpython DESTINATION ${CMAKE_INSTALL_LIBDIR}/${CMAKE_PROJECT_NAME}/plugins)
