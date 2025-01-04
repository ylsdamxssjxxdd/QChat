# set target name
set(TARGET QChat)

# add resources
set(resourcesFILE ../resources/res.qrc)

# add binary file
add_executable(${TARGET} 
main.cpp 
ui/mainui.cpp 
${resourcesFILE}
ui/friendwindow.cpp 
ui/chatwindow.cpp 
ui/expandwindow.cpp 
ui/settingswindow.cpp
)


# link related librarys and create binary file
# Find required Qt packages
find_package(Qt5 COMPONENTS 
    Core 
    Widgets 
    Network 
    Multimedia 
    Concurrent 
    Gui
    REQUIRED)

# Add include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${Qt5Core_INCLUDE_DIRS}
    ${Qt5Network_INCLUDE_DIRS}
    ${Qt5Widgets_INCLUDE_DIRS}
    ${Qt5Concurrent_INCLUDE_DIRS}
)

# Link Qt libraries
target_link_libraries(${TARGET} PRIVATE 
    Qt5::Core 
    Qt5::Widgets 
    Qt5::Network 
    Qt5::Multimedia 
    Qt5::Concurrent
    Qt5::Gui
)

if(MSVC)
    # 在生成目标之后执行 windeployqt
    add_custom_command(TARGET ${TARGET} POST_BUILD
        COMMAND "${Qt5_BIN_DIR}/windeployqt.exe" "--release" "--no-translations" "$<TARGET_FILE:${TARGET}>"
        COMMENT "Running windeployqt..."
    )
endif()
