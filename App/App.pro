MOC_DIR = ./moc
OBJECTS_DIR = ./obj
LIBS = -lGLEW -lglfw -lGL -lX11 -lpthread -lXrandr -lXi

INCLUDEPATH += $$PWD

HEADERS       = glwidget.h \
                cartes.h \
                mesh.h \
                window.h \
                mainwindow.h \
                logo.h \
                diffusionterrain.h \
                gpu-shader.h \
                basics.h \
                vec.h

SOURCES       = glwidget.cpp \
                cartes.cpp \
                main.cpp \
                mesh.cpp \
                window.cpp \
                mainwindow.cpp \
                logo.cpp \
                diffusionterrain.cpp \
                gpu-shader.cpp

RESOURCES += \
    shaders.qrc

QT           += widgets

