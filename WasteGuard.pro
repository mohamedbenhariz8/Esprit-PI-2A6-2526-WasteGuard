QT += core gui widgets sql charts multimedia multimediawidgets printsupport network quickwidgets qml quick location positioning opengl openglwidgets concurrent serialport
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17
CONFIG += console

# Force UTF-8 source and execution charset to keep UI text readable in binaries.
QMAKE_CXXFLAGS += -finput-charset=UTF-8 -fexec-charset=UTF-8

TARGET = WasteGuard

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
    commande.cpp \
    stockmapwidget.cpp \
    videogenerationdialog.cpp \
    triposr3ddialog.cpp \
    viewer3ddialog.cpp \
    modelviewerwidget.cpp \
    accessibilityhelper.cpp \
    voiceassistant.cpp \
    labibassistant.cpp \
    emailnotificationmanager.cpp \
    thememanager.cpp \
    bacdetector.cpp \
    bacstatusdialog.cpp \
    maintenancestatusdelegate.cpp \
    arduino.cpp \
    employeehistorystore.cpp \
    repairdialog.cpp

HEADERS += \
    client.h \
    connection.h \
    employe.h \
    intervention.h \
    mainwindow.h \
    mascotwidget.h \
    produit.h \
    stock.h \
    commande.h \
    stockmapwidget.h \
    videogenerationdialog.h \
    triposr3ddialog.h \
    viewer3ddialog.h \
    modelviewerwidget.h \
    accessibilityhelper.h \
    voiceassistant.h \
    labibassistant.h \
    emailnotificationmanager.h \
    thememanager.h \
    bacdetector.h \
    bacstatusdialog.h \
    maintenancestatusdelegate.h \
    arduino.h \
    employeehistorystore.h \
    repairdialog.h

FORMS += \
    mainwindow.ui

RESOURCES += \
    logo.qrc

unix {
    QMAKE_POST_LINK += cp -f \"$$PWD/voice_engine.py\" \"$$OUT_PWD/voice_engine.py\" || true$$escape_expand(\\n\\t)
    QMAKE_POST_LINK += cp -rf \"$$PWD/models\" \"$$OUT_PWD/models\" || true$$escape_expand(\\n\\t)
    QMAKE_POST_LINK += cp -f \"$$PWD/ai_worker.py\" \"$$OUT_PWD/ai_worker.py\" || true
}
win32 {
    QMAKE_POST_LINK += $$quote(cmd /c "(copy /Y \"$$PWD\\voice_engine.py\" \"$$OUT_PWD\\voice_engine.py\" >nul || ver >nul)")$$escape_expand(\\n\\t)
    QMAKE_POST_LINK += $$quote(cmd /c "(xcopy /Y /I /E \"$$PWD\\models\" \"$$OUT_PWD\\models\" >nul || ver >nul)")$$escape_expand(\\n\\t)
    QMAKE_POST_LINK += $$quote(cmd /c "(copy /Y \"$$PWD\\ai_worker.py\" \"$$OUT_PWD\\ai_worker.py\" >nul || ver >nul)")
}
