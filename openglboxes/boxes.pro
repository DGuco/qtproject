# ----------------------------------------------------
# This file is generated by the Qt Visual Studio Tools.
# ------------------------------------------------------

TEMPLATE = app
TARGET = boxes
DESTDIR = ./debug
QT += opengl widgets
CONFIG += debug
DEFINES += _WINDOWS _UNICODE WIN64
LIBS += -lshell32 \
    -lopengl32
DEPENDPATH += .
MOC_DIR += .
OBJECTS_DIR += debug
UI_DIR += GeneratedFiles
RCC_DIR += .
include(boxes.pri)
