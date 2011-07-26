include(../config.pri)

NAME=python
VER=1

TEMPLATE = lib
CONFIG += plugin
TARGET = $$qtLibraryTarget(python_plugin)
DEPENDPATH += src
INCLUDEPATH += src
DESTDIR = .

win32:TARGET = $$qtLibraryTarget(libpython_plugin)

CONFIG -= debug debug_and_release
CONFIG += release
INCLUDEPATH += ../libraries/cbcserial

LIBS += -L../libraries/cbcserial -lcbcserial

HEADERS += Python.h
SOURCES += Python.cpp

QMAKE_CLEAN += $(DESTDIR)/$(TARGET) src/*~

#################
# Extra targets #
#################
lib_dir.target = lib
lib_dir.commands = mkdir -p lib


QMAKE_EXTRA_TARGETS += lib_dir include_dir
POST_TARGETDEPS += $$lib_dir.target $$include_dir.target

######################
# Install Directives #
######################

PYTHON_TARGET_FILE_INSTALL = ../$${INSTALL_BASE}/targets/python/python.target

target_base.files = python.api
target_base.path = ../$${INSTALL_BASE}/targets/python
target_base.extra = cp python.target $${PYTHON_TARGET_FILE_INSTALL}

target.path = ../$${INSTALL_BASE}/targets/python

INSTALLS += target target_base

#############
# Templates #
#############

templates.path = ../$${INSTALL_BASE}/targets/python
templates.extra = rm -Rf ../$${INSTALL_BASE}/targets/python/templates; cp -r templates ../$${INSTALL_BASE}/targets/python/templates

INSTALLS += templates

include (../install.pri)