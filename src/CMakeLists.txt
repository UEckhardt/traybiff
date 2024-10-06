add_compile_options(-Wall
					-Wextra
					-Wpedantic
					-Werror
					$<$<CONFIG:DEBUG>:-fno-omit-frame-pointer>
					$<$<CONFIG:DEBUG>:-fsanitize=address>)

add_compile_definitions (VERSION="${PROJECT_VERSION}"
                         $<$<CONFIG:RELEASE>:QT_NO_DEBUG_OUTPUT>)

add_link_options ("$<$<CONFIG:DEBUG>:-fno-omit-frame-pointer>"
                  "$<$<CONFIG:DEBUG>:-fsanitize=address>")

message("Buildtype: ${CMAKE_BUILD_TYPE}")

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(SRCS
	traybiff.cpp
	CAbout.cpp
	CMailApp.cpp
	systemtray/CTrayMenu.cpp
	setup/CSetupDialog.cpp
	setup/CConfig.cpp
	protocols/CMailMonitor.cpp
	protocols/CMailSocket.cpp
	protocols/CCrypt.cpp
	protocols/CPop3.cpp
	protocols/CImap.cpp
)

set(HDRS
	traybiff.h
	CAbout.h
	CMailApp.h
	systemtray/CTrayMenu.h
	setup/CSetupDialog.h
	setup/CConfig.h
	protocols/CMailMonitor.h
	protocols/CMailSocket.h
	protocols/CCrypt.h
	protocols/CPop3.h
	protocols/CImap.h
	protocols/IMailProtocol.h
)

set(UIS
	IAbout.ui
	setup/ISetupDialog.ui
)

set (RCS
	resources.qrc
)

set (QTMODULES
  Qt6::Core
  Qt6::Widgets
  Qt6::Network
  Qt6::Gui
 )

###############################################################################################
# Some Qt magic
###############################################################################################

qt_wrap_ui(UI_HDRS ${UIS})
qt_add_resources(RC_SRCS ${RCS})

add_executable(${PROJECT_NAME} ${SRCS} ${HDRS} ${UI_HDRS} ${RC_SRCS})

target_compile_definitions(traybiff PUBLIC
    -DVER_MAJOR=${PROJECT_VERSION_MAJOR}
    -DVER_MINOR=${PROJECT_VERSION_MINOR}
    -DVER_STEP=${PROJECT_VERSION_PATCH}
    -DAPPLICATION_NAME=${PROJECT_NAME}
)

target_link_libraries(${PROJECT_NAME} PRIVATE ${QTMODULES} )

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    # Install desktop entry
    install(FILES data/traybiff.desktop DESTINATION share/applications )

    # Install icon
    install(FILES icons/64x64/traybiff.png DESTINATION /usr/share/icons/hicolor/64x64/apps)
    install(FILES icons/64x64/traybiff.png DESTINATION /usr/share/icons/hicolor/128x128/apps)
endif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")

install(TARGETS ${PROJECT_NAME}
        EXPORT ${PROJECT_NAME}
        RUNTIME DESTINATION bin)

