#-------------------------------------------------
#
# Project created by QtCreator 2016-02-28T21:57:11
#
#-------------------------------------------------

QT       += core gui opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

TARGET = mg1
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    sceneglwidget.cpp \
    torus.cpp \
    fullshader.cpp

HEADERS  += mainwindow.h \
    sceneglwidget.h \
    torus.h \
    glm.h \
    fullshader.h

FORMS    += mainwindow.ui
