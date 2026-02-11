set(TARGET_NAME ysflight_core)
set(LIB_DEPENDENCY
geblkernel
geblgl
ysflight_config
ysflight_platform_common
ysflight_gamemode
ysflight_external 
ysflight_aicontrol
ysflight_environment
ysflight_vehicle 
ysflight_util
yssocket
ysflight_graphics_common
fsguilib
fsguifiledialog
ysbitmap
ysbitmapfont
ysfontrenderer
ysport
ysscenery_dnm
ystexturemanager
ysclass
ysclass11
ysglcpp
)

set(SRCS
fscontrol.cpp
fsexistence.cpp
fsfield.cpp
fslattice.cpp
fsnetwork.cpp
fsparticle.cpp
fspersona.cpp
fsrunloop.cpp
fssiminfo.cpp
fssimulation.cpp
fssimulationdemomode.cpp
fsstdout.cpp
fsworld.cpp
)

set(HEADERS
fs.h
fscontrol.h
fsexistence.h
fsfield.h
fsgroundsky.h
fslattice.h
fsnetwork.h
fsparticle.h
fspersona.h
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
