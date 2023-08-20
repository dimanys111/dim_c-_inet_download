#-------------------------------------------------
#
# Project created by QtCreator 2015-10-19T20:48:13
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4) {
    QT += widgets
}

lessThan(QT_MAJOR_VERSION, 6) {
    QT += multimedia multimediawidgets
}

TARGET = inet1
TEMPLATE = app

CONFIG += c++17

SOURCES += main.cpp\
        mainwindow.cpp \
    player.cpp \
    robot.cpp \
    stream.cpp \
    work_bonga.cpp \
    work_bonga_m3u.cpp

HEADERS  += mainwindow.h \
    player.h \
    robot.h \
    stream.h \
    work_bonga.h \
    work_bonga_m3u.h

FORMS    += mainwindow.ui

win32 {
    LIBS +=  -lopengl32 -lglu32 -L../../ffmpeg/64/lib
}

LIBS +=  -lavcodec -lavutil -lavformat -lswscale -lavfilter

