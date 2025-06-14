QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets multimedia multimediawidgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGSQT

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

####以下代码为添加的OpenCV3库####
INCLUDEPATH +=/usr/local/include
LIBS +=-L/usr/local/lib -lopencv_imgproc -lopencv_core -lopencv_highgui -lopencv_imgcodecs
####以上代码为添加的OpenCV3库####


SOURCES += \
    circlepainter.cpp \
    countwindow.cpp \
    loginwindow.cpp \
    main.cpp \
    mycamera.cpp \
    screenshot.cpp

HEADERS += \
    circlepainter.h \
    countwindow.h \
    loginwindow.h \
    mycamera.h \
    screenshot.h

FORMS += \
    camerawindow.ui \
    circlepainter.ui \
    countwindow.ui \
    loginwindow.ui \
    mycamera.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RC_ICONS = myico.ico
