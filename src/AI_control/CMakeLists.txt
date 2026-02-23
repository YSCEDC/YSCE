set(TARGET_NAME ysflight_aicontrol)
set(LIB_DEPENDENCY 
	geblkernel 
	geblgl
	fssimplewindow
	ysclass
	ysport
	yssocket
	ysflight_core
	ysflight_environment
	ysflight_instrument
	ysflight_sound
	ysflight_util
	ysflight_vehicle
	ysflight_platform_common 
	ysflight_graphics_common 
	ysscenery_dnm 
)
set(SRCS
fsai_airshow.cpp
fsai_air.cpp
fsai_experimental.cpp
fsai_defender.cpp
fsai_airtoair.cpp
fsai_formation.cpp
fsai_airtoground.cpp
fsai_mission.cpp
fsai_refuelandtakeoff.cpp
fsai_threatavoidingpath.cpp
fsai_ground.cpp
fsai_common.cpp
fsatc.cpp
)
set(HEADERS
fsai_air.h
fsai_experimental.h
fsai_defender.h
fsai_airtoair.h
fsai_formation.h
fsai_airtoground.h
fsai_mission.h
fsai_refuelandtakeoff.h
fsai_common.h
fsatc.h
fsai_ground.h
fsai_pathplanning.h
fsai_threatavoidingpath.h
)

add_library(${TARGET_NAME} ${SRCS} ${HEADERS})
target_link_libraries(${TARGET_NAME} ${LIB_DEPENDENCY})
target_include_directories(${TARGET_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
