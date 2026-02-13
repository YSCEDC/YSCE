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
fsairshowacro.cpp
fsautopilot.cpp
fsautopilotexperimental.cpp
fsdefenderautopilot.cpp
fsdogfightautopilot.cpp
fsformationautopilot.cpp
fsgroundattackautopilot.cpp
fsmissionautopilot.cpp
fsrefuelandtakeoff.cpp
fsthreatavoidingpath.cpp
fsautodrive.cpp
fsaibasic.cpp
fsatc.cpp
)
set(HEADERS
fsautopilot.h
fsautopilotexperimental.h
fsdefenderautopilot.h
fsdogfightautopilot.h
fsformationautopilot.h
fsgroundattackautopilot.h
fsmissionautopilot.h
fsrefuelandtakeoff.h
fsaibasic.h
fsatc.h
fsautodrive.h
)

add_library(${TARGET_NAME} ${SRCS} ${HEADERS})
target_link_libraries(${TARGET_NAME} ${LIB_DEPENDENCY})
target_include_directories(${TARGET_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
