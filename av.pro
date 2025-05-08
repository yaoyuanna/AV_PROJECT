QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++14

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

include(./opengl/opengl.pri)

INCLUDEPATH += ./opengl/

SOURCES += \
    audioplayer.cpp \
    m_queue.cpp \
    main.cpp \
    mainwindow.cpp \
    play_window.cpp \
    player.cpp \
    player1.cpp \
    videoplayer.cpp

HEADERS += \
    audioplayer.h \
    m_queue.h \
    mainwindow.h \
    play_window.h \
    player.h \
    videoplayer.h

FORMS += \
    mainwindow.ui \
    play_window.ui

INCLUDEPATH +=$${PWD}/ffmpeg-4.2.2-win64-dev/include\
              $${PWD}/SDL2-2.0.10/include
              #$${PWD}/ffmpeg-7.0.2-full_build-shared/include\
              #$${PWD}/SDL3-devel-3.2.10-VC/SDL3-3.2.10/include

# LIBS += $${PWD}/ffmpeg-7.0.2-full_build-shared/lib/avcodec.lib\
#         $${PWD}/ffmpeg-7.0.2-full_build-shared/lib/avdevice.lib\
#         $${PWD}/ffmpeg-7.0.2-full_build-shared/lib/avfilter.lib\
#         $${PWD}/ffmpeg-7.0.2-full_build-shared/lib/avformat.lib\
#         $${PWD}/ffmpeg-7.0.2-full_build-shared/lib/avutil.lib\
#         $${PWD}/ffmpeg-7.0.2-full_build-shared/lib/postproc.lib\
#         $${PWD}/ffmpeg-7.0.2-full_build-shared/lib/swresample.lib\
#         $${PWD}/ffmpeg-7.0.2-full_build-shared/lib/swscale.lib\

LIBS += $${PWD}/ffmpeg-4.2.2-win64-dev/lib/avcodec.lib\
        $${PWD}/ffmpeg-4.2.2-win64-dev/lib/avdevice.lib\
        $${PWD}/ffmpeg-4.2.2-win64-dev/lib/avfilter.lib\
        $${PWD}/ffmpeg-4.2.2-win64-dev/lib/avformat.lib\
        $${PWD}/ffmpeg-4.2.2-win64-dev/lib/avutil.lib\
        $${PWD}/ffmpeg-4.2.2-win64-dev/lib/postproc.lib\
        $${PWD}/ffmpeg-4.2.2-win64-dev/lib/swresample.lib\
        $${PWD}/ffmpeg-4.2.2-win64-dev/lib/swscale.lib\
        $${PWD}/SDL2-2.0.10/lib/x64/SDL2.lib
        #$${PWD}/SDL3-devel-3.2.10-VC/SDL3-3.2.10/lib/x64/SDL3.lib

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    picture/defaultimage.png \
    picture/start.jpg

RESOURCES += \
    picture.qrc
