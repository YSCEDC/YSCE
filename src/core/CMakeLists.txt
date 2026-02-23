set(TARGET_NAME ysflight_core)
set(LIB_DEPENDENCY
ysflight_aicontrol
ysflight_config
ysflight_environment
ysflight_external
ysflight_gamemode
ysflight_graphics_common
ysflight_gui
ysflight_instrument
ysflight_platform_common
ysflight_sound
ysscenery_dnm
ysflight_util
ysflight_vehicle
ysflight_weapon

geblkernel
ysclass
ysclass11
ysport
yssocket
ysglcpp
ysbitmap
)

set(SRCS
fscamera.cpp
fscontrol.cpp
fsexistence.cpp
fsfield.cpp
fslattice.cpp
fsnetwork.cpp
fsparticle.cpp
fspersona.cpp
fsphysics.cpp
fsrunloop.cpp
fssiminfo.cpp
fssimulation.cpp
fssimulationdemomode.cpp
fsstdout.cpp
fsworld.cpp
)

set(HEADERS
fs.h
fscamera.h
fscontrol.h
fsexistence.h
fsfield.h
fsgroundsky.h
fslattice.h
fsnetwork.h
fsparticle.h
fspersona.h
fsphysics.h
fsprintf.h
fsrunloop.h
fssiminfo.h
fssimulation.h
fsstdout.h
fsworld.h
)
add_library(${TARGET_NAME} ${SRCS} ${HEADERS})
target_link_libraries(${TARGET_NAME} ${LIB_DEPENDENCY})
target_include_directories(${TARGET_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
