set(TARGET_NAME ysflight_instrument)
set(LIB_DEPENDENCY
ysclass
ysport
ysglcpp
ysflight_core
ysflight_external
ysflight_util
ysflight_graphics_common
ysflight_platform_common
)

set(SRCS
fshud.cpp
fshud2.cpp
fsinstpanel.cpp
fsnavaid.cpp
fsradar.cpp
)

set(HEADERS
fshud.h
fshud2.h
fsinstpanel.h
fsnavaid.h
fsradar.h
)
add_library(${TARGET_NAME} ${SRCS} ${HEADERS})
target_link_libraries(${TARGET_NAME} ${LIB_DEPENDENCY})
target_include_directories(${TARGET_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
