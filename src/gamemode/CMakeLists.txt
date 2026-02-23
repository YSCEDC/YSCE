set(TARGET_NAME ysflight_gamemode)
set(LIB_DEPENDENCY
geblkernel
geblgl
ysflight_core
ysflight_util
ysflight_weapon
ysport
ysclass
ysclass11
ysglcpp
)

set(SRCS
"fsgamemode.cpp"
fsgamemode_racing.cpp
"fsgamemode_endurance.cpp"
fsgamemode_intercept.cpp
"fsgamemode_closeairsupport.cpp"
fsgamemode_groundtoair.cpp
)

set(HEADERS
"fsgamemode.h"
fsgamemode_racing.h
"fsgamemode_endurance.h"
fsgamemode_intercept.h
"fsgamemode_closeairsupport.h"
fsgamemode_groundtoair.h
)

add_library(${TARGET_NAME} ${SRCS} ${HEADERS})
target_link_libraries(${TARGET_NAME} ${LIB_DEPENDENCY})
target_include_directories(${TARGET_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
