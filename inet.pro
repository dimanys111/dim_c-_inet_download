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

win32 {
    INCLUDEPATH += ../../ffmpeg/64/include
}

SOURCES += main.cpp\
    Player.cpp \
        mainwindow.cpp \
    robot.cpp \
    htmlcxx/html/ParserDom.cc \
    htmlcxx/html/Node.cc \
    htmlcxx/html/ParserSax.cc \
    spisovetka.cpp \
    stream.cpp \
    stringlistthread.cpp \
    work.cpp \
    work_bonga.cpp \
    work_bonga_m3u.cpp

HEADERS  += mainwindow.h \
    Player.h \
    htmlcxx/html/tree.h \
    htmlcxx/html/ParserDom.h \
    htmlcxx/html/Node.h \
    htmlcxx/html/ParserSax.h \
    robot.h \
    spisovetka.h \
    stream.h \
    stringlistthread.h \
    work.h \
    work_bonga.h \
    work_bonga_m3u.h

FORMS    += mainwindow.ui

win32 {
    LIBS +=  -lopengl32 -lglu32 -L../../ffmpeg/64/lib
}

LIBS +=  -lavcodec -lavutil -lavformat -lswscale -lavfilter

