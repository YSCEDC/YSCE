set(TARGET_NAME ysflight_ui)
set(LIB_DEPENDENCY
geblkernel
geblgl
ysflight_core
ysflight_config
ysflight_platform_common
ysflight_pathplanning 
ysflight_externalconsole 
ysflight_autopilot 
ysflight_dynamics
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
ysflight_filename
ysclass
ysclass11
ysglcpp
)

set(SRCS


)

set(HEADERS


)

add_library(${TARGET_NAME} ${SRCS} ${HEADERS})
target_link_libraries(${TARGET_NAME} ${LIB_DEPENDENCY})
target_include_directories(${TARGET_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
