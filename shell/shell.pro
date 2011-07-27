include(../config.pri)

NAME=shell
VER=1

TEMPLATE = lib
CONFIG += plugin
TARGET = $$qtLibraryTarget($${NAME}_plugin)
DEPENDPATH += src
INCLUDEPATH += src
DESTDIR = .

win32:TARGET = $$qtLibraryTarget(lib$${NAME}_plugin)

CONFIG -= debug debug_and_release
CONFIG += release
INCLUDEPATH += ../libraries/cbcserial

LIBS += -L../libraries/cbcserial -lcbcserial

HEADERS += Shell.h
SOURCES += Shell.cpp

QMAKE_CLEAN += $(DESTDIR)/$(TARGET) src/*~

######################
# Install Directives #
######################

TARGET_FILE_INSTALL = ../$${INSTALL_BASE}/targets/$${NAME}/$${NAME}.target

target_base.files = $${NAME}.api
target_base.path = ../$${INSTALL_BASE}/targets/$${NAME}
target_base.extra = cp $${NAME}.target $${TARGET_FILE_INSTALL}

target.path = ../$${INSTALL_BASE}/targets/$${NAME}

INSTALLS += target target_base

#############
# Templates #
#############

templates.path = ../$${INSTALL_BASE}/targets/$${NAME}
templates.extra = rm -Rf ../$${INSTALL_BASE}/targets/$${NAME}/templates; cp -r templates ../$${INSTALL_BASE}/targets/$${NAME}/templates

INSTALLS += templates

include (../install.pri)