set(TARGET_NAME ysflight_config)
set(LIB_DEPENDENCY
ysclass
ysport
ysflight_core
ysflight_environment
ysflight_graphics_common
ysflight_gui
ysflight_platform_common
ysflight_util
)
set(SRCS
fsapplyoption.cpp
fsconfig.cpp
fsfilename.cpp
fsoption.cpp
fsnetconfig.cpp
)
set(HEADERS
fsapplyoption.h
fsconfig.h
fsfilename.h
fsoption.h
fsnetconfig.h
)

add_library(${TARGET_NAME} ${SRCS} ${HEADERS})
target_link_libraries(${TARGET_NAME} ${LIB_DEPENDENCY})
target_include_directories(${TARGET_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
