set(TARGET_NAME ysflight_environment)
set(LIB_DEPENDENCY
ysclass
ysport
ysflight_core
ysflight_util
ysflight_graphics_common
)

set(SRCS
fsairproperty.cpp
fscloud.cpp
fsweather.cpp
)

set(HEADERS
fsairproperty.h
fscloud.h
fsweather.h
)
add_library(${TARGET_NAME} ${SRCS} ${HEADERS})
target_link_libraries(${TARGET_NAME} ${LIB_DEPENDENCY})
target_include_directories(${TARGET_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
