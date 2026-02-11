set(TARGET_NAME "ysflight_util")
set(LIB_DEPENDENCY
geblkernel
geblgl
fsguilib
fsguifiledialog
ysbitmap
ysbitmapfont
ysfontrenderer
ysport
ystexturemanager
ysclass
ysclass11
ysglcpp
)

set(SRCS
fsbase.cpp
fsdef.cpp
fsgenmdl.cpp
fstextresource.cpp
fsutil.cpp
)

set(HEADERS
fsairsound.h
fsbase.h
fsdef.h
fsgenmdl.h
fsnetutil.h
fsrecord.h
fstextresource.h
fsutil.h
)

add_library(${TARGET_NAME} ${SRCS} ${HEADERS})
target_link_libraries(${TARGET_NAME} ${LIB_DEPENDENCY})
target_include_directories(${TARGET_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
