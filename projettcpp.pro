QT       += core gui widgets sql charts multimedia multimediawidgets printsupport network concurrent opengl openglwidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17
QMAKE_CXXFLAGS += -finput-charset=UTF-8 -fexec-charset=UTF-8

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    client.cpp \
    connection.cpp \
    commande.cpp \
    employe.cpp \
    intervention.cpp \
    main.cpp \
    mainwindow.cpp \
    mascotwidget.cpp \
    produit.cpp \
    stock.cpp \
    stockmapwidget.cpp \
    videogenerationdialog.cpp \
    triposr3ddialog.cpp \
    viewer3ddialog.cpp \
    modelviewerwidget.cpp \
    accessibilityhelper.cpp \
    voiceassistant.cpp \
    labibassistant.cpp \
    emailnotificationmanager.cpp \
    thememanager.cpp

HEADERS += \
    client.h \
    connection.h \
    commande.h \
    employe.h \
    intervention.h \
    mainwindow.h \
    mascotwidget.h \
    produit.h \
    stock.h \
    stockmapwidget.h \
    videogenerationdialog.h \
    triposr3ddialog.h \
    viewer3ddialog.h \
    modelviewerwidget.h \
    accessibilityhelper.h \
    voiceassistant.h \
    labibassistant.h \
    emailnotificationmanager.h \
    thememanager.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    logo.qrc
CONFIG += console
