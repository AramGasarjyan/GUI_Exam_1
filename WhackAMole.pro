TEMPLATE = app
TARGET = WhackAMole
QT = core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
CONFIG += c++11

SOURCES += main.cpp \
           game_window.cpp \
           clock_label.cpp

HEADERS += game_window.h \
           clock_label.h
