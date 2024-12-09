QT       += core gui widgets

CONFIG += c++11

SOURCES += \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    mainwindow.h

FORMS += \
	mainwindow.ui

LIBS += -L$$PWD/pugixml/lib/ -lpugixml

INCLUDEPATH += $$PWD/pugixml/include
DEPENDPATH += $$PWD/pugixml/include
