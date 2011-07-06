include(../../config.pri)

TEMPLATE = lib

CONFIG -= debug debug_and_release
CONFIG += staticlib release

SOURCES += Gdb.cpp
HEADERS += Gdb.h