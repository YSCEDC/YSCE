set(TARGET_NAME ysflight_instrument)
set(LIB_DEPENDENCY ysclass ysport ysflight_common ysflight_util)

set(SRCS
fsradar.cpp
fshud.cpp
fshud2.cpp
fsinstpanel.cpp
fsnavaid.cpp
)

set(HEADERS
fsradar.h
fshud.h
fshud2.h
fsinstpanel.h
fsnavaid.h
)
add_library(${TARGET_NAME} ${SRCS} ${HEADERS})
target_link_libraries(${TARGET_NAME} ${LIB_DEPENDENCY})
target_include_directories(${TARGET_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
