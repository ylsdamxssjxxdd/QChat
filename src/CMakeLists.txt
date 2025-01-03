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
network/NetworkManager.cpp
)

# Add test executable
qt5_wrap_cpp(NetworkManagerTest_MOCS 
    network/NetworkManagerTest.cpp
    network/NetworkTester.h
)
add_executable(NetworkManagerTest
network/NetworkManagerTest.cpp
network/NetworkManager.cpp
network/NetworkTester.cpp
${NetworkManagerTest_MOCS}
)
target_link_libraries(NetworkManagerTest PRIVATE 
    Qt5::Core 
    Qt5::Network 
    Qt5::Widgets
)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/network
    ${Qt5Core_INCLUDE_DIRS}
    ${Qt5Network_INCLUDE_DIRS}
    ${Qt5Widgets_INCLUDE_DIRS}
)

# link related librarys and create binary file
target_link_libraries(${TARGET} PRIVATE Qt5::Core Qt5::Widgets Qt5::Network Qt5::Multimedia)
