cmake_minimum_required(VERSION 3.16)
project(WhackAMole LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)          # runs moc automatically (needed for Q_OBJECT)

# Works with Qt 6 or Qt 5
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

add_executable(WhackAMole WIN32 MACOSX_BUNDLE
    main.cpp
    game_window.cpp
    game_window.h
    clock_label.cpp
    clock_label.h
)

target_link_libraries(WhackAMole PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
