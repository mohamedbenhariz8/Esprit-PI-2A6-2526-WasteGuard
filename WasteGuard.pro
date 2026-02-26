QT += core gui widgets sql charts multimedia multimediawidgets
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17
CONFIG += console

# Ensure every line except the last one has a backslash \
SOURCES += \
    client.cpp \
    connection.cpp \
    employe.cpp \
    intervention.cpp \
    main.cpp \
    mainwindow.cpp \
    mascotwidget.cpp \
    produit.cpp \
    stock.cpp \
    commande.cpp

HEADERS += \
    client.h \
    connection.h \
    employe.h \
    intervention.h \
    mainwindow.h \
    mascotwidget.h \
    produit.h \
    stock.h \
    commande.h

FORMS += \
    mainwindow.ui

RESOURCES += \
    logo.qrc
