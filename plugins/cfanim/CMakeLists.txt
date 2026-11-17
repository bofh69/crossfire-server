add_library(cfanim MODULE
    ../common/plugin_common.cpp
    cfanim.cpp
)
set_target_properties(cfanim PROPERTIES PREFIX "")
include_directories(../common/include include)
target_precompile_headers(cfanim PRIVATE ../../include/global.h)
install(TARGETS cfanim DESTINATION ${CMAKE_INSTALL_LIBDIR}/${CMAKE_PROJECT_NAME}/plugins)
