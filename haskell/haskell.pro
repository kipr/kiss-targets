include(../config.pri)

NAME=haskell
VER=1

TEMPLATE = lib
CONFIG += plugin
TARGET = $$qtLibraryTarget(haskell_plugin)
DEPENDPATH += src
INCLUDEPATH += src
DESTDIR = .

win32:TARGET = $$qtLibraryTarget(libhaskell_plugin)

CONFIG -= debug debug_and_release
CONFIG += release
INCLUDEPATH += ../libraries/cbcserial

LIBS += -L../libraries/cbcserial -lcbcserial

HEADERS += Haskell.h
SOURCES += Haskell.cpp

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

HASKELL_TARGET_FILE_INSTALL = ../$${INSTALL_BASE}/targets/haskell/haskell.target

target_base.files = haskell.api
target_base.path = ../$${INSTALL_BASE}/targets/haskell
target_base.extra = cp haskell.target $${HASKELL_TARGET_FILE_INSTALL}

target.path = ../$${INSTALL_BASE}/targets/haskell

INSTALLS += target target_base

#############
# Templates #
#############

templates.path = ../$${INSTALL_BASE}/targets/haskell
templates.extra = rm -Rf ../$${INSTALL_BASE}/targets/haskell/templates; cp -r templates ../$${INSTALL_BASE}/targets/haskell/templates

INSTALLS += templates

include (../install.pri)