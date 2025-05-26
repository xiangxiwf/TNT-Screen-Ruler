cmake_minimum_required(VERSION 3.16)

project(TNT-Screen-Ruler VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(PROJECT_SOURCES
    main.cpp
    widget.cpp
    widget.h
)

# 设置图标资源文件（仅 Windows）
if(WIN32)
    set(APP_ICON_RESOURCE "${CMAKE_CURRENT_SOURCE_DIR}/app_icon.rc")
endif()

# Qt6 构建方式
if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(TNT-Screen-Ruler
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        images.qrc
        ${APP_ICON_RESOURCE}  # 添加图标资源
    )
else()
    if(ANDROID)
        add_library(TNT-Screen-Ruler SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(TNT-Screen-Ruler
            ${PROJECT_SOURCES}
            ${APP_ICON_RESOURCE}  # 添加图标资源
        )
    endif()
endif()

target_link_libraries(TNT-Screen-Ruler PRIVATE Qt${QT_VERSION_MAJOR}::Widgets dwmapi)

# macOS/iOS 设置（保持不变）
if(${QT_VERSION} VERSION_LESS 6.1.0)
    set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.TNT-Screen-Ruler)
endif()
set_target_properties(TNT-Screen-Ruler PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS TNT-Screen-Ruler
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(TNT-Screen-Ruler)
endif()
