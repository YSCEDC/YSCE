set(TARGET_NAME "ysflight_vehicle")
set(LIB_DEPENDENCY 
ysscenery_dnm 
ysflight_core
ysflight_environment
ysflight_instrument
ysflight_util
ysflight_weapon
ysflight_platform_common 
ysflight_graphics_common  
yssocket
geblkernel 
geblgl 
ysclass 
ysport 
)

# Don't link fssimplewindow.  It can be fssimplenowindow.  Just add include directory
get_property(ADDITIONAL_INCLUDE_DIR TARGET fssimplewindow PROPERTY INCLUDE_DIRECTORIES)

set(SRCS
fsairplaneproperty.cpp
fsgroundproperty.cpp
fscommonproperty.cpp
fsengine.cpp
)

set(HEADERS
fsairplaneproperty.h
fsgroundproperty.h
fscommonproperty.h
fsengine.h
)


add_library(${TARGET_NAME} ${SRCS} ${HEADERS})
target_link_libraries(${TARGET_NAME} ${LIB_DEPENDENCY})
target_include_directories(${TARGET_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${ADDITIONAL_INCLUDE_DIR})
