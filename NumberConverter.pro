#-------------------------------------------------
#
# Project created by QtCreator 2018-01-14T22:52:20
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = NumberConverter
TEMPLATE = app

#CONFIG += c++11

QMAKE_CXXFLAGS += -std=c++17 -Werror=return-type -Werror=pedantic --pedantic-errors -Wall -Wextra

QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE += -O3

QMAKE_CXXFLAGS_DEBUG += -O0

QMAKE_CXXFLAGS += -isystemK:\1\0\source\boost_1_71_0


SOURCES += main.cpp\
		MainWindow.cpp\
		NumberConverter.cpp \
		Ut.cpp

HEADERS  += MainWindow.h\
		NumberConverter.h \
		Ut.h

FORMS    += MainWindow.ui


