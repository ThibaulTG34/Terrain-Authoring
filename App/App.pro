MOC_DIR = ./moc
OBJECTS_DIR = ./obj
LIBS = -lGL  -lX11 -lpthread -lXrandr -lXi -lglut -lassimp

INCLUDEPATH += $$PWD

HEADERS       = glwidget.h \
                cartes.h \
                mesh.h \
                window.h \
                mainwindow.h \
                Camera.h \
                AssimpModel.h

SOURCES       = glwidget.cpp \
                cartes.cpp \
                main.cpp \
                mesh.cpp \
                window.cpp \
                mainwindow.cpp \
                Camera.cpp \
                AssimpModel.cpp


RESOURCES += \
    shaders.qrc

QT           += widgets

