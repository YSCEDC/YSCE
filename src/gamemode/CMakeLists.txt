set(TARGET_NAME ysflight_gamemode)
set(LIB_DEPENDENCY
geblkernel
geblgl
ysflight_core
ysflight_config
ysflight_aicontrol
ysflight_vehicle 
ysflight_util
ysport
ysclass
ysclass11
ysglcpp
)

set(SRCS
fssimextension.cpp
fssimextension_racing.cpp
fssimextension_endurance.cpp
fssimextension_intercept.cpp
fssimextension_closeairsupport.cpp
fssimextension_groundtoair.cpp
)

set(HEADERS
fssimextension.h
fssimextension_racing.h
fssimextension_endurance.h
fssimextension_intercept.h
fssimextension_closeairsupport.h
fssimextension_groundtoair.h
)

add_library(${TARGET_NAME} ${SRCS} ${HEADERS})
target_link_libraries(${TARGET_NAME} ${LIB_DEPENDENCY})
target_include_directories(${TARGET_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
