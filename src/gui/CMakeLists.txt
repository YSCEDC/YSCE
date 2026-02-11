set(TARGET_NAME ysflight_gui)
set(LIB_DEPENDENCY ysflight_ui ysjoystick)

set(CURL_DIR "")


set(SRCS
fschoose.cpp
fsguicommondialog.cpp
fsguiconfigdlg.cpp
fsguiinfltdlg.cpp
fsguimaincanvas.cpp
fsguinetdialog.cpp
fsguinewflightdialog.cpp
fsguiselectiondialogbase.cpp
fsguisiminfodialog.cpp
fsmenu.cpp
fsmenu_file.cpp
fsmenu_net.cpp
fsmenu_sim.cpp
fsmenu_util.cpp
fsmenu_option.cpp
fssubmenu.cpp
)

set(HEADERS
fschoose.h
fsdialog.h
fsguicommondialog.h
fsguiconfigdlg.h
fsguiinfltdlg.h
fsguimaincanvas.h
fsguinetdialog.h
fsguinewflightdialog.h
fsguiselectiondialogbase.h
fsguisiminfodialog.h
fsmenu.h
fsmissiongoal.h
fssubmenu.h
)

find_package(CURL CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

add_library(${TARGET_NAME} ${SRCS} ${HEADERS})
target_link_libraries(${TARGET_NAME} ${LIB_DEPENDENCY})
target_link_libraries(${TARGET_NAME} ${LIB_DEPENDENCY} CURL::libcurl)
target_link_libraries(${TARGET_NAME} ${LIB_DEPENDENCY} nlohmann_json::nlohmann_json)
target_include_directories(${TARGET_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
