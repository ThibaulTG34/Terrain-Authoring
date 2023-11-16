MOC_DIR = ./moc
OBJECTS_DIR = ./obj

INCLUDEPATH += $$PWD

HEADERS       = glwidget.h \
                cartes.h \
                mesh.h \
                window.h \
                mainwindow.h \
                logo.h
SOURCES       = glwidget.cpp \
                cartes.cpp \
                main.cpp \
                mesh.cpp \
                window.cpp \
                mainwindow.cpp \
                logo.cpp

RESOURCES += \
    shaders.qrc

QT           += widgets


