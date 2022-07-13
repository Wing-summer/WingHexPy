#-------------------------------------------------
#
# Project created by QtCreator 2022-07-12T10:53:26
#
#-------------------------------------------------

QT       += core gui dtkwidget

TARGET = WingHexPy
TEMPLATE = lib
CONFIG += plugin

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    winghexpy.cpp \
    plginterface.cpp \
    QCodeEditor/QCodeEditor.cpp \
    QCodeEditor/QFramedTextAttribute.cpp \
    QCodeEditor/QLineNumberArea.cpp \
    QCodeEditor/QPythonCompleter.cpp \
    QCodeEditor/QPythonHighlighter.cpp \
    QCodeEditor/QStyleSyntaxHighlighter.cpp \
    QCodeEditor/QSyntaxStyle.cpp \
    QCodeEditor/QLanguage.cpp \
    scriptwindow.cpp

HEADERS += \
    winghexpy.h \
    plginterface.h\
    ../WingHexExplorer/wing-hex-explorer.sourcecode/WingHexExplorer/plugin/iwingplugin.h \
    QCodeEditor/QCodeEditor.hpp \
    QCodeEditor/QFramedTextAttribute.hpp \
    QCodeEditor/QLineNumberArea.hpp \
    QCodeEditor/QPythonCompleter.hpp \
    QCodeEditor/QPythonHighlighter.hpp \
    QCodeEditor/QStyleSyntaxHighlighter.hpp \
    QCodeEditor/QSyntaxStyle.hpp \
    QCodeEditor/QHighlightBlockRule.hpp \
    QCodeEditor/QHighlightRule.hpp \
    QCodeEditor/QLanguage.hpp \
    scriptwindow.h

DISTFILES += WingHexPy.json 

TRANSLATIONS += \
    $$PWD/WingHexPy.ts

INCLUDEPATH += -I /usr/include/python3.7
LIBS += /usr/lib/x86_64-linux-gnu/libpython3.7m.so.1

FORMS +=

RESOURCES += \
    resources.qrc
