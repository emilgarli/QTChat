cmake_minimum_required(VERSION 3.16)

project(ChattingApp VERSION 0.1 LANGUAGES CXX)

# Enable automatic Qt handling
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Qt libraries
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

# Define project source files
set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    ConnectionHandler.h
    ConnectionHandler.cpp
    Rawsocket.h
    Rawsocket.cpp
    activeconnection.h
    activeconnection.cpp
)

# Check if Qt version is 6 or greater
if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(ChattingApp
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
else()
    if(ANDROID)
        add_library(ChattingApp SHARED ${PROJECT_SOURCES})
    else()
        add_executable(ChattingApp ${PROJECT_SOURCES})
    endif()
endif()

# Link Qt libraries
target_link_libraries(ChattingApp PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

# Link Winsock (needed for Windows sockets)
target_link_libraries(ChattingApp PRIVATE ws2_32)

# PortAudio setup
# Include PortAudio headers
include_directories(${CMAKE_SOURCE_DIR}/../libs/portaudio/include)

# Link PortAudio library (assuming it's located in the "release" directory)
target_link_libraries(ChattingApp PRIVATE ${CMAKE_SOURCE_DIR}/../libs/portaudio/release/portaudio_x64.lib)

# macOS bundle properties
if(${QT_VERSION} VERSION_LESS 6.1.0)
    set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.ChattingApp)
endif()

# Set properties for the target
set_target_properties(ChattingApp PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

# Installation rules
include(GNUInstallDirs)
install(TARGETS ChattingApp
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Finalize Qt executable
if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(ChattingApp)
endif()
