set(TARGET_NAME ysflight_sound)
set(LIB_DEPENDENCY
ysclass
)
set(SRCS
fssound.cpp
)
set(HEADERS
fssound.h
)

add_library(${TARGET_NAME} ${SRCS} ${HEADERS})
target_link_libraries(${TARGET_NAME} ${LIB_DEPENDENCY})
target_include_directories(${TARGET_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})


if(MSVC)
	add_subdirectory(sounddll/win32)
	add_subdirectory(voicedll/generic)
elseif(APPLE)
	add_subdirectory(sounddll/macosx)
	add_subdirectory(voicedll/generic)
elseif(UNIX)
	add_subdirectory(sounddll/linux-alsa)
	add_subdirectory(sounddll/linux-pulseaudio)
	add_subdirectory(voicedll/generic)
else()
	add_subdirectory(voicedll/generic)
endif()