#-------------------------------------------------
#
# Project created by QtCreator 2014-10-04T08:55:50
#
#-------------------------------------------------

QT       += core gui xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ComicMetaEditor
TEMPLATE = app


SOURCES +=\
    FileUtility.cpp \
    FunctionalGraphicsView.cpp \
    Main.cpp \
    MainWindow.cpp \
    ComicMetaEditorSetting.cpp \
    CommonFunction.cpp \
    GraphicsItemData.cpp \
    ComicMetadata.cpp

HEADERS  += \
    Common.h \
    FileUtility.h\
    FunctionalGraphicsView.h \
    MainWindow.h \
    ComicMetaEditorSetting.h \
    CommonFunction.h \
    GraphicsItemData.h \
    ComicMetadata.h


FORMS    += \
    MainWindow.ui

RESOURCES += \
    Icon.qrc
