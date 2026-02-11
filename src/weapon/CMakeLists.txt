set(TARGET_NAME ysflight_weapon)
set(LIB_DEPENDENCY
ysclass
ysport
ysflight_common
ysflight_util
)

set(SRCS
fsexplosion.cpp
fsweapon.cpp
fsproperty.cpp
)

set(HEADERS
fsexplosion.h
fsweapon.h
fsproperty.h
)

add_library(${TARGET_NAME} ${SRCS} ${HEADERS})
target_link_libraries(${TARGET_NAME} ${LIB_DEPENDENCY})
target_include_directories(${TARGET_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
