QT       += core gui widgets sql charts multimedia multimediawidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

SOURCES += \
    connection.cpp \
    main.cpp \
    mainwindow.cpp \
    mascotwidget.cpp \
    produit.cpp \
    stockmapwidget.cpp

HEADERS += \
    connection.h \
    mainwindow.h \
    mascotwidget.h \
    produit.h \
    stockmapwidget.h

FORMS += \
    mainwindow.ui

RESOURCES += \
    logo.qrc
QT += sql
CONFIG += console
