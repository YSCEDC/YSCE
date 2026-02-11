set(TARGET_NAME "ysflight_external")
set(LIB_DEPENDENCY
ysclass
ysflight_util
)

set(SRCS
fsatcfileio.cpp
fsinstreading.cpp
fspluginmgr.cpp
fssimulationfileio.cpp
fsscript.cpp
fscmdparaminfo.cpp
)
set(HEADERS
fsexternalconsole.h
fsplugin.h
fspluginmgr.h
fsscript.h
fscmdparaminfo.h
)

add_library(${TARGET_NAME} ${SRCS} ${HEADERS})
target_link_libraries(${TARGET_NAME} ${LIB_DEPENDENCY})
target_include_directories(${TARGET_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
