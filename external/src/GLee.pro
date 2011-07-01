include(../../config.pri)

TEMPLATE = lib
TARGET = GLee 
DEPENDPATH += GLee 
INCLUDEPATH += GLee

DESTDIR = ../lib

CONFIG -= qt
CONFIG += staticlib

# Input
HEADERS += GLee.h
SOURCES += GLee.c

gl_dir.target = ../include/GL
copy_header.target = ../include/GL/GLee.h

gl_dir.commands = mkdir -p ../include/GL
copy_header.commands = cp GLee/GLee.h ../include/GL

QMAKE_EXTRA_TARGETS = gl_dir copy_header
POST_TARGETDEPS += $$gl_dir.target $$copy_header.target

QMAKE_CLEAN += $${DESTDIR}/lib$${TARGET}.a ../include/GL/GLee.h
