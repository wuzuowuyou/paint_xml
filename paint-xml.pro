QT += core
QT -= gui

CONFIG += c++11

TARGET = paint-xml
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    Markup.cpp

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

HEADERS += \
    Markup.h


OPENCV_ROOT_PATH = /home/yhl/software_install/opencv3.2  #opencv3.2

INCLUDEPATH += $${OPENCV_ROOT_PATH}/include \
               $${OPENCV_ROOT_PATH}/include/opencv \
               $${OPENCV_ROOT_PATH}/include/opencv2

LIBS += -L$${OPENCV_ROOT_PATH}/lib



LIBS += -lopencv_core \
        -lopencv_highgui \
        -lopencv_imgproc \
        -lopencv_imgcodecs \
        -lopencv_videoio


LIBS += -lboost_serialization
LIBS += -lboost_system
LIBS += -lboost_filesystem
LIBS += -lboost_thread



