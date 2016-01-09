#-------------------------------------------------
#
# Project created by QtCreator 2016-01-09T18:27:21
#
#-------------------------------------------------

QT       -= core

QT       -= gui

TARGET = BrownMovement
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp

# GTK+ library
unix: CONFIG	+= link_pkgconfig
unix: PKGCONFIG += gtk+-3.0
